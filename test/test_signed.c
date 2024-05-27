
#include "test_signed.h"

#include "test_harness.h"

#include "xint_signed.h"
#include "xint_io.h"

TEST_GROUP(signed_arith);

TEST(signed_arith, simple)
{
    xint_t a = XINT_INIT_VAL;
    xint_t b = XINT_INIT_VAL;
    xint_t c = XINT_INIT_VAL;
    xint_t z = XINT_INIT_VAL;
    
    xint_assign_ulong(a, 500);
    xint_assign_ulong(b, 600);
    xint_assign_long(c, -600);
    
    xint_adds(z, a, b); // 1100
    ASSERT_TRUE(xint_cmp_long(z, 1100) == 0);
    xint_adds(z, a, c); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_adds(z, c, a); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_subs(z, a, b); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_subs(z, a, c); // 1100
    ASSERT_TRUE(xint_cmp_long(z, 1100) == 0);
    xint_subs(z, c, a); // -1100
    ASSERT_TRUE(xint_cmp_long(z, -1100) == 0);
    
    xint_assign_ulong(z, 0xfffffffffffffffeUL);
    ASSERT_EQ(1, xint_cmp_ulong(z, 0xfffffffffffffffdUL));
    ASSERT_EQ(0, xint_cmp_ulong(z, 0xfffffffffffffffeUL));
    ASSERT_EQ(-1, xint_cmp_ulong(z, 0xffffffffffffffffUL));

    END_TEST(signed_arith);
}

int test_signed(void)
{
    CALL_TEST(signed_arith, simple);
    
    END_TEST_GROUP(signed_arith);
    
}
