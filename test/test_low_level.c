
#include "test_low_level.h"
#include "test_harness.h"

#include "xint.h"

TEST_GROUP(low_level);

TEST(low_level, xll_div_normalise)
{
    
    
    END_TEST(low_level);
}

TEST(low_level, xll_div_addback)
{
    // Do a div that exercises the add back test
    // From Knuth "when b = 2^16 we can let u = (7fff800100000000)base16, v = (800080020005)base16"
    // For 64 bit..
    const int m = 1;
    const int n = 3;
    xword_t u[m+n+1] = { 0x0000000000000000UL, 0x0000000000000000UL, 0x8000000000000001UL, 0x7fffffffffffffffUL, 0x0000000000000000ULL };
    xword_t v[n] = { 0x0000000000000005UL, 0x8000000000000002UL, 0x8000000000000000UL };
    xword_t q[2];
    xll_div(q, u, v, m, n);
    ASSERT_EQ(0xfffffffffffffffdUL, q[0]);
    ASSERT_EQ(0x0000000000000000UL, q[1]);
    ASSERT_EQ(0x000000000000000fUL, u[0]);
    ASSERT_EQ(0x8000000000000001UL, u[1]);
    ASSERT_EQ(0x8000000000000000UL, u[2]);
    // xll_div leaves junk in the high part of u so zero it. Needed for below recalc.
    u[3] = 0;
    // Multiply out to check it worked
    xword_t calc[5] = { 0, 0, 0, 0, 0 };
    xll_mul(calc, v, 3, q, 1);
    calc[4] = xll_add(calc, calc, u, 4);
    ASSERT_EQ(0x0000000000000000UL, calc[0]);
    ASSERT_EQ(0x0000000000000000UL, calc[1]);
    ASSERT_EQ(0x8000000000000001UL, calc[2]);
    ASSERT_EQ(0x7fffffffffffffffUL, calc[3]);

    END_TEST(low_level);
}

int test_low_level(void)
{
    CALL_TEST(low_level, xll_div_normalise);
    CALL_TEST(low_level, xll_div_addback);
    END_TEST_GROUP(low_level);
}
