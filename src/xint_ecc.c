
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"

xint_ecc_curve_t p224 =
{
    224,
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF 00000000 00000000 00000001",
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFF FFFFFFFE",
    "B4050A85 0C04B3AB F5413256 5044B0B7 D7BFD8BA 270B3943 2355FFB4",
    "B70E0CBD 6BB4BF7F 321390B9 4A03C1D3 56C21122 343280D6 115C1D21",
    "BD376388 B5F723FB 4C22DFE6 CD4375A0 5A074764 44D58199 85007E34",
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFF16A2 E0B8F03E 13DD2945 5C5C2A3D",
    "01",
    xint_point_add_pcurve,
    xint_point_double_pcurve
};

xint_ecc_curve_t p256 =
{
    256,
    "FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF",
    "FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFC",
    "5AC635D8 AA3A93E7 B3EBBD55 769886BC 651D06B0 CC53B0F6 3BCE3C3E 27D2604B",
    "6B17D1F2 E12C4247 F8BCE6E5 63A440F2 77037D81 2DEB33A0 F4A13945 D898C296",
    "4FE342E2 FE1A7F9B 8EE7EB4A 7C0F9E16 2BCE3357 6B315ECE CBB64068 37BF51F5",
    "FFFFFFFF 00000000 FFFFFFFF FFFFFFFF BCE6FAAD A7179E84 F3B9CAC2 FC632551",
    "01",
    xint_point_add_pcurve,
    xint_point_double_pcurve
};

xint_ecc_curve_t p384 =
{
    384,
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF 00000000 00000000 FFFFFFFF",
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF 00000000 00000000 FFFFFFFC",
    "B3312FA7 E23EE7E4 988E056B E3F82D19 181D9C6E FE814112 0314088F 5013875A C656398D 8A2ED19D 2A85C8ED D3EC2AEF",
    "AA87CA22 BE8B0537 8EB1C71E F320AD74 6E1D3B62 8BA79B98 59F741E0 82542A38 5502F25D BF55296C 3A545E38 72760AB7",
    "3617DE4A 96262C6F 5D9E98BF 9292DC29 F8F41DBD 289A147C E9DA3113 B5F0B8C0 0A60B1CE 1D7E819D 7A431D7C 90EA0E5F",
    "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF C7634D81 F4372DDF 581A0DB2 48B0A77A ECEC196A CCC52973",
    "01",
    xint_point_add_pcurve,
    xint_point_double_pcurve
};

xint_ecc_curve_t p521 =
{
    521,
    "01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF",
    "01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFC",
    "0051 953EB961 8E1C9A1F 929A21A0 B68540EE A2DA725B 99B315F3 B8B48991 8EF109E1 56193951 EC7E937B 1652C0BD 3BB1BF07 3573DF88 3D2C34F1 EF451FD4 6B503F00",
    "00C6 858E06B7 0404E9CD 9E3ECB66 2395B442 9C648139 053FB521 F828AF60 6B4D3DBA A14B5E77 EFE75928 FE1DC127 A2FFA8DE 3348B3C1 856A429B F97E7E31 C2E5BD66",
    "0118 39296A78 9A3BC004 5C8A5FB4 2C7D1BD9 98F54449 579B4468 17AFBD17 273E662C 97EE7299 5EF42640 C550B901 3FAD0761 353C7086 A272C240 88BE9476 9FD16650",
    "01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFA 51868783 BF2F966B 7FCC0148 F709A5D0 3BB5C9B8 899C47AE BB6FB71E 91386409",
    "01",
    xint_point_add_pcurve,
    xint_point_double_pcurve
};

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
        xint_swap(u1, v1);
        xint_swap(u2, v2);
        xint_swap(u3, v3);
        
        // (v1, v2, v3) ← (t1, t2, t3).
        xint_swap(v1, t1);
        xint_swap(v2, t2);
        xint_swap(v3, t3);
        
        // Return to step X2.
    }
    
    xint_swap(ud, u1);
    xint_swap(vd, u2);
    xint_swap(gcd, u3);
    
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
    xint_delete(gcd);
    xint_delete(ud);
    xint_delete(vd);
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

void xint_mod_add(xint_t w, xint_t u, xint_t v, xint_t m)
{
    xint_add(w, u, v);
    if (xint_cmp(w, m) >= 0)
    {
        xint_sub(w, w, m);
    }
}

void xint_mod_sub(xint_t w, xint_t u, xint_t v, xint_t m)
{
    xint_sub(w, u, v);
    if (xint_is_neg(w))
    {
        xint_add(w, w, m);
    }
}

void xint_mod_mul(xint_t w, xint_t u, xint_t v, xint_t m)
{
    xint_mul(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_mul_ulong(xint_t w, xint_t u, unsigned long v, xint_t m)
{
    xint_mul_ulong(w, u, v);
    xint_mod(w, w, m);
}

void xint_mod_sqr(xint_t w, xint_t u, xint_t m)
{
    xint_sqr(w, u);
    xint_mod(w, w, m);
}

void xint_point_add_pcurve(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m)
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
}

void xint_point_double_pcurve(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m)
{
    xint_t tmp = XINT_INIT_VAL;
    xint_t lambda = XINT_INIT_VAL;
    xint_t xr = XINT_INIT_VAL;
    xint_t yr = XINT_INIT_VAL;

    xint_mod_sqr(tmp, p->x, m);
    xint_mod_mul_ulong(tmp, tmp, 3, m);
    xint_mod_add(tmp, tmp, a, m);

    xint_lshift(lambda, p->y, 1);
    xint_mod(lambda, lambda, m);
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

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_ecc_curve_t c)
{
    xint_ecc_point_t TMP;
    xint_point_init(TMP);
    xint_point_copy(TMP, P);
    xint_t a = XINT_INIT_VAL;
    xint_t p = XINT_INIT_VAL;
    xint_assign_str(a, c.a, 16);
    xint_assign_str(p, c.p, 16);
    for (int i=0; i<c.nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            c.point_add(R, R, TMP, p);
        }
        c.point_double(TMP, TMP, a, p);
    }
}
