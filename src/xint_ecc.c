
#include "xint_internal.h"

#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#include "curves.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>
#include <assert.h>

void xint_point_init(xint_ecc_point_t p)
{
    p->is_at_infinity = 1;
    xint_init(p->x);
    xint_init(p->y);
}

void xint_point_delete(xint_ecc_point_t p)
{
    p->is_at_infinity = 1;
    xint_delete(p->x);
    xint_delete(p->y);
}

void xint_point_copy(xint_ecc_point_t r, const xint_ecc_point_t p)
{
    r->is_at_infinity = p->is_at_infinity;
    xint_copy(r->x, p->x);
    xint_copy(r->y, p->y);
}

void ecc_gen_deterministic_k(xint_t k, uint8_t *h1, int hlen, xint_t v_int, const xint_ecc_curve_t *c)
{
    xint_t q_int = XINT_INIT_VAL;
    int qlen = c->nbits;
    CONST_XINT_FROM_XWORDS(q_int, c->n, c->nwords);

    // a.  The hash is ready, just convert to an xint
    xint_t h1_int = XINT_INIT_VAL;
    xint_from_bin(h1_int, h1, 32);
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - c->nbits);
    xint_mod(h1_int, h1_int, q_int);

    int rolen = (qlen + 7) >> 3;
    int rlen = rolen * 8;
        
    // b. V = 0x01 0x01 0x01 ... 0x01 vlen = 8*ceil(hlen/8)
    uint8_t V[(hlen / 8 ) * 8];
    memset(V, 0x01, sizeof(V));
        
    // c. K = 0x00 0x00 0x00 ... 0x00
    uint8_t K[(hlen / 8 ) * 8];
    memset(K, 0x00, sizeof(K));

    // d. K = HMAC_K(V || 0x00 || int2octets(x) || bits2octets(h1))
    uint8_t tmp_vec[100];
    memset(tmp_vec, 0, sizeof(tmp_vec));
    memcpy(tmp_vec, V, sizeof(V));
    tmp_vec[32] = 0x00;
    xint_to_buf(tmp_vec + 33, rolen, v_int);
    xint_to_buf(tmp_vec + 33 + rolen, rolen, h1_int);
    hmac_calc(K, K, sizeof(K), tmp_vec, 33 + rolen + rolen);

    // e. V = HMAC_K(V)
    hmac_calc(V, K, sizeof(K), V, sizeof(V));

    // f. K = HMAC_K(V || 0x01 || int2octets(x) || bits2octets(h1))
    memset(tmp_vec, 0, sizeof(tmp_vec));
    memcpy(tmp_vec, V, 32);
    tmp_vec[32] = 0x01;
    xint_to_buf(tmp_vec + 33, rolen, v_int);
    xint_to_buf(tmp_vec + 33 + rolen, rolen, h1_int);
    hmac_calc(K, K, sizeof(K), tmp_vec, 33 + rolen + rolen);

    // g. V = HMAC_K(V)
    hmac_calc(V, K, sizeof(K), V, sizeof(V));

    // h.
    while (1)
    {
        // 1.  Set T to the empty sequence length tlen
        uint8_t T[100];
        int tlen = 0;
        // 2.  While tlen < qlen, do the following:
        while (tlen < qlen)
        {
            // V = HMAC_K(V)
            // T = T || V
            hmac_calc(V, K, sizeof(K), V, sizeof(V));
            memcpy(T + tlen, V, sizeof(V));
            tlen += sizeof(V) * 8;
        }
            
        // 3.  Compute:
        // k = bits2int(T)
        xint_from_bin(k, T, 32);
        xint_rshift(k, k, xint_size(k) * XWORD_BITS - qlen);
        
        // If 1 <= k <= q-1 done
        if (xint_cmp_ulong(k, 1) >= 0 && xint_cmp(k, q_int) < 0)
        {
            break;
        }
        else
        {
            // else compute:
            // K = HMAC_K(V || 0x00)
            // V = HMAC_K(V)
            uint8_t tmpV[33];
            memcpy(tmpV, V, sizeof(V));
            tmpV[32] = 0x00;
            hmac_calc(K, K, sizeof(K), tmpV, sizeof(tmpV));
            hmac_calc(V, K, sizeof(K), V, sizeof(V));
        }
    }
    xint_delete(h1_int);
}

int xint_ecc_get_public_key(xint_ecc_point_t pub, xint_t priv, const xint_ecc_curve_t *c)
{
    // Try and generate the public key
//    xint_ecc_point_t G;
//    xint_point_init(G);
//    CONST_XINT_FROM_XWORDS(G->x, c->Gx, c->nwords);
//    CONST_XINT_FROM_XWORDS(G->y, c->Gy, c->nwords);
//    G->is_at_infinity = 0;
//    xint_ecc_mul_scalar_plain(pub, G, priv, c);
    xint_ecc_mul_scalar(pub, c->Gx, c->Gy, priv, c);
    return 1;
}

int xint_ecc_sign_det(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_t priv, const xint_ecc_curve_t *c)
{
    xint_t k = XINT_INIT_VAL;
    ecc_gen_deterministic_k(k, digest, digest_len, priv, c);
              
    xint_ecc_point_t point;
    xint_point_init(point);
    xint_ecc_mul_scalar(point, c->Gx, c->Gy, k, c);

    xint_t N = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(N, c->n, c->nwords);

    // Invert k
    xint_t k_inv = XINT_INIT_VAL;
    xint_mod_inverse(k_inv, k, N);
    
    xint_t h1_int = XINT_INIT_VAL;
    xint_from_bin(h1_int, digest, digest_len);
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - c->nbits);
    xint_mod(h1_int, h1_int, N);
    //xint_print_hex("h1", h1_int);

    xint_mul(point->y, point->x, priv);
    xint_mod(point->y, point->y, N);
    xint_add(point->y, h1_int, point->y);
    xint_mod(point->y, point->y, N);
    xint_mul(point->y, k_inv, point->y);
    xint_mod(point->y, point->y, N);
    
    xint_copy(sig->r, point->x);
    xint_copy(sig->s, point->y);
    //xint_print_hex(" r", sig->r);
    //xint_print_hex(" s", sig->s);

    xint_delete(k);
    xint_delete(k_inv);
    xint_delete(h1_int);
    xint_point_delete(point);

    return 0;
}

int xint_ecc_verify(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_ecc_point_t pub, const xint_ecc_curve_t *c)
{
    xint_t N = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(N, c->n, c->nwords);
    xint_t P = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(P, c->p, c->nwords);

    xint_t h1_int = XINT_INIT_VAL;
    xint_from_bin(h1_int, digest, digest_len);
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - c->nbits);
    xint_mod(h1_int, h1_int, N);

    // Inverse of s
    xint_t s_inv = XINT_INIT_VAL;
    xint_mod_inverse(s_inv, sig->s, N);

    // u1 = z * inv_s
    xint_t u1 = XINT_INIT_VAL;
    xint_mul(u1, h1_int, s_inv);
    xint_mod(u1, u1, N);

    // u2 = r * inv_s
    xint_t u2 = XINT_INIT_VAL;
    xint_mul(u2, sig->r, s_inv);
    xint_mod(u2, u2, N);
    
    xint_ecc_point_t G;
    xint_point_init(G);
    CONST_XINT_FROM_XWORDS(G->x, c->Gx, c->nwords);
    CONST_XINT_FROM_XWORDS(G->y, c->Gy, c->nwords);

    xint_ecc_point_t p3;
    xint_point_init(p3);
    xint_ecc_mul_scalar_shamir(p3, pub, G, u1, u2, c);
    if (xint_cmp(p3->x, sig->r) == 0)
    {
        return 1;
    }
    return 0;
}

void xint_ecc_mul_scalar_plain(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c)
{
    xint_ecc_point_t TMP;
    xint_point_init(TMP);
    xint_point_copy(TMP, P);
    xint_t a = XINT_INIT_VAL;
    xint_t p = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(a, c->a, c->nwords);
    CONST_XINT_FROM_XWORDS(p, c->p, c->nwords);
    xint_assign_zero(R->x);
    xint_assign_zero(R->y);
    R->is_at_infinity = 1;
    for (int i=0; i<c->nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            c->affine_add(R, R, TMP, p);
        }
        c->affine_double(TMP, TMP, a, p);
    }
}

void xint_point_jacobian_init(xint_ecc_point_jacobian_t p, int nwords)
{
    p->x = (xword_t *)malloc(nwords * sizeof(xword_t));
    p->y = (xword_t *)malloc(nwords * sizeof(xword_t));
    p->z = (xword_t *)malloc(nwords * sizeof(xword_t));
    p->is_at_infinity = 1;
    p->nwords = nwords;
}

void xint_point_jacobian_delete(xint_ecc_point_jacobian_t p)
{
    p->is_at_infinity = 1;
    free(p->x);
    free(p->y);
    free(p->z);
}

void xint_point_jacobian_copy(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p)
{
    r->is_at_infinity = p->is_at_infinity;
    xll_move(r->x, p->x, r->nwords);
    xll_move(r->y, p->y, r->nwords);
    xll_move(r->z, p->z, r->nwords);
}

void to_jacobian(xint_ecc_point_jacobian_t w, const xint_ecc_point_t u)
{
    xll_zero(w->x, w->nwords);
    xll_zero(w->y, w->nwords);
    xll_zero(w->z, w->nwords);
    xll_move(w->x, u->x->data, u->x->size);
    xll_move(w->y, u->y->data, u->y->size);
    w->z[0] = 1;
    w->is_at_infinity = u->is_at_infinity;
}

void from_jacobian(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, const xint_ecc_curve_t *c)
{
    // Convert back to affine
    xint_t u_x = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(u_x, u->x, u->nwords);
    
    xint_t u_y = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(u_y, u->y, u->nwords);
    
    xint_t u_z = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(u_z, u->z, u->nwords);
    
    xint_t p = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(p, c->p, u->nwords);

    xint_t z_inv = XINT_INIT_VAL;
    xint_mod_inverse(z_inv, u_z, p);
    
    xint_mod_sqr(w->x, z_inv, p);       // x = 1/Z^2
    xint_mod_mul(w->y, w->x, z_inv, p); // y = 1/Z^3
    xint_mod_mul(w->x, w->x, u_x, p);   // x = X/Z^2
    xint_mod_mul(w->y, w->y, u_y, p);   // y = Y/Z^3
    
    w->is_at_infinity = 0;
    xint_delete(z_inv);
}

void xint_ecc_mul_scalar_jacobian(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c)
{
    xint_t a = XINT_INIT_VAL;
    xint_t p = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(a, c->a, c->nwords);
    CONST_XINT_FROM_XWORDS(p, c->p, c->nwords);
#if 1
    // Left to right
    xint_ecc_point_jacobian_t TMPj;
    xint_point_jacobian_init(TMPj, c->nwords);
    to_jacobian(TMPj, P);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj, c->nwords);
    for (int i=c->nbits-1; i>=0; --i)
    {
        c->jacobian_double(Rj, Rj, c);
        if (xint_get_bit(k, i) == 1)
        {
            c->jacobian_add(Rj, Rj, TMPj, c);
        }
    }
    from_jacobian(R, Rj, c);
    xint_point_jacobian_delete(TMPj);
    xint_point_jacobian_delete(Rj);
#elif 0
    // Right to left
    xint_ecc_point_jacobian_t TMPj;
    xint_point_jacobian_init(TMPj, c->nwords);
    to_jacobian(TMPj, P);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj, c->nwords);
    for (int i=0; i<c->nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            c->jacobian_add(Rj, Rj, TMPj, c);
        }
        c->jacobian_double(TMPj, TMPj, c);
    }
    from_jacobian(R, Rj, c);
    xint_point_jacobian_delete(TMPj);
    xint_point_jacobian_delete(Rj);
#else
    xint_ecc_point_jacobian_t Rj[2];
    xint_point_jacobian_init(Rj[0], c->nwords);
    xint_point_jacobian_init(Rj[1], c->nwords);
    to_jacobian(Rj[1], P);
    for (int i=0; i<c->nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
        c->jacobian_double(Rj[1-bit], Rj[1-bit], c);
        c->jacobian_add(Rj[1-bit], Rj[1-bit], Rj[bit], c);
    }
    from_jacobian(R, Rj[0], c);
    xint_point_jacobian_delete(Rj[0]);
    xint_point_jacobian_delete(Rj[1]);
#endif
}

void xint_ecc_mul_scalar_shamir(xint_ecc_point_t R, const xint_ecc_point_t S, const xint_ecc_point_t G, const xint_t u1, const xint_t u2, const xint_ecc_curve_t *c)
{
    xint_ecc_point_jacobian_t P[4];
    xint_point_jacobian_init(P[1], c->nwords);
    to_jacobian(P[1], G);
    P[1]->is_at_infinity = 0;

    xint_point_jacobian_init(P[2], c->nwords);
    to_jacobian(P[2], S);
    P[2]->is_at_infinity = 0;

    xint_point_jacobian_init(P[3], c->nwords);
    c->jacobian_add(P[3], P[1], P[2], c);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj, c->nwords);

    // Left to right because it's easier to do the Shamir trick
    for (int i=c->nbits-1; i>=0; --i)
    {
        c->jacobian_double(Rj, Rj, c);
        int bits = xint_get_bit(u2, i) << 1 | xint_get_bit(u1, i);
        if (bits)
        {
            c->jacobian_add(Rj, Rj, P[bits], c);
        }
    }
    from_jacobian(R, Rj, c);
    xint_point_jacobian_delete(Rj);
    xint_point_jacobian_delete(P[1]);
    xint_point_jacobian_delete(P[2]);
    xint_point_jacobian_delete(P[3]);
}
