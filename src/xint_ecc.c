
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"

void xint_gcd(xint_t w, const xint_t u, const xint_t v)
{
    // Knuths Algorithm A (Modern Euclidean algorithm)
    xint_t tmp_u = XINT_INIT_VAL;
    xint_t tmp_v = XINT_INIT_VAL;
    xint_t r = XINT_INIT_VAL;
    xint_copy(tmp_u, u);
    xint_copy(tmp_v, v);
    while (1)
    {
        // A1. [v = 0?] If v = 0, the algorithm terminates with u as the answer.
        if (xint_is_zero(tmp_v))
        {
            break;
        }
        // A2.[Take u mod v.]
        // Set r ← u mod v
        xint_mod(r, tmp_u, tmp_v);
        // Set u ← v
        xint_copy(tmp_u, tmp_v);
        // Set v ← r,
        xint_copy(tmp_v, r);
    }

    xint_copy(w, tmp_u);

    xint_delete(tmp_u);
    xint_delete(tmp_v);
    xint_delete(r);
}

void xint_gcd_ext(xint_t gcd, xint_t ud, xint_t vd, const xint_t u, const xint_t v)
{
    // Knuths Algorithm X (Extended Euclid’s algorithm)
    xint_t q = XINT_INIT_VAL;
    xint_t r = XINT_INIT_VAL;
    
    xint_t t1 = XINT_INIT_VAL;
    xint_t t2 = XINT_INIT_VAL;
    xint_t t3 = XINT_INIT_VAL;
    
    xint_t u1 = XINT_INIT_VAL;
    xint_t u2 = XINT_INIT_VAL;
    xint_t u3 = XINT_INIT_VAL;
    
    xint_t v1 = XINT_INIT_VAL;
    xint_t v2 = XINT_INIT_VAL;
    xint_t v3 = XINT_INIT_VAL;
    
    // X1. [Initialize.] Set (u1, u2, u3) ← (1, 0, u), (v1, v2, v3) ← (0, 1, v)
    xint_assign_long(u1, 1);
    xint_assign_zero(u2);
    xint_copy(u3, u);
    
    xint_assign_zero(v1);
    xint_assign_long(v2, 1);
    xint_copy(v3, v);
    
    while (1)
    {
        // X2. [Is v3 = 0?] If v3 = 0, the algorithm terminates.
        if (xint_is_zero(v3))
        {
            break;
        }
        
        // X3. [Divide, subtract.]
        // Set q ← ⌊u3/v3⌋, and then set
        xint_div(q, r, u3, v3);
        
        // (t1, t2, t3) ← (u1, u2, u3) − (v1, v2, v3) q
        xint_mul(t1, v1, q);
        xint_mul(t2, v2, q);
        xint_mul(t3, v3, q);

        xint_sub(t1, u1, t1);
        xint_sub(t2, u2, t2);
        xint_sub(t3, u3, t3);
        
        // (u1, u2, u3) ← (v1, v2, v3)
        xint_copy(u1, v1);
        xint_copy(u2, v2);
        xint_copy(u3, v3);
        
        // (v1, v2, v3) ← (t1, t2, t3).
        xint_copy(v1, t1);
        xint_copy(v2, t2);
        xint_copy(v3, t3);
        
        // Return to step X2.
    }
    
    xint_copy(ud, u1);
    xint_copy(vd, u2);
    xint_copy(gcd, u3);
    
    xint_delete(q);
    xint_delete(r);

    xint_delete(t1);
    xint_delete(t2);
    xint_delete(t3);

    xint_delete(u1);
    xint_delete(u2);
    xint_delete(u3);

    xint_delete(v1);
    xint_delete(v2);
    xint_delete(v3);
}

int xint_mod_inverse(xint_t w, const xint_t u, const xint_t v)
{
    xint_t gcd = XINT_INIT_VAL;
    xint_t ud = XINT_INIT_VAL;
    xint_t vd = XINT_INIT_VAL;
    int ret = 0;

    xint_gcd_ext(gcd, ud, vd, u, v);
    if (xint_cmp_ulong(gcd, 1) == 0)
    {
        xint_copy(w, ud);
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

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

void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p)
{
    xint_point_copy(r, p);
    xint_chs(r->y);
}

void xint_point_add(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t a, xint_t m)
{
    xint_t sumy = XINT_INIT_VAL;
    xint_t sumx = XINT_INIT_VAL;
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

    xint_add(sumy, p->y, q->y);
    xint_add(sumx, p->x, q->x);
    xint_mod_inverse(sumx, sumx, m);
    xint_mul(lambda, sumy, sumx);
        
    xint_sqr(xr, lambda);
    xint_add(xr, xr, lambda);
    xint_add(xr, xr, p->x);
    xint_add(xr, xr, q->x);
    xint_add(xr, xr, a);

    xint_add(yr, p->x, xr);
    xint_mul(yr, yr, lambda);
    xint_add(yr, yr, xr);
    xint_add(yr, yr, p->y);

    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
}

void xint_point_add_p(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m)
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

    xint_sub(diffy, q->y, p->y);
    xint_mod(diffy, diffy, m);
    xint_sub(diffx, q->x, p->x);
    xint_mod(diffx, diffx, m);
    xint_mod_inverse(diffx, diffx, m);
    xint_mul(lambda, diffy, diffx);
    xint_mod(lambda, lambda, m);
        
    xint_sqr(xr, lambda);
    xint_sub(xr, xr, p->x);
    xint_mod(xr, xr, m);
    xint_sub(xr, xr, q->x);
    xint_mod(xr, xr, m);

    xint_sub(yr, p->x, xr);
    xint_mod(yr, yr, m);
    xint_mul(yr, yr, lambda);
    xint_mod(yr, yr, m);
    xint_sub(yr, yr, p->y);
    xint_mod(yr, yr, m);

    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
    r->is_at_infinity = 0;
}

void xint_point_double(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t b, xint_t m)
{
    xint_t x1inv = XINT_INIT_VAL;
    xint_t tmp2 = XINT_INIT_VAL;
    xint_t x1sq = XINT_INIT_VAL;
    xint_t xr = XINT_INIT_VAL;
    xint_t yr = XINT_INIT_VAL;
    
    if (p->is_at_infinity)
    {
        xint_point_copy(r, p);
        return;
    }

    xint_sqr(x1sq, p->x);
    xint_mod_inverse(tmp2, x1sq, m);
    xint_mul(tmp2, tmp2, b);
    xint_add(xr, x1sq, tmp2);
    
    xint_mod_inverse(x1inv, p->x, m);
    xint_mul(tmp2, p->y, x1inv);
    xint_add(tmp2, tmp2, p->x);
    xint_mul(tmp2, tmp2, xr);
    xint_add(tmp2, tmp2, x1sq);
    xint_add(yr, tmp2, xr);

    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
}

void xint_point_double_p(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m)
{
    xint_t tmp1 = XINT_INIT_VAL;
    xint_t tmp2 = XINT_INIT_VAL;
    xint_t tmp3 = XINT_INIT_VAL;
    xint_t lambda = XINT_INIT_VAL;
    xint_t xr = XINT_INIT_VAL;
    xint_t yr = XINT_INIT_VAL;

    xint_sqr(tmp1, p->x);
    xint_mod(tmp1, tmp1, m);
    xint_mul_ulong(tmp1, tmp1, 3);
    xint_mod(tmp1, tmp1, m);
    xint_add(tmp1, tmp1, a);
    xint_mod(tmp1, tmp1, m);

    xint_mul_ulong(tmp2, p->y, 2);
    xint_mod(tmp2, tmp2, m);
    xint_mod_inverse(tmp2, tmp2, m);
    xint_mod(tmp2, tmp2, m);
    xint_mul(tmp3, tmp1, tmp2);
    xint_mod(tmp3, tmp3, m);
    xint_mod(lambda, tmp3, m);
    
    xint_sqr(xr, lambda);
    xint_mod(xr, xr, m);
    xint_sub(xr, xr, p->x);
    xint_mod(xr, xr, m);
    xint_sub(xr, xr, p->x);
    xint_mod(xr, xr, m);

    xint_sub(tmp3, p->x, xr);
    xint_mod(tmp3, tmp3, m);
    xint_mul(tmp3, tmp3, lambda);
    xint_mod(yr, tmp3, m);
    xint_sub(yr, yr, p->y);
    
    xint_mod(r->x, xr, m);
    xint_mod(r->y, yr, m);
    r->is_at_infinity = 0;
}

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_t a, xint_t p)
{
    xint_ecc_point_t TMP;
    xint_point_init(TMP);
    xint_point_copy(TMP, P);

    for (int i=0; i<256; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            xint_point_add_p(R, R, TMP, p);
        }
        xint_point_double_p(TMP, TMP, a, p);
    }
}
