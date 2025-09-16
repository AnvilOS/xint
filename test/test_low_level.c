
#include "test_low_level.h"
#include "test_harness.h"

#include "xint.h"

TEST_GROUP(low_level);

// Constants that resize for xword_t size
#define Ox0000__0000    ((xword_t)0)
#define Ox0000__0005    ((xword_t)5)
#define Ox0000__0015    ((xword_t)15)
#define Ox0020__0002    (((xword_t)1<<(XWORD_BITS-11))+2)
#define Ox7FFF__FFFF    (((xword_t)1<<(XWORD_BITS-1))-1)
#define Ox8000__0000    ((xword_t)1<<(XWORD_BITS-1))
#define Ox8000__0001    (((xword_t)1<<(XWORD_BITS-1))+1)
#define Ox8000__0002    (((xword_t)1<<(XWORD_BITS-1))+2)
#define OxFFFF__FFFD    ((xword_t)-3)
#define OxFFFF__FFFF    ((xword_t)-1)

TEST(low_level, xll_div_normalise)
{
    const int m = 2;
    const int n = 4;
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

TEST(low_level, xll_div_addback)
{
    // Do a div that exercises the add back test
    // From Knuth "when b = 2^16 we can let u = (7fff800100000000)base16, v = (800080020005)base16"
    // For 64 bit..
    const int m = 1;
    const int n = 3;
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
    CALL_TEST(low_level, xll_div_normalise);
    CALL_TEST(low_level, xll_div_addback);
    END_TEST_GROUP(low_level);
}
