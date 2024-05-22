
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
    xint_delete(u);

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
    xint_suba_1(u, u, 5);
    ASSERT_TRUE(u->size == -1 && u->data[0] == 5);
    xint_delete(u);

    END_TEST(abs_arith);
}

TEST(abs_arith, adda_suba)
{
    xint_t u = XINT_INIT_VAL;
    xint_t v = XINT_INIT_VAL;
    xint_t w = XINT_INIT_VAL;
    
    // Identical terms
    xint_assign_ulong(u, 0xffffffffffffffffULL);
    xint_assign_ulong(v, 0xffffffffffffffffULL);
    ASSERT_TRUE(u->size == 2 && u->data[1] == 0xffffffff && u->data[0] == 0xffffffff);
    ASSERT_TRUE(v->size == 2 && v->data[1] == 0xffffffff && v->data[0] == 0xffffffff);
    xint_adda(w, u, v);
    ASSERT_TRUE(w->size == 3 && w->data[2] == 1 && w->data[1] == 0xffffffff && w->data[0] == 0xfffffffe);
    xint_suba(w, u, v);
    ASSERT_TRUE(w->size == 0);

    // Same sized terms
    xint_suba_1(u, u, 2);
    xint_suba(w, u, v);
    ASSERT_TRUE(w->size == 1 && w->data[0] == 2);
    xint_suba(w, v, u);
    ASSERT_TRUE(w->size == 1 && w->data[0] == 2);

    // Different sized terms
    xint_adda_1(u, u, 2);
    xint_suba(w, u, v);
    ASSERT_TRUE(w->size == 1 && w->data[0] == 2);
    xint_suba(w, v, u);
    ASSERT_TRUE(w->size == 1 && w->data[0] == 2);

    // One of the terms is 0
    xint_assign_ulong(u, 0);
    xint_suba(w, u, v);
    ASSERT_TRUE(w->size == 2 && w->data[1] == 0xffffffff && w->data[0] == 0xffffffff);
    xint_suba(w, v, u);
    ASSERT_TRUE(w->size == 2 && w->data[1] == 0xffffffff && w->data[0] == 0xffffffff);

    END_TEST(abs_arith);
}

int test_abs_arith(void)
{
    CALL_TEST(abs_arith, adda_suba_1);
    CALL_TEST(abs_arith, adda_suba_1_neg);
    CALL_TEST(abs_arith, adda_suba);

    END_TEST_GROUP(abs_arith);
}
