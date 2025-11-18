
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#include <string.h>
#include <assert.h>

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
    { 0, XINT_CONST(p224_Gx), XINT_CONST(p224_Gy) },
    XINT_CONST(p224_n),
    XINT_CONST(p224_h),
    xint_point_add,
    xint_point_double,
    xint_mod_fast_224
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
    { 0, XINT_CONST(p256_Gx), XINT_CONST(p256_Gy) },
    XINT_CONST(p256_n),
    XINT_CONST(p256_h),
    xint_point_add,
    xint_point_double,
    xint_mod_fast_256
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
    { 0, XINT_CONST(p384_Gx), XINT_CONST(p384_Gy) },
    XINT_CONST(p384_n),
    XINT_CONST(p384_h),
    xint_point_add,
    xint_point_double,
    xint_mod_fast_384
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
    { 0,XINT_CONST(p521_Gx), XINT_CONST(p521_Gy) },
    XINT_CONST(p521_n),
    XINT_CONST(p521_h),
    xint_point_add,
    xint_point_double,
    xint_mod_fast_521
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

static void trim(xint_t u)
{
    int Un = abs(u->size);
    for (int j=Un-1; j>=0; --j)
    {
        if (u->data[j] != 0)
        {
            u->size = j + 1;
            return;
        }
    }
    u->size = 0;
}

void xint_mod_fast_224(xint_t w, const xint_t u, const xint_t m)
{
    // From NIST SP 800-186
    //   for 224-bit
    //   B =  ( T + S1 + S2 – D1 – D2 ) mod p,
    // T = ( A6 || A5 || A4 || A3 || A2 || A1 || A0 )
    // S1 = ( A10 || A9 || A8 || A7 || 0 || 0 || 0 )
    // S2 = ( 0 || A13 || A12 || A11 || 0 || 0 || 0 )
    // D1 = ( A13 || A12 || A11 || A10 || A9 || A8 || A7 )
    // D2 = ( 0 || 0 || 0 || 0 || A13 || A12 || A11 ).

    int is_neg = u->size < 0;
    
    unsigned A[14];
    memset(A, 0, sizeof(A));
    for (int i=0; i<abs(u->size); ++i)
    {
#if XDWORD_MAX
        A[i] = u->data[i];
#else
        A[2*i] = (unsigned)u->data[i];
        A[2*i+1] = u->data[i]>>(XWORD_BITS/2);
#endif
    }

    xint_t tmp = XINT_INIT_VAL;
    xword_t tmp_data[7];
    tmp->size = 7;
    tmp->capacity = 7;
    tmp->data = tmp_data;
    
#if XDWORD_MAX
#define TMP_BUILDER(__t6, __t5, __t4, __t3, __t2, __t1, __t0) \
    tmp->data[6] = __t6; \
    tmp->data[5] = __t5; \
    tmp->data[4] = __t4, \
    tmp->data[3] = __t3; \
    tmp->data[2] = __t2; \
    tmp->data[1] = __t1; \
    tmp->data[0] = __t0; \
    tmp->size = 7; \
    trim(tmp);
#else
#define TMP_BUILDER(__t6, __t5, __t4, __t3, __t2, __t1, __t0) \
    tmp->data[3] = __t6; \
    tmp->data[2] = ((xword_t)__t5 << (XWORD_BITS/2)) | __t4; \
    tmp->data[1] = ((xword_t)__t3 << (XWORD_BITS/2)) | __t2; \
    tmp->data[0] = ((xword_t)__t1 << (XWORD_BITS/2)) | __t0; \
    tmp->size = 4; \
    trim(tmp);
#endif

    //xint_t B = XINT_INIT_VAL;
    xint_assign_zero(w);
    
    // T
    TMP_BUILDER(A[6], A[5], A[4], A[3], A[2], A[1], A[0]);
    xint_add(w, w, tmp);

    // S1
    TMP_BUILDER(A[10], A[9], A[8], A[7], 0, 0, 0);
    xint_add(w, w, tmp);

    // S2
    TMP_BUILDER(0, A[13], A[12], A[11], 0, 0, 0);
    xint_add(w, w, tmp);

    // D1
    TMP_BUILDER(A[13], A[12], A[11], A[10], A[9], A[8], A[7]);
    xint_sub(w, w, tmp);

    // D2
    TMP_BUILDER(0, 0, 0, 0, A[13], A[12], A[11]);
    xint_sub(w, w, tmp);

    if (is_neg)
    {
        xint_chs(w);
    }
    while (xint_cmp(w, m) >= 0)
    {
        xint_sub(w, w, m);
    }
    while (xint_is_neg(w))
    {
        xint_add(w, w, m);
    }
#undef TMP_BUILDER
}

void xint_mod_fast_256(xint_t w, const xint_t u, const xint_t m)
{
    // T  = (A7 || A6 || A5 || A4 || A3 || A2 || A1 || A0 )
    // S1 = ( A15 || A14 || A13 || A12 || A11 || 0 || 0 || 0 )
    // S2 = ( 0 || A15 || A14 || A13 || A12 || 0 || 0 || 0 )
    // S3 = ( A15 || A14 || 0 || 0 || 0 || A10 || A9 || A8 )
    // S4 = ( A8 || A13 || A15 || A14 || A13 || A11 || A10 || A9 )
    // D1 = ( A10 || A8 || 0 || 0 || 0 || A13 || A12 || A11 )
    // D2 = ( A11 || A9 || 0 || 0 || A15 || A14 || A13 || A12 )
    // D3 = ( A12 || 0 || A10 || A9 || A8 || A15 || A14 || A13 )
    // D4 = ( A13 || 0 || A11 || A10 || A9 || 0 || A15 || A14 ).

    int is_neg = u->size < 0;
    
    xword_t A[16];
    memset(A, 0, sizeof(A));
    for (int i=0; i<abs(u->size); ++i)
    {
        A[i] = u->data[i];
    }

    xint_t tmp = XINT_INIT_VAL;
    xword_t tmp_data[8];
    tmp->size = 8;
    tmp->capacity = 8;
    tmp->data = tmp_data;
    
#if XDWORD_MAX
#define TMP_BUILDER(__t7, __t6, __t5, __t4, __t3, __t2, __t1, __t0) \
    tmp->data[7] = __t7; \
    tmp->data[6] = __t6; \
    tmp->data[5] = __t5; \
    tmp->data[4] = __t4, \
    tmp->data[3] = __t3; \
    tmp->data[2] = __t2; \
    tmp->data[1] = __t1; \
    tmp->data[0] = __t0; \
    tmp->size = 8; \
    trim(tmp);
#else
#define TMP_BUILDER(__t3, __t2, __t1, __t0) \
    tmp->data[3] = __t3; \
    tmp->data[2] = __t2; \
    tmp->data[1] = __t1; \
    tmp->data[0] = __t0; \
    tmp->size = 4; \
    trim(tmp);
#endif

    //xint_t B = XINT_INIT_VAL;
    xint_assign_zero(w);
    
#if XDWORD_MAX
    // T
    TMP_BUILDER(A[7], A[6], A[5], A[4], A[3], A[2], A[1], A[0]);
    xint_add(w, w, tmp);

    // S1
    TMP_BUILDER(A[15], A[14], A[13], A[12], A[11], 0, 0, 0);
    xint_add(w, w, tmp);
    xint_add(w, w, tmp);

    // S2
    TMP_BUILDER(0, A[15], A[14], A[13], A[12], 0, 0, 0);
    xint_add(w, w, tmp);
    xint_add(w, w, tmp);

    // S3
    TMP_BUILDER(A[15], A[14], 0, 0, 0, A[10], A[9], A[8]);
    xint_add(w, w, tmp);

    // S4
    TMP_BUILDER(A[8], A[13], A[15], A[14], A[13], A[11], A[10], A[9]);
    xint_add(w, w, tmp);

    // D1
    TMP_BUILDER(A[10], A[8], 0, 0, 0, A[13], A[12], A[11]);
    xint_sub(w, w, tmp);

    // D2
    TMP_BUILDER(A[11], A[9], 0, 0, A[15], A[14], A[13], A[12]);
    xint_sub(w, w, tmp);

    // D3
    TMP_BUILDER(A[12], 0, A[10], A[9], A[8], A[15], A[14], A[13]);
    xint_sub(w, w, tmp);

    // D4
    TMP_BUILDER(A[13], 0, A[11], A[10], A[9], 0, A[15], A[14]);
    xint_sub(w, w, tmp);
#else
#define SHR(__a) ((__a)>>(XWORD_BITS/2))
#define HI(__a) ((__a)&0xffffffff00000000ULL)
#define SHL(__a) ((__a)<<(XWORD_BITS/2))
#define LO(__a) ((__a)&0xffffffff)
    // T
    TMP_BUILDER(A[3], A[2], A[1], A[0]);
    xint_add(w, w, tmp);

    // S1
    TMP_BUILDER(A[7], A[6], HI(A[5]), 0);
    xint_add(w, w, tmp);
    xint_add(w, w, tmp);

    // S2
    TMP_BUILDER(SHR(A[7]), SHL(A[7])|SHR(A[6]), SHL(A[6]), 0);
    xint_add(w, w, tmp);
    xint_add(w, w, tmp);

    // S3
    TMP_BUILDER(A[7], 0, LO(A[5]), A[4]);
    xint_add(w, w, tmp);

    // S4
    TMP_BUILDER(SHL(A[4])|SHR(A[6]), A[7], HI(A[6])|SHR(A[5]), SHL(A[5])|SHR(A[4]));
    xint_add(w, w, tmp);

    // D1
    TMP_BUILDER(SHL(A[5])|LO(A[4]), 0, SHR(A[6]), SHL(A[6])|SHR(A[5]));
    xint_sub(w, w, tmp);

    // D2
    TMP_BUILDER(HI(A[5])|SHR(A[4]), 0, A[7], A[6]);
    xint_sub(w, w, tmp);

    // D3
    TMP_BUILDER(SHL(A[6]), SHL(A[5])|SHR(A[4]), SHL(A[4])|SHR(A[7]), SHL(A[7])|SHR(A[6]));
    xint_sub(w, w, tmp);

    // D4
    TMP_BUILDER(HI(A[6]), A[5], HI(A[4]), A[7]);
    xint_sub(w, w, tmp);
#endif
    
    if (is_neg)
    {
        xint_chs(w);
    }
    while (xint_cmp(w, m) >= 0)
    {
        xint_sub(w, w, m);
    }
    while (xint_is_neg(w))
    {
        xint_add(w, w, m);
    }
#undef TMP_BUILDER
}

void xint_mod_fast_384(xint_t w, const xint_t u, const xint_t m)
{
    xint_mod(w, u, m);
}

void xint_mod_fast_521(xint_t w, const xint_t u, const xint_t m)
{
    xint_mod(w, u, m);
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

void xint_mod_mul(xint_t w, const xint_t u, const xint_t v, const xint_ecc_curve_t curve)
{
    xint_mul(w, u, v);
    xint_mod(w, w, curve.p);
}

void xint_mod_mul_ulong(xint_t w, const xint_t u, unsigned long v, const xint_ecc_curve_t curve)
{
    xint_mul_ulong(w, u, v);
    xint_mod(w, w, curve.p);
}

void xint_mod_sqr(xint_t w, const xint_t u, const xint_ecc_curve_t curve)
{
    xint_sqr(w, u);
    xint_mod(w, w, curve.p);
}

void to_jacobian(xint_ecc_point_jacobian_t w, const xint_ecc_point_t u)
{
    xint_copy(w->x, u->x);
    xint_copy(w->y, u->y);
    xint_assign_ulong(w->z, 1);
    w->is_at_infinity = u->is_at_infinity;
}

void from_jacobian(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, const xint_ecc_curve_t curve)
{
    // Convert back to affine
    xint_t X = XINT_INIT_VAL;
    xint_t Y = XINT_INIT_VAL;
    xint_t z_inv = XINT_INIT_VAL;
    xint_mod_inverse(z_inv, u->z, curve.p);
    
    xint_copy(X, z_inv);
    xint_mod_sqr(X, X, curve);
    xint_mod_mul(X, X, u->x, curve);
    
    xint_copy(Y, z_inv);
    xint_mod_sqr(Y, Y, curve);
    xint_mod_mul(Y, Y, z_inv, curve);
    xint_mod_mul(Y, Y, u->y, curve);
    
    xint_copy(w->x, X);
    xint_copy(w->y, Y);
    w->is_at_infinity = 0;
}

#define x1 Pj->x
#define y1 Pj->y
#define z1 Pj->z
#define x2 Qj->x
#define y2 Qj->y
#define z2 Qj->z
#define x3 Rj->x
#define y3 Rj->y
#define z3 Rj->z
    
void xint_point_add(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t curve)
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
    
    // U1 = x1.z2^2 - use H as a scratch for z2^2
    xint_mod_sqr(H, z2, curve);
    xint_mod_mul(U1, H, x1, curve);
    
    // U2 = x2.z1^2 - use R as a scratch for z1^2
    xint_mod_sqr(R, z1, curve);
    xint_mod_mul(U2, R, x2, curve);
    
    // S1 = y1.z2^3
    xint_mod_mul(S1, H, z2, curve);
    xint_mod_mul(S1, S1, y1, curve);
    
    // S2 = y2.z1^3
    xint_mod_mul(S2, R, z1, curve);
    xint_mod_mul(S2, S2, y2, curve);
    
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
    xint_mod_sub(H, U2, U1, curve.p);
    // R = S2 - S1
    xint_mod_sub(R, S2, S1, curve.p);
    
    // Calc H^2, H^3 and R^2
    xint_t H2 = XINT_INIT_VAL;
    xint_t H3 = XINT_INIT_VAL;
    xint_mod_sqr(H2, H, curve);
    xint_mod_mul(H3, H2, H, curve);
    xint_t R2 = XINT_INIT_VAL;
    xint_mod_sqr(R2, R, curve);
    
    // X3 = R^2 - H^3 - 2.U1.H^2
    xint_mod_mul(x3, U1, H2, curve);
    xint_mod_mul_ulong(x3, x3, 2, curve);
    xint_mod_sub(x3, R2, x3, curve.p);
    xint_mod_sub(x3, x3, H3, curve.p);
    
    // Y3 = R.(U1.H^2 - X3) - S1.H^3
    xint_mod_mul(y3, U1, H2, curve);
    xint_mod_sub(y3, y3, x3, curve.p);
    xint_mod_mul(y3, y3, R, curve);
    // use z3 as temp S1 . H3
    xint_mod_mul(z3, S1, H3, curve);
    xint_mod_sub(y3, y3, z3, curve.p);
    
    // Z3 = H.Z1.Z2
    xint_mod_mul(z3, H, z1, curve);
    xint_mod_mul(z3, z3, z2, curve);
    
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

void xint_point_double(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t curve)
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
    xint_mod_sqr(M, x1, curve);
    xint_mod_mul_ulong(M, M, 3, curve);
    xint_mod_sqr(tmp, z1, curve);
    xint_mod_sqr(tmp, tmp, curve);
    xint_mod_mul(tmp, tmp, curve.a, curve);
    xint_mod_add(M, M, tmp, curve.p);
    
    // S = 4.x1.y1^2
    xint_mod_sqr(tmp, y1, curve);
    xint_mod_mul(S, tmp, x1, curve);
    xint_mod_mul_ulong(S, S, 4, curve);
    
    // x3 = M^2 - 2.S
    xint_mod_sqr(x3, M, curve);
    xint_mod_sub(x3, x3, S, curve.p);
    xint_mod_sub(x3, x3, S, curve.p);
    
    // y3 = M.(S - x3) - 8.y^4
    xint_mod_sub(y3, S, x3, curve.p);
    xint_mod_mul(y3, y3, M, curve);
    xint_mod_sqr(tmp, tmp, curve);
    xint_mod_mul_ulong(tmp, tmp, 8, curve);
    xint_mod_sub(y3, y3, tmp, curve.p);
    
    // z3 = 2.y1.z1
    xint_copy(z3, y1);
    xint_mod_mul(z3, z3, z1, curve);
    xint_mod_mul_ulong(z3, z3, 2, curve);
    
    xint_point_jacobian_copy(Rjx, Rj);
    Rjx->is_at_infinity = 0;

    xint_point_jacobian_delete(Rj);

    xint_delete(S);
    xint_delete(M);
    xint_delete(tmp);
}

void ecc_zaddu(xint_ecc_point_jacobian_t Pj, xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t curve)
{
    assert(xint_cmp(Pj->z, Qj->z) == 0);
    
    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);
    
    xint_t C = XINT_INIT_VAL;
    xint_t W1 = XINT_INIT_VAL;
    xint_t W2 = XINT_INIT_VAL;
    xint_t D = XINT_INIT_VAL;
    xint_t A1 = XINT_INIT_VAL;
    xint_t tmp = XINT_INIT_VAL;

    // C = (X1 − X2)^2
    xint_mod_sub(C, x1, x2, curve.p);
    xint_mod_sqr(C, C, curve);
    
    // W1 = X1 * C
    xint_mod_mul(W1, x1, C, curve);
    // W2 = X2 * C
    xint_mod_mul(W2, x2, C, curve);
    
    // D = (Y1 − Y2)^2
    xint_mod_sub(D, y1, y2, curve.p);
    xint_mod_sqr(D, D, curve);
    
    // A1 = Y1 * (W1 − W2)
    xint_mod_sub(A1, W1, W2, curve.p);
    xint_mod_mul(A1, A1, y1, curve);
    
    // X3 = D − W1 − W2
    xint_mod_sub(x3, D, W1, curve.p);
    xint_mod_sub(x3, x3, W2, curve.p);
    
    // Y3 = (Y1 − Y2) * (W1 − X3) − A1
    xint_mod_sub(y3, y1, y2, curve.p);
    xint_mod_sub(tmp, W1, x3, curve.p);
    xint_mod_mul(y3, y3, tmp, curve);
    xint_mod_sub(y3, y3, A1, curve.p);

    // Z3 = Z * (X1 − X2)
    xint_mod_sub(z3, x1, x2, curve.p);
    xint_mod_mul(z3, z3, z1, curve);
    
    xint_point_jacobian_copy(Qj, Rj);
    Qj->is_at_infinity = 0;
    
    // X1 = W1
    xint_copy(Pj->x, W1);
    // Y1 = A1
    xint_copy(Pj->y, A1);
    // Z1 = Z3
    xint_copy(Pj->z, Rj->z);
    
    Pj->is_at_infinity = 0;
}

void ecc_zaddc(xint_ecc_point_jacobian_t Pj, xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t curve)
{
    assert(xint_cmp(Pj->z, Qj->z) == 0);
    
    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);
    
    xint_t C = XINT_INIT_VAL;
    xint_t W1 = XINT_INIT_VAL;
    xint_t W2 = XINT_INIT_VAL;
    xint_t D = XINT_INIT_VAL;
    xint_t A1 = XINT_INIT_VAL;
    xint_t Dd = XINT_INIT_VAL;
    xint_t x3d = XINT_INIT_VAL;
    xint_t y3d = XINT_INIT_VAL;
    xint_t z3d = XINT_INIT_VAL;
    xint_t tmp = XINT_INIT_VAL;

    // C = (X1 − X2)^2
    xint_mod_sub(C, x1, x2, curve.p);
    xint_mod_sqr(C, C, curve);
    
    // W1 = X1 * C
    xint_mod_mul(W1, x1, C, curve);
    // W2 = X2 * C
    xint_mod_mul(W2, x2, C, curve);
    
    // D = (Y1 − Y2)^2
    xint_mod_sub(D, y1, y2, curve.p);
    xint_mod_sqr(D, D, curve);
    
    // A1 = Y1 * (W1 − W2)
    xint_mod_sub(A1, W1, W2, curve.p);
    xint_mod_mul(A1, A1, y1, curve);
    
    // X3 = D − W1 − W2
    xint_mod_sub(x3, D, W1, curve.p);
    xint_mod_sub(x3, x3, W2, curve.p);
    
    // Y3 = (Y1 − Y2) * (W1 − X3) − A1
    xint_mod_sub(y3, y1, y2, curve.p);
    xint_mod_sub(tmp, W1, x3, curve.p);
    xint_mod_mul(y3, y3, tmp, curve);
    xint_mod_sub(y3, y3, A1, curve.p);

    // Z3 = Z * (X1 − X2)
    xint_mod_sub(z3, x1, x2, curve.p);
    xint_mod_mul(z3, z3, z1, curve);
    
    // Dd = (Y1 + Y2)^2
    xint_mod_add(Dd, y1, y2, curve.p);
    xint_mod_sqr(Dd, Dd, curve);

    // X3d = Dd − W1 − W2
    xint_mod_sub(x3d, Dd, W1, curve.p);
    xint_mod_sub(x3d, x3d, W2, curve.p);

    // Y3d = (Y1 + Y2) * (W1 − X3d) − A1
    xint_mod_sub(tmp, W1, x3d, curve.p);
    xint_mod_add(y3d, y1, y2, curve.p);
    xint_mod_mul(y3d, y3d, tmp, curve);
    xint_mod_sub(y3d, y3d, A1, curve.p);

    xint_point_jacobian_copy(Qj, Rj);
    Qj->is_at_infinity = 0;
    
    // X1 = W1
    xint_copy(Pj->x, x3d);
    // Y1 = A1
    xint_copy(Pj->y, y3d);
    // Z1 = Z3
    xint_copy(Pj->z, Rj->z);

    Pj->is_at_infinity = 0;

}

void ecc_zdau(xint_ecc_point_jacobian_t Pj, xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t c)
{
    xint_t T1 = XINT_INIT_VAL;
    xint_t T2 = XINT_INIT_VAL;
    xint_t T3 = XINT_INIT_VAL;
    xint_t T4 = XINT_INIT_VAL;
    xint_t T5 = XINT_INIT_VAL;
    xint_t T6 = XINT_INIT_VAL;
    xint_t T7 = XINT_INIT_VAL;
    xint_t T8 = XINT_INIT_VAL;
    
    xint_copy(T1, Pj->x);
    xint_copy(T2, Pj->y);
    xint_copy(T3, Pj->z);
    xint_copy(T4, Qj->x);
    xint_copy(T5, Qj->y);
    
    // 1
    xint_mod_sub(T6, T1, T4, c.p);
    xint_mod_sqr(T7, T6, c);
    xint_mod_mul(T1, T1, T7, c);
    xint_mod_mul(T4, T4, T7, c);
    xint_mod_sub(T5, T2, T5, c.p);
    
    // 6
    xint_mod_sub(T8, T1, T4, c.p);
    xint_mod_mul(T2, T2, T8, c);
    xint_mod_mul_ulong(T2, T2, 2, c);
    xint_mod_sqr(T8, T5, c);
    xint_mod_sub(T4, T8, T4, c.p);
    
    // 11
    xint_mod_sub(T4, T4, T1, c.p);
    xint_mod_sub(T4, T4, T1, c.p);
    xint_mod_add(T6, T4, T6, c.p);
    xint_mod_sqr(T6, T6, c);
    xint_mod_sub(T6, T6, T7, c.p);

    // 16
    xint_mod_sub(T5, T5, T4, c.p);
    xint_mod_sqr(T5, T5, c);
    xint_mod_sub(T5, T5, T8, c.p);
    xint_mod_sub(T5, T5, T2, c.p);
    xint_mod_sqr(T7, T4, c);
    
    // 21
    xint_mod_sub(T5, T5, T7, c.p);
    xint_mod_mul_ulong(T8, T7, 4, c);
    xint_mod_sub(T6, T6, T7, c.p);
    xint_mod_mul(T3, T3, T6, c);
    xint_mod_mul(T6, T1, T8, c);
    
    // 26
    xint_mod_add(T1, T1, T4, c.p);
    xint_mod_mul(T8, T8, T1, c);
    xint_mod_add(T7, T2, T5, c.p);
    xint_mod_sub(T2, T5, T2, c.p);
    xint_mod_sub(T1, T8, T6, c.p);

    // 31
    xint_mod_mul(T5, T5, T1, c);
    xint_mod_add(T6, T6, T8, c.p);
    xint_mod_sqr(T1, T2, c);
    xint_mod_sub(T1, T1, T6, c.p);
    xint_mod_sub(T4, T8, T1, c.p);

    // 36
    xint_mod_mul(T2, T2, T4, c);
    xint_mod_sub(T2, T2, T5, c.p);
    xint_mod_sqr(T4, T7, c);
    xint_mod_sub(T4, T4, T6, c.p);
    xint_mod_sub(T8, T8, T4, c.p);

    // 41
    xint_mod_mul(T7, T7, T8, c);
    xint_mod_sub(T5, T7, T5, c.p);

    xint_copy(Pj->x, T1);
    xint_copy(Pj->y, T2);
    xint_copy(Pj->z, T3);
    xint_copy(Qj->x, T4);
    xint_copy(Qj->y, T5);
    xint_copy(Qj->z, T3);
}

void ecc_dblu(xint_ecc_point_jacobian_t Rjx, xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t curve)
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
    xint_mod_sqr(M, x1, curve);
    xint_mod_mul_ulong(M, M, 3, curve);
    xint_mod_sqr(tmp, z1, curve);
    xint_mod_sqr(tmp, tmp, curve);
    xint_mod_mul(tmp, tmp, curve.a, curve);
    xint_mod_add(M, M, tmp, curve.p);
    
    // S = 4.x1.y1^2
    xint_mod_sqr(tmp, y1, curve);
    xint_mod_mul(S, tmp, x1, curve);
    xint_mod_mul_ulong(S, S, 4, curve);
    
    // x3 = M^2 - 2.S
    xint_mod_sqr(x3, M, curve);
    xint_mod_sub(x3, x3, S, curve.p);
    xint_mod_sub(x3, x3, S, curve.p);
    
    // y3 = M.(S - x3) - 8.y^4
    xint_mod_sub(y3, S, x3, curve.p);
    xint_mod_mul(y3, y3, M, curve);
    xint_mod_sqr(tmp, tmp, curve);
    xint_mod_mul_ulong(tmp, tmp, 8, curve);
    xint_mod_sub(y3, y3, tmp, curve.p);
    
    // z3 = 2.y1.z1
    xint_copy(z3, y1);
    xint_mod_mul(z3, z3, z1, curve);
    xint_mod_mul_ulong(z3, z3, 2, curve);
    
    xint_point_jacobian_copy(Rjx, Rj);
    Rjx->is_at_infinity = 0;
    
    // Now update Pj
    xint_copy(x1, S);
    xint_copy(y1, tmp);
    xint_copy(z1, z3);

    xint_point_jacobian_delete(Rj);

    xint_delete(S);
    xint_delete(M);
    xint_delete(tmp);
}

void ecc_tplu(xint_ecc_point_jacobian_t Rj, xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t curve)
{
    // 3P = 2P + P
    //
    // (2P, P') =  DBLU(P),
    // 3P = ZADDU(P', 2P)
    xint_ecc_point_jacobian_t Tj;
    xint_point_jacobian_init(Tj);
    ecc_dblu(Tj, Pj, curve);
    ecc_zaddu(Pj, Tj, curve);
    xint_point_jacobian_copy(Rj, Tj);
    Tj->is_at_infinity = 0;
}

#undef x1
#undef y1
#undef z1
#undef x2
#undef y2
#undef z2
#undef x3
#undef y3
#undef z3

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k_in, const xint_ecc_curve_t c)
{
    // Force b0 of k to be a 1
    xint_t k = XINT_INIT_VAL;
    xint_copy(k, k_in);
    if (xint_get_bit(k, 0) == 0)
    {
        xint_add(k, k, c.n);
    }
    
    int nbits = xint_highest_bit_num(k) + 1;
    xint_ecc_point_jacobian_t Rj[2];
    xint_point_jacobian_init(Rj[0]);
    xint_point_jacobian_init(Rj[1]);
 
    int bit = xint_get_bit(k, 1);
    to_jacobian(Rj[bit], P);
    
    ecc_tplu(Rj[1-bit], Rj[bit], c);

    for (int i=2; i<nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
#if 1
        ecc_zdau(Rj[1-bit], Rj[bit], c);
#else
        ecc_zaddu(Rj[1-bit], Rj[bit], c);
        ecc_zaddc(Rj[bit], Rj[1-bit], c);
#endif
    }
    from_jacobian(R, Rj[0], c);
}

void xint_ecc_mul_scalar_old(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t c)
{
    xint_ecc_point_jacobian_t Rj[2];
    xint_point_jacobian_init(Rj[0]);
    xint_point_jacobian_init(Rj[1]);
    to_jacobian(Rj[1], P);
    for (int i=0; i<c.nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
        c.point_double(Rj[1-bit], Rj[1-bit], c);
        c.point_add(Rj[1-bit], Rj[1-bit], Rj[bit], c);
    }
    from_jacobian(R, Rj[0], c);
}

int xint_ecc_sign_det(char *sig, char *key, char *digest, int digest_len, xint_ecc_curve_t c)
{
    return 0;
}

int xint_ecc_verify(         )
{
    return 0;
}

