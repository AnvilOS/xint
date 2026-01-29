
#include "prime_curves.h"

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

