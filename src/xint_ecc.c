
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#if defined XDWORD_MAX
#define X(__a, __b) __a, __b
#else
#define X(__a, __b) (xword_t)__b<<32|__a
#endif

const xword_t p224_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFF };
const xword_t p224_a[]  = { X(0xFFFFFFFE, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFE), X(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFF };
const xword_t p224_b[]  = { X(0x2355FFB4, 0x270B3943), X(0xD7BFD8BA, 0x5044B0B7), X(0xF5413256, 0x0C04B3AB), 0xB4050A85 };
const xword_t p224_Gx[] = { X(0x115C1D21, 0x343280D6), X(0x56C21122, 0x4A03C1D3), X(0x321390B9, 0x6BB4BF7F), 0xB70E0CBD };
const xword_t p224_Gy[] = { X(0x85007E34, 0x44D58199), X(0x5A074764, 0xCD4375A0), X(0x4C22DFE6, 0xB5F723FB), 0xBD376388 };
const xword_t p224_n[]  = { X(0x5C5C2A3D, 0x13DD2945), X(0xE0B8F03E, 0xFFFF16A2), X(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFF };
const xword_t p224_h[]  = { 0x01 };
const xint_ecc_curve_t p224 =
{
    224,
    XINT_CONST(p224_p),
    XINT_CONST(p224_a),
    XINT_CONST(p224_b),
    XINT_CONST(p224_Gx),
    XINT_CONST(p224_Gy),
    XINT_CONST(p224_n),
    XINT_CONST(p224_h),
    xint_point_add,
    xint_point_double
};

const xword_t p256_p[]  = { X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0x00000000), X(0x00000000, 0x00000000), X(0x00000001, 0xFFFFFFFF) };
const xword_t p256_a[]  = { X(0xFFFFFFFC, 0xFFFFFFFF), X(0xFFFFFFFF, 0x00000000), X(0x00000000, 0x00000000), X(0x00000001, 0xFFFFFFFF) };
const xword_t p256_b[]  = { X(0x27D2604B, 0x3BCE3C3E), X(0xCC53B0F6, 0x651D06B0), X(0x769886BC, 0xB3EBBD55), X(0xAA3A93E7, 0x5AC635D8) };
const xword_t p256_Gx[] = { X(0xD898C296, 0xF4A13945), X(0x2DEB33A0, 0x77037D81), X(0x63A440F2, 0xF8BCE6E5), X(0xE12C4247, 0x6B17D1F2) };
const xword_t p256_Gy[] = { X(0x37BF51F5, 0xCBB64068), X(0x6B315ECE, 0x2BCE3357), X(0x7C0F9E16, 0x8EE7EB4A), X(0xFE1A7F9B, 0x4FE342E2) };
const xword_t p256_n[]  = { X(0xFC632551, 0xF3B9CAC2), X(0xA7179E84, 0xBCE6FAAD), X(0xFFFFFFFF, 0xFFFFFFFF), X(0x00000000, 0xFFFFFFFF) };
const xword_t p256_h[]  = { 0x01 };
const xint_ecc_curve_t p256 =
{
    256,
    XINT_CONST(p256_p),
    XINT_CONST(p256_a),
    XINT_CONST(p256_b),
    XINT_CONST(p256_Gx),
    XINT_CONST(p256_Gy),
    XINT_CONST(p256_n),
    XINT_CONST(p256_h),
    xint_point_add,
    xint_point_double
};

const xword_t p384_p[]  = { X(0xFFFFFFFF, 0x00000000), X(0x00000000, 0xFFFFFFFF), X(0xFFFFFFFE, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF) };
const xword_t p384_a[]  = { X(0xFFFFFFFC, 0x00000000), X(0x00000000, 0xFFFFFFFF), X(0xFFFFFFFE, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF) };
const xword_t p384_b[]  = { X(0xD3EC2AEF, 0x2A85C8ED), X(0x8A2ED19D, 0xC656398D), X(0x5013875A, 0x0314088F), X(0xFE814112, 0x181D9C6E), X(0xE3F82D19, 0x988E056B), X(0xE23EE7E4, 0xB3312FA7) };
const xword_t p384_Gx[] = { X(0x72760AB7, 0x3A545E38), X(0xBF55296C, 0x5502F25D), X(0x82542A38, 0x59F741E0), X(0x8BA79B98, 0x6E1D3B62), X(0xF320AD74, 0x8EB1C71E), X(0xBE8B0537, 0xAA87CA22) };
const xword_t p384_Gy[] = { X(0x90EA0E5F, 0x7A431D7C), X(0x1D7E819D, 0x0A60B1CE), X(0xB5F0B8C0, 0xE9DA3113), X(0x289A147C, 0xF8F41DBD), X(0x9292DC29, 0x5D9E98BF), X(0x96262C6F, 0x3617DE4A) };
const xword_t p384_n[]  = { X(0xCCC52973, 0xECEC196A), X(0x48B0A77A, 0x581A0DB2), X(0xF4372DDF, 0xC7634D81), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF) };
const xword_t p384_h[]  = { 0x01 };
const xint_ecc_curve_t p384 =
{
    384,
    XINT_CONST(p384_p),
    XINT_CONST(p384_a),
    XINT_CONST(p384_b),
    XINT_CONST(p384_Gx),
    XINT_CONST(p384_Gy),
    XINT_CONST(p384_n),
    XINT_CONST(p384_h),
    xint_point_add,
    xint_point_double
};


const xword_t p521_p[]  = { X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), 0x01FF };
const xword_t p521_a[]  = { X(0xFFFFFFFC, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), 0x01FF };
const xword_t p521_b[]  = { X(0x6B503F00, 0xEF451FD4), X(0x3D2C34F1, 0x3573DF88), X(0x3BB1BF07, 0x1652C0BD), X(0xEC7E937B, 0x56193951), X(0x8EF109E1, 0xB8B48991), X(0x99B315F3, 0xA2DA725B), X(0xB68540EE, 0x929A21A0), X(0x8E1C9A1F, 0x953EB961), 0x0051 };
const xword_t p521_Gx[] = { X(0xC2E5BD66, 0xF97E7E31), X(0x856A429B, 0x3348B3C1), X(0xA2FFA8DE, 0xFE1DC127), X(0xEFE75928, 0xA14B5E77), X(0x6B4D3DBA, 0xF828AF60), X(0x053FB521, 0x9C648139), X(0x2395B442, 0x9E3ECB66), X(0x0404E9CD, 0x858E06B7), 0x00C6 };
const xword_t p521_Gy[] = { X(0x9FD16650, 0x88BE9476), X(0xA272C240, 0x353C7086), X(0x3FAD0761, 0xC550B901), X(0x5EF42640, 0x97EE7299), X(0x273E662C, 0x17AFBD17), X(0x579B4468, 0x98F54449), X(0x2C7D1BD9, 0x5C8A5FB4), X(0x9A3BC004, 0x39296A78), 0x0118 };
const xword_t p521_n[]  = { X(0x91386409, 0xBB6FB71E), X(0x899C47AE, 0x3BB5C9B8), X(0xF709A5D0, 0x7FCC0148), X(0xBF2F966B, 0x51868783), X(0xFFFFFFFA, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), 0x01FF };
const xword_t p521_h[]  = { 0x01 };
const xint_ecc_curve_t p521 =
{
    521,
    XINT_CONST(p521_p),
    XINT_CONST(p521_a),
    XINT_CONST(p521_b),
    XINT_CONST(p521_Gx),
    XINT_CONST(p521_Gy),
    XINT_CONST(p521_n),
    XINT_CONST(p521_h),
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

void from_jacobian(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, const xint_t p)
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

    for (int i=0; i<c.nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            c.point_add(Rj, Rj, TMPj, c.p);
        }
        c.point_double(TMPj, TMPj, c.a, c.p);
    }
    from_jacobian(R, Rj, c.p);
}

void xint_ecc_mul_scalar_mont_ladder(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_ecc_curve_t c)
{
    xint_ecc_point_jacobian_t R0;
    xint_point_jacobian_init(R0);

    xint_ecc_point_jacobian_t R1;
    xint_point_jacobian_init(R1);
    to_jacobian(R1, P);

    for (int i=0; i<c.nbits; ++i)
    {
        if (xint_get_bit(k, i) == 0)
        {
            c.point_double(R1, R1, c.a, c.p);
            c.point_add(R1, R1, R0, c.p);
        }
        else
        {
            c.point_double(R0, R0, c.a, c.p);
            c.point_add(R0, R0, R1, c.p);
        }
    }
    from_jacobian(R, R0, c.p);
}
