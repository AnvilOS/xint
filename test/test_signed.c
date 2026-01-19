
#include "test_signed.h"

#include "test_harness.h"

#include "xint.h"
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
    
    xint_add(z, a, b); // 1100
    ASSERT_TRUE(xint_cmp_long(z, 1100) == 0);
    xint_add(z, a, c); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_add(z, c, a); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_sub(z, a, b); // -100
    ASSERT_TRUE(xint_cmp_long(z, -100) == 0);
    xint_sub(z, a, c); // 1100
    ASSERT_TRUE(xint_cmp_long(z, 1100) == 0);
    xint_sub(z, c, a); // -1100
    ASSERT_TRUE(xint_cmp_long(z, -1100) == 0);
    
    xint_assign_ulong(z, 0xfffffffffffffffeUL);
    ASSERT_EQ(1, xint_cmp_ulong(z, 0xfffffffffffffffdUL));
    ASSERT_EQ(0, xint_cmp_ulong(z, 0xfffffffffffffffeUL));
    ASSERT_EQ(-1, xint_cmp_ulong(z, 0xffffffffffffffffUL));

    END_TEST(signed_arith);
}

//           Trunc    Floor     Ceil    Euclid
// (+8, +3) (+2, +2) (+2, +2) (+3, -1) (+2, +2)
// (+8, -3) (-2, +2) (-3, -1) (-2, +2) (-2, +2)
// (-8, +3) (-2, -2) (-3, +1) (-2, -2) (-3, +1)
// (-8, -3) (+2, -2) (+2, -2) (+3, +1) (+3, +1)

TEST(signed_arith, div)
{
    xint_t u = XINT_INIT_VAL;
    xint_t v = XINT_INIT_VAL;
    xint_t q = XINT_INIT_VAL;
    xint_t r = XINT_INIT_VAL;
    
    xint_assign_str(u, "80000000000000000000000000000000000000000", 10);
    xint_assign_str(v, "30000000000000000000000000000000000000000", 10);
    xint_div_trunc(q, r, u, v);
    xint_print("u", u);
    xint_print("v", v);
    xint_print("q", q);
    xint_print("r", r);
    xint_chs(v);
    xint_div_trunc(q, r, u, v);
    xint_print("u", u);
    xint_print("v", v);
    xint_print("q", q);
    xint_print("r", r);
    xint_chs(u);
    xint_chs(v);
    xint_div_trunc(q, r, u, v);
    xint_print("u", u);
    xint_print("v", v);
    xint_print("q", q);
    xint_print("r", r);
    xint_chs(v);
    xint_div_trunc(q, r, u, v);
    xint_print("u", u);
    xint_print("v", v);
    xint_print("q", q);
    xint_print("r", r);
    END_TEST(signed_arith);
}

int test_signed(void)
{
    CALL_TEST(signed_arith, simple);
    CALL_TEST(signed_arith, div);

    END_TEST_GROUP(signed_arith);
    
}
