
#include "xint_internal.h"

#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>
#include <assert.h>

const xword_t k163_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_Gx[] = { X(0x5c94eee8, 0xde4e6d5e), X(0xaa07d793, 0x7bbc11ac), X(0xfe13c053, 0x02) };
const xword_t k163_Gy[] = { X(0xccdaa3d9, 0x0536d538), X(0x321f2e80, 0x5d38ff58), X(0x89070fb0, 0x02) };
const xword_t k163_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k163_h[]  = { 0x02 };
const xint_ecc_curve_t k163 =
{
    163,
    CURVE_WORDS(163),
    k163_p,
    k163_a,
    k163_b,
    k163_Gx,
    k163_Gy,
    k163_n,
    k163_h,
    xint_mod_fast_k163
};

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

void xint_mod_fast_k163(xword_t *w, xword_t *u)
{
    //xint_mod_std(w, u, &k163);
}

void xint_mod_std(xword_t *w, xword_t *u, const xint_ecc_curve_t *c)
{
    xword_t q[40];
    int mul_sz = 2 * c->nwords;
    while(u[mul_sz - 1] == 0)
    {
        --mul_sz;
    }
    int cmp;
    if (mul_sz < c->nwords)
    {
        cmp = -1;
    }
    else if (mul_sz == c->nwords)
    {
        cmp = xll_cmp(u, c->p, c->nwords);
    }
    else
    {
        cmp = 1;
    }
    if (cmp >= 0)
    {
        int n = c->nwords;
        int m = mul_sz - c->nwords;
        u[mul_sz] = 0;
        xll_div(q, u, c->p, m, n);
    }
    xll_move(w, u, c->nwords);
}

static void inline xint_ecc_mod_add(xword_t *w, const xword_t *u, const xword_t *v, const xint_ecc_curve_t *c)
{
    xword_t k = xll_add(w, u, v, c->nwords);
    if (k || xll_cmp(w, c->p, c->nwords) >= 0)
    {
        xll_sub(w, w, c->p, c->nwords);
    }
}

static void inline xint_ecc_mod_sub(xword_t *w, const xword_t *u, const xword_t *v, const xint_ecc_curve_t *c)
{
    xword_t b = xll_sub(w, u, v, c->nwords);
    if (b)
    {
        xll_add(w, w, c->p, c->nwords);
    }
}

static void inline xint_ecc_mod_mul(xword_t *w, const xword_t *u, const xword_t *v, const xint_ecc_curve_t *c)
{
    xword_t tmp[40];
    xll_mul(tmp, u, c->nwords, v, c->nwords);
    c->xint_mod_fast(w, tmp);
}

static void inline xint_ecc_mod_mul_ulong(xword_t *w, const xword_t *u, xword_t v, const xint_ecc_curve_t *c)
{
    xword_t k = xll_mul_1(w, u, c->nwords, v);
    while (k > 0 || xll_cmp(w, c->p, c->nwords) >= 0)
    {
        k -= xll_sub(w, w, c->p, c->nwords);
    }
}

static void inline xint_ecc_mod_lshift(xword_t *w, const xword_t *u, int nbits, const xint_ecc_curve_t *c)
{
    xword_t k = x_lshift(w, u, c->nwords, nbits);
    while (k > 0 || xll_cmp(w, c->p, c->nwords) >= 0)
    {
        k -= xll_sub(w, w, c->p, c->nwords);
    }
}

static void inline xint_ecc_mod_rshift(xword_t *w, const xword_t *u, int nbits, const xint_ecc_curve_t *c)
{
    xword_t k = x_rshift(w, u, c->nwords, nbits);
    while (k > 0 || xll_cmp(w, c->p, c->nwords) >= 0)
    {
        k -= xll_sub(w, w, c->p, c->nwords);
    }
}

static void inline xint_ecc_mod_sqr(xword_t *w, const xword_t *u, const xint_ecc_curve_t *c)
{
    xword_t tmp[40];
    xll_squ(tmp, u, c->nwords);
    c->xint_mod_fast(w, tmp);
}

void ecc_zaddu(xint_ecc_point_jacobian_t Rj, xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c)
{
    xword_t *T1 = Rj->x;
    xword_t *T2 = Rj->y;
    xword_t *T3 = Rj->z;
    xword_t *T4 = Pj->x;
    xword_t *T5 = Pj->y;
    xword_t T6[c->nwords];
    
    xint_ecc_mod_sub(T6, T1, T4, c);
    xint_ecc_mod_mul(T3, T3, T6, c);
    xint_ecc_mod_sqr(T6, T6, c);
    xint_ecc_mod_mul(T1, T1, T6, c);
    xint_ecc_mod_mul(T6, T6, T4, c);

    xint_ecc_mod_sub(T5, T2, T5, c);
    xint_ecc_mod_sqr(T4, T5, c);
    xint_ecc_mod_sub(T4, T4, T1, c);
    xint_ecc_mod_sub(T4, T4, T6, c);
    xint_ecc_mod_sub(T6, T1, T6, c);

    xint_ecc_mod_mul(T2, T2, T6, c);
    xint_ecc_mod_sub(T6, T1, T4, c);
    xint_ecc_mod_mul(T5, T5, T6, c);
    xint_ecc_mod_sub(T5, T5, T2, c);
    
    xll_move(Pj->z, T3, c->nwords);
}

void ecc_zdau(xint_ecc_point_jacobian_t Rj, xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c)
{
    xword_t *T1 = Rj->x;
    xword_t *T2 = Rj->y;
    xword_t *T3 = Rj->z;
    xword_t *T4 = Pj->x;
    xword_t *T5 = Pj->y;
    xword_t T6[c->nwords];
    xword_t T7[c->nwords];
    xword_t T8[c->nwords];
    
    // 1
    xint_ecc_mod_sub(T6, T1, T4, c);        // T6 = X1 - X2
    xint_ecc_mod_sqr(T7, T6, c);            // T7 = (X1 - X2) ^2
    xint_ecc_mod_mul(T1, T1, T7, c);        // T1 = X1 * (X1 - X2) ^ 2
    xint_ecc_mod_mul(T4, T4, T7, c);        // T2 = X2 * (X1 - X2) ^ 2
    xint_ecc_mod_sub(T5, T2, T5, c);        //
    
    // 6
    xint_ecc_mod_sub(T8, T1, T4, c);
    xint_ecc_mod_mul(T2, T2, T8, c);
    xint_ecc_mod_lshift(T2, T2, 1, c);
    xint_ecc_mod_sqr(T8, T5, c);
    xint_ecc_mod_sub(T4, T8, T4, c);
    
    // 11
    xint_ecc_mod_sub(T4, T4, T1, c);
    xint_ecc_mod_sub(T4, T4, T1, c);
    xint_ecc_mod_add(T6, T4, T6, c);
    xint_ecc_mod_sqr(T6, T6, c);
    xint_ecc_mod_sub(T6, T6, T7, c);
    
    // 16
    xint_ecc_mod_sub(T5, T5, T4, c);
    xint_ecc_mod_sqr(T5, T5, c);
    xint_ecc_mod_sub(T5, T5, T8, c);
    xint_ecc_mod_sub(T5, T5, T2, c);
    xint_ecc_mod_sqr(T7, T4, c);
    
    // 21
    xint_ecc_mod_sub(T5, T5, T7, c);
    xint_ecc_mod_lshift(T8, T7, 2, c);
    xint_ecc_mod_sub(T6, T6, T7, c);
    xint_ecc_mod_mul(T3, T3, T6, c);
    xint_ecc_mod_mul(T6, T1, T8, c);
    
    // 26
    xint_ecc_mod_add(T1, T1, T4, c);
    xint_ecc_mod_mul(T8, T8, T1, c);
    xint_ecc_mod_add(T7, T2, T5, c);
    xint_ecc_mod_sub(T2, T5, T2, c);
    xint_ecc_mod_sub(T1, T8, T6, c);
    
    // 31
    xint_ecc_mod_mul(T5, T5, T1, c);
    xint_ecc_mod_add(T6, T6, T8, c);
    xint_ecc_mod_sqr(T1, T2, c);
    xint_ecc_mod_sub(T1, T1, T6, c);
    xint_ecc_mod_sub(T4, T8, T1, c);
    
    // 36
    xint_ecc_mod_mul(T2, T2, T4, c);
    xint_ecc_mod_sub(T2, T2, T5, c);
    xint_ecc_mod_sqr(T4, T7, c);
    xint_ecc_mod_sub(T4, T4, T6, c);
    xint_ecc_mod_sub(T8, T8, T4, c);
    
    // 41
    xint_ecc_mod_mul(T7, T7, T8, c);
    xint_ecc_mod_sub(T5, T7, T5, c);
    
    xll_move(Pj->z, T3, c->nwords);
}

void ecc_dblu(xint_ecc_point_jacobian_t Rj, xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c)
{
    // 4M + 6S
    
    xword_t *x1 = Pj->x;
    xword_t *y1 = Pj->y;
    xword_t *z1 = Pj->z;
    xword_t *x3 = Rj->x;
    xword_t *y3 = Rj->y;
    xword_t *z3 = Rj->z;

    // Use algorithm from Wikibooks
    xword_t S[c->nwords];
    xword_t M[c->nwords];
    xword_t tmp[c->nwords];

    // M = 3.x1^2 + a.z1^4
    xint_ecc_mod_sqr(M, x1, c);
    xint_ecc_mod_mul_ulong(M, M, 3, c);
    xint_ecc_mod_sqr(tmp, z1, c);
    xint_ecc_mod_sqr(tmp, tmp, c);
    xint_ecc_mod_mul(tmp, tmp, c->a, c);
    xint_ecc_mod_add(M, M, tmp, c);
    
    // S = 4.x1.y1^2
    xint_ecc_mod_sqr(tmp, y1, c);
    xint_ecc_mod_mul(S, tmp, x1, c);
    xint_ecc_mod_lshift(S, S, 2, c);
    
    // x3 = M^2 - 2.S
    xint_ecc_mod_sqr(x3, M, c);
    xint_ecc_mod_sub(x3, x3, S, c);
    xint_ecc_mod_sub(x3, x3, S, c);
    
    // y3 = M.(S - x3) - 8.y^4
    xint_ecc_mod_sub(y3, S, x3, c);
    xint_ecc_mod_mul(y3, y3, M, c);
    xint_ecc_mod_sqr(tmp, tmp, c);
    xint_ecc_mod_lshift(tmp, tmp, 3, c);
    xint_ecc_mod_sub(y3, y3, tmp, c);
    
    // z3 = 2.y1.z1
    xll_move(z3, y1, c->nwords);
    xint_ecc_mod_mul(z3, z3, z1, c);
    xint_ecc_mod_lshift(z3, z3, 1, c);
    
    // Now update Pj
    xll_move(x1, S, c->nwords);
    xll_move(y1, tmp, c->nwords);
    xll_move(z1, z3, c->nwords);
    Rj->is_at_infinity = 0;
}

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xword_t *Px, const xword_t *Py, const xint_t k_in, const xint_ecc_curve_t *c)
{
    // Force b0 of k to be a 1
    xint_t k = XINT_INIT_VAL;
    xint_copy(k, k_in);
    if (xint_get_bit(k, 0) == 0)
    {
        xword_t ret = xll_add(k->data, k->data, c->n, c->nwords);
        if (ret)
        {
            resize(k, k->size+1);
            k->data[k->size-1] = ret;
        }
    }
    
    int nbits = xint_highest_bit_num(k) + 1;
    
    xint_ecc_point_jacobian_t Rj[2];
    xint_point_jacobian_init(Rj[0], c->nwords);
    xint_point_jacobian_init(Rj[1], c->nwords);

    int bit = xint_get_bit(k, 1);
    
    // To Jacobian
    xll_move(Rj[bit]->x, Px, c->nwords);
    xll_move(Rj[bit]->y, Py, c->nwords);
    xll_zero(Rj[bit]->z, c->nwords);
    Rj[bit]->z[0] = 1;
    Rj[bit]->is_at_infinity = 0;
    
    // 3P = 2P + P
    //
    // (2P, P') =  DBLU(P),
    // 3P = ZADDU(P', 2P)
    ecc_dblu(Rj[1-bit], Rj[bit], c);
    ecc_zaddu(Rj[bit], Rj[1-bit], c);

    for (int i=2; i<nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
        ecc_zdau(Rj[1-bit], Rj[bit], c);
    }

    from_jacobian(R, Rj[0], c);
    xint_delete(k);
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
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - p256.nbits);
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
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - p256.nbits);
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
            xint_ecc_point_add(R, R, TMP, p);
        }
        xint_ecc_point_double(TMP, TMP, a, p);
    }
}

void xint_mod_add(xint_t w, const xint_t u, const xint_t v, const xint_t m)
{
    xint_add(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_sub(xint_t w, const xint_t u, const xint_t v, const xint_t m)
{
    xint_sub(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_mul(xint_t w, const xint_t u, const xint_t v, const xint_t m)
{
    xint_mul(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_mul_ulong(xint_t w, const xint_t u, unsigned long v, const xint_t m)
{
    xint_mul_ulong(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_lshift(xint_t w, const xint_t u, int nbits, const xint_t m)
{
    xint_lshift(w, u, nbits);
    xint_mod(w, w, m);
}

void xint_mod_sqr(xint_t w, const xint_t u, const xint_t m)
{
    xint_sqr(w, u);
    xint_mod(w, w, m);
}

void xint_ecc_point_add(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m)
{
    xint_t diffy = XINT_INIT_VAL;
    xint_t diffx = XINT_INIT_VAL;
    xint_t lambda = XINT_INIT_VAL;
    xint_t xr = XINT_INIT_VAL;
    xint_t yr = XINT_INIT_VAL;
    
    if (p->is_at_infinity)
    {
        xint_point_copy(r, q);
        return;
    }

    if (q->is_at_infinity)
    {
        xint_point_copy(r, p);
        return;
    }

    xint_mod_sub(diffy, q->y, p->y, m);
    xint_mod_sub(diffx, q->x, p->x, m);
    xint_mod_inverse(diffx, diffx, m);
    xint_mod_mul(lambda, diffy, diffx, m);

    xint_mod_sqr(xr, lambda, m);
    xint_mod_sub(xr, xr, p->x, m);
    xint_mod_sub(xr, xr, q->x, m);

    xint_mod_sub(yr, p->x, xr, m);
    xint_mod_mul(yr, yr, lambda, m);
    xint_mod_sub(yr, yr, p->y, m);

    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
    r->is_at_infinity = 0;

    xint_delete(diffx);
    xint_delete(diffy);
    xint_delete(lambda);
    xint_delete(xr);
    xint_delete(yr);
}

void xint_ecc_point_double(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m)
{
    xint_t tmp = XINT_INIT_VAL;
    xint_t lambda = XINT_INIT_VAL;
    xint_t xr = XINT_INIT_VAL;
    xint_t yr = XINT_INIT_VAL;

    xint_mod_sqr(tmp, p->x, m);
    xint_mod_mul_ulong(tmp, tmp, 3, m);
    xint_mod_add(tmp, tmp, a, m);

    xint_lshift(lambda, p->y, 1);
    xint_mod_inverse(lambda, lambda, m);
    xint_mod_mul(lambda, tmp, lambda, m);

    xint_mod_sqr(xr, lambda, m);
    xint_mod_sub(xr, xr, p->x, m);
    xint_mod_sub(xr, xr, p->x, m);

    xint_mod_sub(tmp, p->x, xr, m);
    xint_mod_mul(yr, tmp, lambda, m);
    xint_mod_sub(yr, yr, p->y, m);

    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
    r->is_at_infinity = 0;
    
    xint_delete(tmp);
    xint_delete(lambda);
    xint_delete(xr);
    xint_delete(yr);
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
    
    xint_mod_sqr(w->x, z_inv, p);
    xint_mod_mul(w->y, w->x, z_inv, p);
    xint_mod_mul(w->x, w->x, u_x, p);
    xint_mod_mul(w->y, w->y, u_y, p);
    
    w->is_at_infinity = 0;
    xint_delete(z_inv);
}

void xint_point_add_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c)
{
    if (Pj->is_at_infinity)
    {
        xint_point_jacobian_copy(Rjx, Qj);
        return;
    }
    
    if (Qj->is_at_infinity)
    {
        xint_point_jacobian_copy(Rjx, Pj);
        return;
    }

    xword_t T1[10];
    xword_t T2[10];
    xword_t T3[10];
    xword_t T4[10];
    xword_t T5[10];
    xword_t T6[10];
    xword_t T7[10];

    xll_move(T1, Pj->x, Pj->nwords);
    xll_move(T2, Pj->y, Pj->nwords);
    xll_move(T3, Pj->z, Pj->nwords);
    xll_move(T4, Qj->x, Qj->nwords);
    xll_move(T5, Qj->y, Qj->nwords);
    xll_move(T6, Qj->z, Qj->nwords);
    
    xint_ecc_mod_sqr(T7, T3, c);        // T7 = z1^2
    xint_ecc_mod_mul(T4, T4, T7, c);    // T4 = x2.z1^2 = U2
    xint_ecc_mod_mul(T5, T5, T3, c);    // T5 = y2.z1
    xint_ecc_mod_mul(T5, T5, T7, c);    // T5 = y2.z1^3 = S2
    xint_ecc_mod_sqr(T7, T6, c);        // T7 = z2^2z
    
    xint_ecc_mod_mul(T1, T1, T7, c);    // T1 = x1.z2^2 = U1
    xint_ecc_mod_mul(T2, T2, T6, c);    // T2 = y1.z2
    xint_ecc_mod_mul(T2, T2, T7, c);    // T2 = y1.z2^3 = S1
    xint_ecc_mod_sub(T1, T1, T4, c);    // T1 = U1 - U2 = -H
    xint_ecc_mod_mul(T3, T6, T3, c);    // T3 = z2.z
    
    xint_ecc_mod_mul(T3, T1, T3, c);    // T3 = -H.z1.z2 = Z3 ***
    
    xint_ecc_mod_sub(T2, T2, T5, c);    // T2 = S1 - S2 = -R
    xint_ecc_mod_sqr(T7, T1, c);        // T7 = H^2
    xint_ecc_mod_sqr(T6, T2, c);        // T6 = R^2
    xint_ecc_mod_mul(T4, T4, T7, c);    // T4 = U2.H^2
    xint_ecc_mod_mul(T1, T7, T1, c);    // T1 = -H^3

    xint_ecc_mod_sub(T6, T6, T1, c);    // T6 = R^2 - (-H^3)
    xint_ecc_mod_mul_ulong(T7, T4, 2, c); // T7 = 2.U2.H^2
    xint_ecc_mod_sub(T6, T6, T7, c);    // T6 = R^2 - (-H^3) - 2.U2.H^2 = X3 ***
    xint_ecc_mod_sub(T4, T4, T6, c);    // T4 = U2.H^2 - X3
    xint_ecc_mod_mul(T2, T2, T4, c);    // T2 = -R(U2.H^2 - X3)

    xint_ecc_mod_mul(T7, T5, T1, c);    // T7 = -S2.H^3
    xint_ecc_mod_sub(T7, T2, T7 , c);   // T7 = -R(U2.H^2 - X3) - S2.H^3 = Y3 ***

    xll_move(Rjx->x, T6, Pj->nwords);
    xll_move(Rjx->y, T7, Pj->nwords);
    xll_move(Rjx->z, T3, Pj->nwords);
    Rjx->is_at_infinity = 0;
}

void xint_point_double_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c)
{
    // 4M + 6S
    if (Pj->is_at_infinity)
    {
        xint_point_jacobian_copy(Rjx, Pj);
        return;
    }

    xword_t T1[10];
    xword_t T2[10];
    xword_t T3[10];
    xword_t T4[10];
    xword_t T5[10];

    xll_move(T1, Pj->x, Pj->nwords);
    xll_move(T2, Pj->y, Pj->nwords);
    xll_move(T3, Pj->z, Pj->nwords);
    
    xint_ecc_mod_sqr(T4, T2, c);        // T4 = Y^2
    xint_ecc_mod_mul(T5, T1, T4, c);    // T5 = X.Y^2
#if !defined LAMBDA_2
    xint_ecc_mod_mul_ulong(T5, T5, 4, c);// T5 = 4.X.Y^2 = S***
#endif
    xint_ecc_mod_sqr(T4, T4, c);        // T4 = Y^4
#if !defined LAMBDA_2
    xint_ecc_mod_mul_ulong(T4, T4, 8, c);// T4 = 8.Y^4
#endif
    xint_ecc_mod_mul(T2, T2, T3, c);    // T2 = Y.Z
#if !defined LAMBDA_2
    xint_ecc_mod_mul_ulong(T2, T2, 2, c);// T2 = 2.Y.Z
#endif
    xint_ecc_mod_sqr(T3, T3, c);        // T3 = Z^2

    xint_ecc_mod_add(T1, T1, T3, c);    // T1 = X + Z^2
    xint_ecc_mod_add(T3, T3, T3, c);    // T3 = 2.Z^2
    xint_ecc_mod_sub(T3, T1, T3, c);    // T3 = X - Z^2
    xint_ecc_mod_mul(T1, T1, T3, c);    // T1 = X^2 - Z^4
    
    xint_ecc_mod_add(T3, T1, T1, c);    // T3 = 2.(X^2 - Z^4)
    xint_ecc_mod_add(T1, T1, T3, c);    // T1 = 3.(X^2 - Z^4) = M***

#if defined LAMBDA_2
xint_ecc_mod_rshift(T1, T1, 1, c);
#endif
    xint_ecc_mod_sqr(T3, T1, c);        // T3 = M^2
    xint_ecc_mod_sub(T3, T3, T5, c);    // T3 = M^2 - S

    xint_ecc_mod_sub(T3, T3, T5, c);    // T3 = M^2 - 2.S = X3***
    xint_ecc_mod_sub(T5, T5, T3, c);    // T5 = S - X'
    xint_ecc_mod_mul(T1, T1, T5, c);    // T1 = M.(S - X')
    xint_ecc_mod_sub(T1, T1, T4, c);    // T1 = M.(S - X') - 8.Y^4

    xll_move(Rjx->x, T3, Pj->nwords);
    xll_move(Rjx->y, T1, Pj->nwords);
    xll_move(Rjx->z, T2, Pj->nwords);
    Rjx->is_at_infinity = 0;
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
        xint_point_double_jacobian(Rj, Rj, c);
        if (xint_get_bit(k, i) == 1)
        {
            xint_point_add_jacobian(Rj, Rj, TMPj, c);
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
            xint_point_add_jacobian(Rj, Rj, TMPj, c);
        }
        xint_point_double_jacobian(TMPj, TMPj, c);
    }
    from_jacobian_ex(R, Rj, c);
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
        xint_point_double_jacobian(Rj[1-bit], Rj[1-bit], c);
        xint_point_add_jacobian(Rj[1-bit], Rj[1-bit], Rj[bit], c);
    }
    from_jacobian_ex(R, Rj[0], c);
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
    xint_point_add_jacobian(P[3], P[1], P[2], c);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj, c->nwords);

    // Left to right because it's easier to do the Shamir trick
    for (int i=c->nbits-1; i>=0; --i)
    {
        xint_point_double_jacobian(Rj, Rj, c);
        int bits = xint_get_bit(u2, i) << 1 | xint_get_bit(u1, i);
        if (bits)
        {
            xint_point_add_jacobian(Rj, Rj, P[bits], c);
        }
    }
    from_jacobian(R, Rj, c);
    xint_point_jacobian_delete(Rj);
    xint_point_jacobian_delete(P[1]);
    xint_point_jacobian_delete(P[2]);
    xint_point_jacobian_delete(P[3]);
}
