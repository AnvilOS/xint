
#include "xint_internal.h"

#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>
#include <assert.h>

#if defined XDWORD_MAX
#define X(__a, __b) __a, __b
#else
#define X(__a, __b) (xword_t)__b<<32|__a
#endif

#define CURVE_WORDS(__nbits) (((__nbits-1)/XWORD_BITS)+1)

const xword_t k163_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFF };
const xword_t k163_a[]  = { X(0xFFFFFFFE, 0xFFFFFFFF), X(0xFFFFFFFF, 0xFFFFFFFE), X(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFF };
const xword_t k163_b[]  = { X(0x2355FFB4, 0x270B3943), X(0xD7BFD8BA, 0x5044B0B7), X(0xF5413256, 0x0C04B3AB), 0xB4050A85 };
const xword_t k163_Gx[] = { X(0x115C1D21, 0x343280D6), X(0x56C21122, 0x4A03C1D3), X(0x321390B9, 0x6BB4BF7F), 0xB70E0CBD };
const xword_t k163_Gy[] = { X(0x85007E34, 0x44D58199), X(0x5A074764, 0xCD4375A0), X(0x4C22DFE6, 0xB5F723FB), 0xBD376388 };
const xword_t k163_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k163_h[]  = { 0x01 };
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
    CURVE_WORDS(224),
    p224_p,
    p224_a,
    p224_b,
    p224_Gx,
    p224_Gy,
    p224_n,
    p224_h,
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
    CURVE_WORDS(256),
    p256_p,
    p256_a,
    p256_b,
    p256_Gx,
    p256_Gy,
    p256_n,
    p256_h,
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
    CURVE_WORDS(384),
    p384_p,
    p384_a,
    p384_b,
    p384_Gx,
    p384_Gy,
    p384_n,
    p384_h,
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
    CURVE_WORDS(521),
    p521_p,
    p521_a,
    p521_b,
    p521_Gx,
    p521_Gy,
    p521_n,
    p521_h,
    xint_mod_fast_521
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
    xint_mod_std(w, u, &k163);
}

void xint_mod_fast_224(xword_t *w, xword_t *u)
{
    xint_mod_std(w, u, &p224);
//    // From NIST SP 800-186
//    //   for 224-bit
//    //   B =  ( T + S1 + S2 – D1 – D2 ) mod p,
//    // T = ( A6 || A5 || A4 || A3 || A2 || A1 || A0 )
//    // S1 = ( A10 || A9 || A8 || A7 || 0 || 0 || 0 )
//    // S2 = ( 0 || A13 || A12 || A11 || 0 || 0 || 0 )
//    // D1 = ( A13 || A12 || A11 || A10 || A9 || A8 || A7 )
//    // D2 = ( 0 || 0 || 0 || 0 || A13 || A12 || A11 ).
//
//    int is_neg = u->size < 0;
//    
//    unsigned A[14];
//    memset(A, 0, sizeof(A));
//    for (int i=0; i<abs(u->size); ++i)
//    {
//#if XDWORD_MAX
//        A[i] = u->data[i];
//#else
//        A[2*i] = (unsigned)u->data[i];
//        A[2*i+1] = u->data[i]>>(XWORD_BITS/2);
//#endif
//    }
//
//    xint_t tmp = XINT_INIT_VAL;
//    xword_t tmp_data[7];
//    tmp->size = 7;
//    tmp->capacity = 7;
//    tmp->data = tmp_data;
//    
//#if XDWORD_MAX
//#define TMP_BUILDER(__t6, __t5, __t4, __t3, __t2, __t1, __t0) \
//    tmp->data[6] = __t6; \
//    tmp->data[5] = __t5; \
//    tmp->data[4] = __t4, \
//    tmp->data[3] = __t3; \
//    tmp->data[2] = __t2; \
//    tmp->data[1] = __t1; \
//    tmp->data[0] = __t0; \
//    tmp->size = 7; \
//    trim(tmp);
//#else
//#define TMP_BUILDER(__t6, __t5, __t4, __t3, __t2, __t1, __t0) \
//    tmp->data[3] = __t6; \
//    tmp->data[2] = ((xword_t)__t5 << (XWORD_BITS/2)) | __t4; \
//    tmp->data[1] = ((xword_t)__t3 << (XWORD_BITS/2)) | __t2; \
//    tmp->data[0] = ((xword_t)__t1 << (XWORD_BITS/2)) | __t0; \
//    tmp->size = 4; \
//    trim(tmp);
//#endif
//
//    //xint_t B = XINT_INIT_VAL;
//    xint_assign_zero(w);
//    
//    // T
//    TMP_BUILDER(A[6], A[5], A[4], A[3], A[2], A[1], A[0]);
//    xint_add(w, w, tmp);
//
//    // S1
//    TMP_BUILDER(A[10], A[9], A[8], A[7], 0, 0, 0);
//    xint_add(w, w, tmp);
//
//    // S2
//    TMP_BUILDER(0, A[13], A[12], A[11], 0, 0, 0);
//    xint_add(w, w, tmp);
//
//    // D1
//    TMP_BUILDER(A[13], A[12], A[11], A[10], A[9], A[8], A[7]);
//    xint_sub(w, w, tmp);
//
//    // D2
//    TMP_BUILDER(0, 0, 0, 0, A[13], A[12], A[11]);
//    xint_sub(w, w, tmp);
//
//    if (is_neg)
//    {
//        xint_chs(w);
//    }
//    while (xint_cmp(w, m) >= 0)
//    {
//        xint_sub(w, w, m);
//    }
//    while (xint_is_neg(w))
//    {
//        xint_add(w, w, m);
//    }
//#undef TMP_BUILDER
}

void xint_mod_fast_256(xword_t *w, xword_t *A)
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

    xword_t tmp_data[8];
    xll_zero(w, p256.nwords);

#if XWORD_BITS == 32
#define TMP_BUILDER(__t7, __t6, __t5, __t4, __t3, __t2, __t1, __t0) \
    tmp_data[7] = __t7; \
    tmp_data[6] = __t6; \
    tmp_data[5] = __t5; \
    tmp_data[4] = __t4, \
    tmp_data[3] = __t3; \
    tmp_data[2] = __t2; \
    tmp_data[1] = __t1; \
    tmp_data[0] = __t0;
    long k;
    // T
    xll_move(w, A, p256.nwords);

    // S1
    TMP_BUILDER(A[15], A[14], A[13], A[12], A[11], 0, 0, 0);
    k = xll_add(w, w, tmp_data, p256.nwords);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S2
    TMP_BUILDER(0, A[15], A[14], A[13], A[12], 0, 0, 0);
    k += xll_add(w, w, tmp_data, p256.nwords);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S3
    TMP_BUILDER(A[15], A[14], 0, 0, 0, A[10], A[9], A[8]);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S4
    TMP_BUILDER(A[8], A[13], A[15], A[14], A[13], A[11], A[10], A[9]);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // D1
    TMP_BUILDER(A[10], A[8], 0, 0, 0, A[13], A[12], A[11]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D2
    TMP_BUILDER(A[11], A[9], 0, 0, A[15], A[14], A[13], A[12]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D3
    TMP_BUILDER(A[12], 0, A[10], A[9], A[8], A[15], A[14], A[13]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D4
    TMP_BUILDER(A[13], 0, A[11], A[10], A[9], 0, A[15], A[14]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);
#elif XWORD_BITS == 64
#define SHR(__a) ((__a)>>(XWORD_BITS/2))
#define HI(__a) ((__a)&0xffffffff00000000ULL)
#define SHL(__a) ((__a)<<(XWORD_BITS/2))
#define LO(__a) ((__a)&0xffffffff)
#define TMP_BUILDER(__t3, __t2, __t1, __t0) \
    tmp_data[3] = __t3; \
    tmp_data[2] = __t2; \
    tmp_data[1] = __t1; \
    tmp_data[0] = __t0;
    long k;
    // T
    TMP_BUILDER(A[3], A[2], A[1], A[0]);
    k = xll_add(w, w, tmp_data, p256.nwords);

    // S1
    TMP_BUILDER(A[7], A[6], HI(A[5]), 0);
    k += xll_add(w, w, tmp_data, p256.nwords);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S2
    TMP_BUILDER(SHR(A[7]), SHL(A[7])|SHR(A[6]), SHL(A[6]), 0);
    k += xll_add(w, w, tmp_data, p256.nwords);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S3
    TMP_BUILDER(A[7], 0, LO(A[5]), A[4]);
    k += xll_add(w, w, tmp_data, p256.nwords);

    // S4
    TMP_BUILDER(SHL(A[4])|SHR(A[6]), A[7], HI(A[6])|SHR(A[5]), SHL(A[5])|SHR(A[4]));
    k += xll_add(w, w, tmp_data, p256.nwords);

    // D1
    TMP_BUILDER(SHL(A[5])|LO(A[4]), 0, SHR(A[6]), SHL(A[6])|SHR(A[5]));
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D2
    TMP_BUILDER(HI(A[5])|SHR(A[4]), 0, A[7], A[6]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D3
    TMP_BUILDER(SHL(A[6]), SHL(A[5])|SHR(A[4]), SHL(A[4])|SHR(A[7]), SHL(A[7])|SHR(A[6]));
    k -= xll_sub(w, w, tmp_data, p256.nwords);

    // D4
    TMP_BUILDER(HI(A[6]), A[5], HI(A[4]), A[7]);
    k -= xll_sub(w, w, tmp_data, p256.nwords);
#else
#error Only 32 and 64 bits supported
#endif

    while (k < 0)
    {
        // Get rid of the carry
        k += xll_add(w, w, p256.p, p256.nwords);
    }
    while (k > 0 || xll_cmp(w, p256.p, p256.nwords) >= 0)
    {
        k -= xll_sub(w, w, p256.p, p256.nwords);
    }
#undef TMP_BUILDER
}

void xint_mod_fast_384(xword_t *w, xword_t *u)
{
    xint_mod_std(w, u, &p384);
}

void xint_mod_fast_521(xword_t *w, xword_t *u)
{
    xint_mod_std(w, u, &p521);
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

static void inline xint_ecc_mod_sqr(xword_t *w, const xword_t *u, const xint_ecc_curve_t *c)
{
    xword_t tmp[40];
    xll_squ(tmp, u, c->nwords);
    c->xint_mod_fast(w, tmp);
}

void from_jacobian(xint_t wx, xint_t wy, const xword_t *ux, const xword_t *uy, const xword_t *uz, const xint_ecc_curve_t *c)
{
    // Convert back to affine
    xint_t X = XINT_INIT_VAL;
    xint_t Y = XINT_INIT_VAL;
    xint_t P = XINT_INIT_VAL;
    xint_t UZ = XINT_INIT_VAL;
    P->size = c->nwords;
    P->data = c->p;
    UZ->size = c->nwords;
    UZ->data = uz;
    xint_t z_inv = XINT_INIT_VAL;
    xint_mod_inverse(z_inv, UZ, P);

    xint_copy(X, z_inv);
    xint_ecc_mod_sqr(X->data, X->data, c);
    xint_ecc_mod_mul(X->data, X->data, ux, c);
    
    xint_copy(Y, z_inv);
    xint_ecc_mod_sqr(Y->data, Y->data, c);
    xint_ecc_mod_mul(Y->data, Y->data, z_inv->data, c);
    xint_ecc_mod_mul(Y->data, Y->data, uy, c);
    
    xint_copy(wx, X);
    xint_copy(wy, Y);
    //w->is_at_infinity = 0;
}

void ecc_zaddu(xword_t *T1, xword_t *T2, xword_t *T3, xword_t *T4, xword_t *T5, const xint_ecc_curve_t *c)
{
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
}

void ecc_zdau(xword_t *T1, xword_t *T2, xword_t *T3, xword_t *T4, xword_t *T5, const xint_ecc_curve_t *c)
{
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
}

void ecc_dblu(xword_t *x3, xword_t *y3, xword_t *z3, xword_t *x1, xword_t *y1, xword_t *z1, const xint_ecc_curve_t *c)
{
    // 4M + 6S

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
    xword_t Rx[2][c->nwords];
    xword_t Ry[2][c->nwords];
    xword_t Rz[2][c->nwords];

    int bit = xint_get_bit(k, 1);
    
    // To Jacobian
    xll_move(Rx[bit], Px, c->nwords);
    xll_move(Ry[bit], Py, c->nwords);
    xll_zero(Rz[bit], c->nwords);
    Rz[bit][0] = 1;
    
    // 3P = 2P + P
    //
    // (2P, P') =  DBLU(P),
    // 3P = ZADDU(P', 2P)
    ecc_dblu(Rx[1-bit], Ry[1-bit], Rz[1-bit], Rx[bit], Ry[bit], Rz[bit], c);
    ecc_zaddu(Rx[bit], Ry[bit], Rz[0], Rx[1-bit], Ry[1-bit], c);

    for (int i=2; i<nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
        ecc_zdau(Rx[1-bit], Ry[1-bit], Rz[0], Rx[bit], Ry[bit], c);
    }

    from_jacobian(R->x, R->y, Rx[0], Ry[0], Rz[0], c);
    xint_delete(k);
}

void ecc_gen_deterministic_k(xint_t k, uint8_t *h1, int hlen, xint_t v_int, const xint_ecc_curve_t *c)
{
    xint_t q_int = XINT_INIT_VAL;
    int qlen = c->nbits;
    q_int->size = c->nwords;
    q_int->data = c->n;

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
    N->size = p256.nwords;
    N->data = p256.n;

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
    N->size = p256.nwords;
    N->data = p256.n;
    xint_t P = XINT_INIT_VAL;
    P->size = c->nwords;
    P->data = c->p;

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
    G->x->size = c->nwords;
    G->x->data = c->Gx;
    G->y->size = c->nwords;
    G->y->data = c->Gy;
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
    a->size = c->nwords;
    a->data = c->a;
    p->size = c->nwords;
    p->data = c->p;
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

void to_jacobian(xint_ecc_point_jacobian_t w, const xint_ecc_point_t u)
{
    xint_copy(w->x, u->x);
    xint_copy(w->y, u->y);
    xint_assign_ulong(w->z, 1);
    w->is_at_infinity = u->is_at_infinity;
}

void from_jacobian_ex(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, const xint_t p)
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

    xint_delete(X);
    xint_delete(Y);
    xint_delete(z_inv);
}

void xint_point_add_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c)
{
    // 13M + 4S
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
    
//    xint_ecc_point_jacobian_t Rj;
//    xint_point_jacobian_init(Rj);

    // Use algorithm from Wikibooks
    xword_t U1[10];
    xword_t U2[10];
    xword_t S1[10];
    xword_t S2[10];
    xword_t H[10];
    xword_t R[10];
    
//#define x1 Pj->x
//#define y1 Pj->y
//#define z1 Pj->z
//#define x2 Qj->x
//#define y2 Qj->y
//#define z2 Qj->z
//#define x3 Rj->x
//#define y3 Rj->y
//#define z3 Rj->z
    
    xword_t x1[10];
    xll_zero(x1, 10);
    xll_move(x1, Pj->x->data, Pj->x->size);
    xword_t y1[10];
    xll_zero(y1, 10);
    xll_move(y1, Pj->y->data, Pj->y->size);
    xword_t z1[10];
    xll_zero(z1, 10);
    xll_move(z1, Pj->z->data, Pj->z->size);

    xword_t x2[10];
    xll_zero(x2, 10);
    xll_move(x2, Qj->x->data, Qj->x->size);
    xword_t y2[10];
    xll_zero(y2, 10);
    xll_move(y2, Qj->y->data, Qj->y->size);
    xword_t z2[10];
    xll_zero(z2, 10);
    xll_move(z2, Qj->z->data, Qj->z->size);

    xword_t x3[10];
    xword_t y3[10];
    xword_t z3[10];
    
    // U1 = x1.z2^2 - use H as a scratch for z2^2
    xint_ecc_mod_sqr(H, z2, c);
    xint_ecc_mod_mul(U1, H, x1, c);
    
    // U2 = x2.z1^2 - use R as a scratch for z1^2
    xint_ecc_mod_sqr(R, z1, c);
    xint_ecc_mod_mul(U2, R, x2, c);
    
    // S1 = y1.z2^3
    xint_ecc_mod_mul(S1, H, z2, c);
    xint_ecc_mod_mul(S1, S1, y1, c);
    
    // S2 = y2.z1^3
    xint_ecc_mod_mul(S2, R, z1, c);
    xint_ecc_mod_mul(S2, S2, y2, c);
    
    if (xll_cmp(U1, U2, c->nwords) == 0)
    {
        if (xll_cmp(S1, S2, c->nwords) != 0)
        {
            return; // inf
        }
        else
        {
            return; // double
        }
    }
    // H = U2 - U1
    xint_ecc_mod_sub(H, U2, U1, c);
    // R = S2 - S1
    xint_ecc_mod_sub(R, S2, S1, c);
    
    // Calc H^2, H^3 and R^2
    xword_t H2[10];
    xword_t H3[10];
    //xint_t H2 = XINT_INIT_VAL;
    //xint_t H3 = XINT_INIT_VAL;
    xint_ecc_mod_sqr(H2, H, c);
    xint_ecc_mod_mul(H3, H2, H, c);
    xword_t R2[10];
//    xint_t R2 = XINT_INIT_VAL;
    xint_ecc_mod_sqr(R2, R, c);
    
    // X3 = R^2 - H^3 - 2.U1.H^2
    xint_ecc_mod_mul(x3, U1, H2, c);
    xint_ecc_mod_mul_ulong(x3, x3, 2, c);
    xint_ecc_mod_sub(x3, R2, x3, c);
    xint_ecc_mod_sub(x3, x3, H3, c);
    
    // Y3 = R.(U1.H^2 - X3) - S1.H^3
    xint_ecc_mod_mul(y3, U1, H2, c);
    xint_ecc_mod_sub(y3, y3, x3, c);
    xint_ecc_mod_mul(y3, y3, R, c);
    // use z3 as temp S1 . H3
    xint_ecc_mod_mul(z3, S1, H3, c);
    xint_ecc_mod_sub(y3, y3, z3, c);
    
    // Z3 = H.Z1.Z2
    xint_ecc_mod_mul(z3, H, z1, c);
    xint_ecc_mod_mul(z3, z3, z2, c);
    
    FAST_RESIZE(Rjx->x, c->nwords);
    xll_move(Rjx->x->data, x3, c->nwords);
    trim_zeroes(Rjx->x);
    FAST_RESIZE(Rjx->y, c->nwords);
    xll_move(Rjx->y->data, y3, c->nwords);
    trim_zeroes(Rjx->y);
    FAST_RESIZE(Rjx->z, c->nwords);
    xll_move(Rjx->z->data, z3, c->nwords);
    trim_zeroes(Rjx->z);
    Rjx->is_at_infinity = 0;

    
//    xint_point_jacobian_copy(Rjx, Rj);
//    Rjx->is_at_infinity = 0;
//    
//    xint_point_jacobian_delete(Rj);
    
//    xint_delete (U1);
//    xint_delete (U2);
//    xint_delete (S1);
//    xint_delete (S2);
//    xint_delete (H);
//    xint_delete (R);
//
//    xint_delete (H2);
//    xint_delete (H3);
//    xint_delete (R2);
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


void xint_point_double_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_t a, const xint_ecc_curve_t *c)
{
    // 4M + 6S
    if (Pj->is_at_infinity)
    {
        xint_point_jacobian_copy(Rjx, Pj);
        return;
    }

    // Use algorithm from Wikibooks
    xword_t S[10];
    xword_t M[10];
    xword_t tmp[10];
    xword_t x1[10];
    xll_zero(x1, 10);
    xll_move(x1, Pj->x->data, Pj->x->size);
    xword_t y1[10];
    xll_zero(y1, 10);
    xll_move(y1, Pj->y->data, Pj->y->size);
    xword_t z1[10];
    xll_zero(z1, 10);
    xll_move(z1, Pj->z->data, Pj->z->size);
    xword_t x3[10];
    xword_t y3[10];
    xword_t z3[10];

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
    xint_ecc_mod_mul_ulong(S, S, 4, c);
    
    // x3 = M^2 - 2.S
    xint_ecc_mod_sqr(x3, M, c);
    xint_ecc_mod_sub(x3, x3, S, c);
    xint_ecc_mod_sub(x3, x3, S, c);
    
    // y3 = M.(S - x3) - 8.y^4
    xint_ecc_mod_sub(y3, S, x3, c);
    xint_ecc_mod_mul(y3, y3, M, c);
    xint_ecc_mod_sqr(tmp, tmp, c);
    xint_ecc_mod_mul_ulong(tmp, tmp, 8, c);
    xint_ecc_mod_sub(y3, y3, tmp, c);
    
    // z3 = 2.y1.z1
    xll_move(z3, y1, c->nwords);
    xint_ecc_mod_mul(z3, z3, z1, c);
    xint_ecc_mod_mul_ulong(z3, z3, 2, c);
    
    FAST_RESIZE(Rjx->x, c->nwords);
    xll_move(Rjx->x->data, x3, c->nwords);
    trim_zeroes(Rjx->x);
    FAST_RESIZE(Rjx->y, c->nwords);
    xll_move(Rjx->y->data, y3, c->nwords);
    trim_zeroes(Rjx->y);
    FAST_RESIZE(Rjx->z, c->nwords);
    xll_move(Rjx->z->data, z3, c->nwords);
    trim_zeroes(Rjx->z);
    Rjx->is_at_infinity = 0;
}

void xint_ecc_mul_scalar_jacobian(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c)
{
    xint_t a = XINT_INIT_VAL;
    xint_t p = XINT_INIT_VAL;
    a->size = c->nwords;
    a->data = c->a;
    p->size = c->nwords;
    p->data = c->p;
#if 1
    // Left to right
    xint_ecc_point_jacobian_t TMPj;
    xint_point_jacobian_init(TMPj);
    to_jacobian(TMPj, P);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);
    for (int i=c->nbits-1; i>=0; --i)
    {
        xint_point_double_jacobian(Rj, Rj, a, c);
        if (xint_get_bit(k, i) == 1)
        {
            xint_point_add_jacobian(Rj, Rj, TMPj, c);
        }
    }
    from_jacobian_ex(R, Rj, p);
    xint_point_jacobian_delete(TMPj);
    xint_point_jacobian_delete(Rj);
#elif 0
    // Right to left
    xint_ecc_point_jacobian_t TMPj;
    xint_point_jacobian_init(TMPj);
    to_jacobian(TMPj, P);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);
    for (int i=0; i<c->nbits; ++i)
    {
        if (xint_get_bit(k, i) == 1)
        {
            xint_point_add_jacobian(Rj, Rj, TMPj, c);
        }
        xint_point_double_jacobian(TMPj, TMPj, a, c);
    }
    from_jacobian_ex(R, Rj, p);
    xint_point_jacobian_delete(TMPj);
    xint_point_jacobian_delete(Rj);
#else
    xint_ecc_point_jacobian_t Rj[2];
    xint_point_jacobian_init(Rj[0]);
    xint_point_jacobian_init(Rj[1]);
    to_jacobian(Rj[1], P);
    for (int i=0; i<c->nbits; ++i)
    {
        int bit = xint_get_bit(k, i);
        xint_point_double_jacobian(Rj[1-bit], Rj[1-bit], a, c);
        xint_point_add_jacobian(Rj[1-bit], Rj[1-bit], Rj[bit], c);
    }
    from_jacobian_ex(R, Rj[0], p);
    xint_point_jacobian_delete(Rj[0]);
    xint_point_jacobian_delete(Rj[1]);
#endif
}

void xint_ecc_mul_scalar_shamir(xint_ecc_point_t R, const xint_ecc_point_t S, const xint_ecc_point_t G, const xint_t u1, const xint_t u2, const xint_ecc_curve_t *c)
{
    xint_t a = XINT_INIT_VAL;
    xint_t p = XINT_INIT_VAL;
    a->size = c->nwords;
    a->data = c->a;
    p->size = c->nwords;
    p->data = c->p;
    
    xint_ecc_point_jacobian_t Gj;
    xint_point_jacobian_init(Gj);
    to_jacobian(Gj, G);
    Gj->is_at_infinity = 0;

    xint_ecc_point_jacobian_t Sj;
    xint_point_jacobian_init(Sj);
    to_jacobian(Sj, S);
    Sj->is_at_infinity = 0;

    xint_ecc_point_jacobian_t Bj;
    xint_point_jacobian_init(Bj);
    xint_point_add_jacobian(Bj, Gj, Sj, c);

    xint_ecc_point_jacobian_t Rj;
    xint_point_jacobian_init(Rj);

    // Left to right because it's easier to do the Shamir trick
    for (int i=c->nbits-1; i>=0; --i)
    {
        xint_point_double_jacobian(Rj, Rj, a, c);
        int bits = xint_get_bit(u2, i) << 1 | xint_get_bit(u1, i);
        if (bits == 3)
        {
            xint_point_add_jacobian(Rj, Rj, Bj, c);
        }
        else if (bits == 2)
        {
            xint_point_add_jacobian(Rj, Rj, Sj, c);
        }
        else if (bits == 1)
        {
            xint_point_add_jacobian(Rj, Rj, Gj, c);
        }
    }
    from_jacobian_ex(R, Rj, p);
    xint_point_jacobian_delete(Gj);
    xint_point_jacobian_delete(Sj);
    xint_point_jacobian_delete(Bj);
    xint_point_jacobian_delete(Rj);
}
