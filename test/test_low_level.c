
#include "test_low_level.h"
#include "test_harness.h"

#include "xint_internal.h"

TEST_GROUP(low_level)

// Constants that resize for xword_t size
#define Ox0000__0000    ((xword_t)0)
#define Ox0000__0005    ((xword_t)5)
#define Ox0000__0015    ((xword_t)15)
#define Ox0020__0002    (((xword_t)1<<(XWORD_BITS-11))+2)
#define Ox7FFF__FFFF    (((xword_t)1<<(XWORD_BITS-1))-1)
#define Ox8000__0000    ((xword_t)1<<(XWORD_BITS-1))
#define Ox8000__0001    (((xword_t)1<<(XWORD_BITS-1))+1)
#define Ox8000__0002    (((xword_t)1<<(XWORD_BITS-1))+2)
#define OxFFFF__FFFB    ((xword_t)-5)
#define OxFFFF__FFFC    ((xword_t)-4)
#define OxFFFF__FFFD    ((xword_t)-3)
#define OxFFFF__FFFE    ((xword_t)-2)
#define OxFFFF__FFFF    ((xword_t)-1)

// REMEMBER: xword_t arrays are LEAST significant first

TEST(low_level, xll_add_1_sub_1)
{
    xword_t A[] = { OxFFFF__FFFE, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF, OxFFFF__FFFF };
    //xword_t B[] = { OxFFFF__FFFF, 0, 0 };
    xword_t k, b;
    
    // Check that in-place add works
    k = xll_add_1(A, A, 2, 12);
    ASSERT_EQ(0, A[11]);
    ASSERT_EQ(0, A[10]);
    ASSERT_EQ(0, A[9]);
    ASSERT_EQ(0, A[8]);
    ASSERT_EQ(0, A[7]);
    ASSERT_EQ(0, A[6]);
    ASSERT_EQ(0, A[5]);
    ASSERT_EQ(0, A[4]);
    ASSERT_EQ(0, A[3]);
    ASSERT_EQ(0, A[2]);
    ASSERT_EQ(0, A[1]);
    ASSERT_EQ(0, A[0]);
    ASSERT_EQ(1, k);

//    // Check that sub works
//    b = xll_sub(A, A, B, 3);
//    ASSERT_EQ(OxFFFF__FFFB, A[2]);
//    ASSERT_EQ(OxFFFF__FFFC, A[1]);
//    ASSERT_EQ(OxFFFF__FFFF, A[0]);
//    ASSERT_EQ(0, b);
//
//    // Check that in-place sub works
//    b = xll_sub(A, A, A, 3);
//    ASSERT_EQ(0, A[2]);
//    ASSERT_EQ(0, A[1]);
//    ASSERT_EQ(0, A[0]);
//    ASSERT_EQ(0, b);

    END_TEST(low_level);
}

TEST(low_level, xll_add_sub)
{
    xword_t A[] = { OxFFFF__FFFF, OxFFFF__FFFE, OxFFFF__FFFD };
    xword_t B[] = { OxFFFF__FFFF, 0, 0 };
    xword_t k, b;
    
    // Check that in-place add works
    k = xll_add(A, A, A, 3);
    ASSERT_EQ(OxFFFF__FFFB, A[2]);
    ASSERT_EQ(OxFFFF__FFFD, A[1]);
    ASSERT_EQ(OxFFFF__FFFE, A[0]);
    ASSERT_EQ(1, k);

    // Check that sub works
    b = xll_sub(A, A, B, 3);
    ASSERT_EQ(OxFFFF__FFFB, A[2]);
    ASSERT_EQ(OxFFFF__FFFC, A[1]);
    ASSERT_EQ(OxFFFF__FFFF, A[0]);
    ASSERT_EQ(0, b);

    // Check that in-place sub works
    b = xll_sub(A, A, A, 3);
    ASSERT_EQ(0, A[2]);
    ASSERT_EQ(0, A[1]);
    ASSERT_EQ(0, A[0]);
    ASSERT_EQ(0, b);

    END_TEST(low_level);
}

#define m 2
#define n 4

TEST(low_level, xll_div_normalise)
{
    xword_t u[m+n+1] = { Ox8000__0002, Ox8000__0002, Ox8000__0002, Ox8000__0002, Ox8000__0002, Ox8000__0002, Ox0000__0000 };
    xword_t v[n]     = { Ox7FFF__FFFF, Ox8000__0001, OxFFFF__FFFD, Ox0020__0002 };
    xword_t q[m+1];
    xll_div(q, u, v, m, n);

    // xll_div leaves junk in the high part of u so zero it. Needed for below recalc.
    u[n] = u[n+1] = u[n+2] = 0;
    
    // Multiply out to check it worked
    xword_t calc[7] = { 0, 0, 0, 0, 0, 0, 0 };
    xll_mul(calc, v, 4, q, 3);
    calc[6] = xll_add(calc, calc, u, 6);
    ASSERT_EQ(Ox8000__0002, calc[0]);
    ASSERT_EQ(Ox8000__0002, calc[1]);
    ASSERT_EQ(Ox8000__0002, calc[2]);
    ASSERT_EQ(Ox8000__0002, calc[3]);
    ASSERT_EQ(Ox8000__0002, calc[4]);
    ASSERT_EQ(Ox8000__0002, calc[5]);
    ASSERT_EQ(Ox0000__0000, calc[6]);

    END_TEST(low_level);
}

#undef m
#undef n
#define m 1
#define n 3

TEST(low_level, xll_div_addback)
{
    // Do a div that exercises the add back test
    // From Knuth "when b = 2^16 we can let u = (7fff800100000000)base16, v = (800080020005)base16"
    // For 64 bit..
    xword_t u[m+n+1] = { Ox0000__0000, Ox0000__0000, Ox8000__0001, Ox7FFF__FFFF, Ox0000__0000 };
    xword_t v[n]     = { Ox0000__0005, Ox8000__0002, Ox8000__0000 };
    xword_t q[m+1];
    xll_div(q, u, v, m, n);
    ASSERT_EQ(OxFFFF__FFFD, q[0]);
    ASSERT_EQ(Ox0000__0000, q[1]);
    ASSERT_EQ(Ox0000__0015, u[0]);
    ASSERT_EQ(Ox8000__0001, u[1]);
    ASSERT_EQ(Ox8000__0000, u[2]);
    
    // xll_div leaves junk in the high part of u so zero it. Needed for below recalc.
    u[3] = u[4] = 0;
    
    // Multiply out to check it worked
    xword_t calc[5] = { 0, 0, 0, 0, 0 };
    xll_mul(calc, v, 3, q, 1);
    calc[4] = xll_add(calc, calc, u, 4);
    ASSERT_EQ(Ox0000__0000, calc[0]);
    ASSERT_EQ(Ox0000__0000, calc[1]);
    ASSERT_EQ(Ox8000__0001, calc[2]);
    ASSERT_EQ(Ox7FFF__FFFF, calc[3]);
    ASSERT_EQ(Ox0000__0000, calc[4]);

    END_TEST(low_level);
}

int test_low_level(void)
{
    CALL_TEST(low_level, xll_add_1_sub_1);
    CALL_TEST(low_level, xll_add_sub);
    CALL_TEST(low_level, xll_div_normalise);
    CALL_TEST(low_level, xll_div_addback);
    END_TEST_GROUP(low_level);
}
