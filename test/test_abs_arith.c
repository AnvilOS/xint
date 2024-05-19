
#include "test_abs_arith.h"

#include "xint.h"

#include "test_harness.h"

TEST_GROUP(abs_arith);

TEST(abs_arith, adda_suba_1)
{
    xint_t u = XINT_INIT_VAL;
    xint_adda_1(u, u, 1);
    ASSERT_TRUE(u->size == 1 && u->data[0] == 1);
    xint_adda_1(u, u, 0xfffffffe);
    ASSERT_TRUE(u->size == 1 && u->data[0] == 0xffffffff);
    xint_adda_1(u, u, 1);
    ASSERT_TRUE(u->size == 2 && u->data[1] == 1 && u->data[0] == 0);
    xint_adda_1(u, u, 1);
    ASSERT_TRUE(u->size == 2 && u->data[1] == 1 && u->data[0] == 1);
    xint_suba_1(u, u, 0xfffffffe);
    ASSERT_TRUE(u->size == 1 && u->data[0] == 3);
    xint_suba_1(u, u, 1);
    ASSERT_TRUE(u->size == 1 && u->data[0] == 2);
    xint_suba_1(u, u, 2);
    ASSERT_TRUE(u->size == 0);

    END_TEST(abs_arith);
}

TEST(abs_arith, adda_suba_1_neg)
{
    xint_t u = XINT_INIT_VAL;
    xint_adda_1(u, u, 1);
    xint_chs(u);
    ASSERT_TRUE(u->size == -1 && u->data[0] == 1);
    xint_adda_1(u, u, 0xfffffffe);
    ASSERT_TRUE(u->size == -1 && u->data[0] == 0xffffffff);
    xint_adda_1(u, u, 1);
    ASSERT_TRUE(u->size == -2 && u->data[1] == 1 && u->data[0] == 0);
    xint_adda_1(u, u, 1);
    ASSERT_TRUE(u->size == -2 && u->data[1] == 1 && u->data[0] == 1);
    xint_suba_1(u, u, 0xfffffffe);
    ASSERT_TRUE(u->size == -1 && u->data[0] == 3);
    xint_suba_1(u, u, 1);
    ASSERT_TRUE(u->size == -1 && u->data[0] == 2);
    xint_suba_1(u, u, 2);
    ASSERT_TRUE(u->size == 0);

    END_TEST(abs_arith);
}

int test_abs_arith(void)
{
    CALL_TEST(abs_arith, adda_suba_1);
    CALL_TEST(abs_arith, adda_suba_1_neg);

    END_TEST_GROUP(abs_arith);
}
