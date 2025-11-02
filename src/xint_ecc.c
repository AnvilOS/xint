
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

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
    xint_point_add,
    xint_point_double
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
    xint_point_add,
    xint_point_double
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
    xint_point_add,
    xint_point_double
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
    xint_point_add,
    xint_point_double
};

int xint_mod_inverse(xint_t w, const xint_t u, const xint_t v)
{
    xint_t gcd = XINT_INIT_VAL;
    xint_t ud = XINT_INIT_VAL;
    xint_t vd = XINT_INIT_VAL;
    int ret = 0;

    xint_gcd_ext(gcd, ud, vd, u, v);
    if (xint_cmp_ulong(gcd, 1) == 0)
    {
        if (xint_is_neg(ud))
        {
            xint_add(ud, ud, v);
        }
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

void xint_point_jacobian_init(xint_ecc_point_jacobian_t p)
{
    p->is_at_infinity = 1;
    xint_init(p->x);
    xint_init(p->y);
    xint_init(p->z);
}

void xint_point_jacobian_delete(xint_ecc_point_jacobian_t p)
{
    p->is_at_infinity = 1;
    xint_delete(p->x);
    xint_delete(p->y);
    xint_delete(p->z);
}

void xint_point_jacobian_copy(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p)
{
    r->is_at_infinity = p->is_at_infinity;
    xint_copy(r->x, p->x);
    xint_copy(r->y, p->y);
    xint_copy(r->z, p->z);
}

void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p)
{
    xint_point_copy(r, p);
    xint_chs(r->y);
}

void xint_mod_add(xint_t w, const xint_t u, const xint_t v, const xint_t m)
{
    xint_add(w, u, v);
    if (xint_cmp(w, m) >= 0)
    {
        xint_sub(w, w, m);
    }
}

void xint_mod_sub(xint_t w, const xint_t u, const xint_t v, const xint_t m)
{
    xint_sub(w, u, v);
    if (xint_is_neg(w))
    {
        xint_add(w, w, m);
    }
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

void xint_mod_sqr(xint_t w, const xint_t u, const xint_t m)
{
    xint_sqr(w, u);
    xint_mod(w, w, m);
}

void to_jacobian(xint_ecc_point_jacobian_t w, const xint_ecc_point_t u)
{
    xint_copy(w->x, u->x);
    xint_copy(w->y, u->y);
    xint_assign_ulong(w->z, 1);
    w->is_at_infinity = u->is_at_infinity;
}

void from_jacobian(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, xint_t p)
{
    // Convert back to affine
    xint_t X = XINT_INIT_VAL;
    xint_t Y = XINT_INIT_VAL;
    xint_t z_inv = XINT_INIT_VAL;
    xint_mod_inverse(z_inv, u->z, p);
    
    xint_copy(X, z_inv);
    xint_mod_sqr(X, X, p);
    xint_mod_mul(X, X, u->x, p);
    
    xint_copy(Y, z_inv);
    xint_mod_sqr(Y, Y, p);
    xint_mod_mul(Y, Y, z_inv, p);
    xint_mod_mul(Y, Y, u->y, p);
    
    xint_copy(w->x, X);
    xint_copy(w->y, Y);
    w->is_at_infinity = 0;
}

void xint_point_add(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_t m)
{
    // 13M + 4S
    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);

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
    
    // Use algorithm from Wikibooks
    xint_t U1 = XINT_INIT_VAL;
    xint_t U2 = XINT_INIT_VAL;
    xint_t S1 = XINT_INIT_VAL;
    xint_t S2 = XINT_INIT_VAL;
    xint_t H = XINT_INIT_VAL;
    xint_t R = XINT_INIT_VAL;
    
#define x1 Pj->x
#define y1 Pj->y
#define z1 Pj->z
#define x2 Qj->x
#define y2 Qj->y
#define z2 Qj->z
#define x3 Rj->x
#define y3 Rj->y
#define z3 Rj->z
    
    // U1 = x1.z2^2 - use H as a scratch for z2^2
    xint_mod_sqr(H, z2, m);
    xint_mod_mul(U1, H, x1, m);
    
    // U2 = x2.z1^2 - use R as a scratch for z1^2
    xint_mod_sqr(R, z1, m);
    xint_mod_mul(U2, R, x2, m);
    
    // S1 = y1.z2^3
    xint_mod_mul(S1, H, z2, m);
    xint_mod_mul(S1, S1, y1, m);
    
    // S2 = y2.z1^3
    xint_mod_mul(S2, R, z1, m);
    xint_mod_mul(S2, S2, y2, m);
    
    if (xint_cmp(U1, U2) == 0)
    {
        if (xint_cmp(S1, S2) != 0)
        {
            return; // inf
        }
        else
        {
            return; // double
        }
    }
    // H = U2 - U1
    xint_sub(H, U2, U1);
    // R = S2 - S1
    xint_sub(R, S2, S1);
    
    // Calc H^2, H^3 and R^2
    xint_t H2 = XINT_INIT_VAL;
    xint_t H3 = XINT_INIT_VAL;
    xint_mod_sqr(H2, H, m);
    xint_mod_mul(H3, H2, H, m);
    xint_t R2 = XINT_INIT_VAL;
    xint_mod_sqr(R2, R, m);
    
    // X3 = R^2 - H^3 - 2.U1.H^2
    xint_mod_mul(x3, U1, H2, m);
    xint_mod_mul_ulong(x3, x3, 2, m);
    xint_mod_sub(x3, R2, x3, m);
    xint_mod_sub(x3, x3, H3, m);
    
    // Y3 = R.(U1.H^2 - X3) - S1.H^3
    xint_mod_mul(y3, U1, H2, m);
    xint_mod_sub(y3, y3, x3, m);
    xint_mod_mul(y3, y3, R, m);
    // use z3 as temp S1 . H3
    xint_mod_mul(z3, S1, H3, m);
    xint_mod_sub(y3, y3, z3, m);
    
    // Z3 = H.Z1.Z2
    xint_mod_mul(z3, H, z1, m);
    xint_mod_mul(z3, z3, z2, m);
    
    xint_point_jacobian_copy(Rjx, Rj);
    Rjx->is_at_infinity = 0;
    
    xint_point_jacobian_delete(Rj);
    
    xint_delete (U1);
    xint_delete (U2);
    xint_delete (S1);
    xint_delete (S2);
    xint_delete (H);
    xint_delete (R);
}

void xint_point_double(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_t a, const xint_t m)
{
    // 4M + 6S
    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);

    if (Pj->is_at_infinity)
    {
        xint_point_jacobian_copy(Rjx, Pj);
        return;
    }

    // Use algorithm from Wikibooks
    xint_t S = XINT_INIT_VAL;
    xint_t M = XINT_INIT_VAL;
    xint_t tmp = XINT_INIT_VAL;
    
    // M = 3.x1^2 + a.z1^4
    xint_mod_sqr(M, x1, m);
    xint_mod_mul_ulong(M, M, 3, m);
    xint_mod_sqr(tmp, z1, m);
    xint_mod_sqr(tmp, tmp, m);
    xint_mod_mul(tmp, tmp, a, m);
    xint_mod_add(M, M, tmp, m);
    
    // S = 4.x1.y1^2
    xint_mod_sqr(tmp, y1, m);
    xint_mod_mul(S, tmp, x1, m);
    xint_mod_mul_ulong(S, S, 4, m);
    
    // x3 = M^2 - 2.S
    xint_mod_sqr(x3, M, m);
    xint_mod_sub(x3, x3, S, m);
    xint_mod_sub(x3, x3, S, m);
    
    // y3 = M.(S - x3) - 8.y^4
    xint_mod_sub(y3, S, x3, m);
    xint_mod_mul(y3, y3, M, m);
    xint_mod_sqr(tmp, tmp, m);
    xint_mod_mul_ulong(tmp, tmp, 8, m);
    xint_mod_sub(y3, y3, tmp, m);
    
    // z3 = 2.y1.z1
    xint_copy(z3, y1);
    xint_mod_mul(z3, z3, z1, m);
    xint_mod_mul_ulong(z3, z3, 2, m);
    
    xint_point_jacobian_copy(Rjx, Rj);
    Rjx->is_at_infinity = 0;

    xint_point_jacobian_delete(Rj);

    xint_delete(S);
    xint_delete(M);
    xint_delete(tmp);
}

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_ecc_curve_t c)
{
    xint_ecc_point_jacobian_t TMPj;
    xint_point_jacobian_init(TMPj);
    to_jacobian(TMPj, P);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);

    xint_t a = XINT_INIT_VAL;
    xint_assign_str(a, c.a, 16);

    xint_t p = XINT_INIT_VAL;
    xint_assign_str(p, c.p, 16);
    
    for (int i=0; i<c.nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            c.point_add(Rj, Rj, TMPj, p);
        }
        c.point_double(TMPj, TMPj, a, p);
    }
    from_jacobian(R, Rj, p);
}

void xint_ecc_mul_scalar_mont_ladder(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_ecc_curve_t c)
{
    xint_ecc_point_jacobian_t R0;
    xint_point_jacobian_init(R0);

    xint_ecc_point_jacobian_t R1;
    xint_point_jacobian_init(R1);
    to_jacobian(R1, P);

    xint_t a = XINT_INIT_VAL;
    xint_assign_str(a, c.a, 16);

    xint_t p = XINT_INIT_VAL;
    xint_assign_str(p, c.p, 16);
    
    for (int i=0; i<c.nbits; ++i)
    {
        if (xint_get_bit(k, i) == 0)
        {
            c.point_double(R1, R1, a, p);
            c.point_add(R1, R1, R0, p);
        }
        else
        {
            c.point_double(R0, R0, a, p);
            c.point_add(R0, R0, R1, p);
        }
    }
    from_jacobian(R, R0, p);
}
