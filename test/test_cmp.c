
#include "test_cmp.h"

#include "xint.h"

#include "test_harness.h"

TEST_GROUP(cmp);

// Tests for
// xint_cmpa_ulong
// xint_cmp_ulong
// xint_cmp_long
// xint_cmpa
// xint_cmp

#define Ox7FFFFFFFFFFFFFF1    (((unsigned long)1<<((sizeof(unsigned long)*8)-1))-15)

TEST(cmp, cmpa_ulong)
{
    xint_t A = XINT_INIT_VAL;

    // Start with A = 0
    ASSERT_EQ(0, xint_cmpa_ulong(A, 0));
    ASSERT_EQ(-1, xint_cmpa_ulong(A, 1));
    
    // Set A to a positive number
    xint_assign_ulong(A, 100);
    ASSERT_EQ(1, xint_cmpa_ulong(A, 99));
    ASSERT_EQ(0, xint_cmpa_ulong(A, 100));
    ASSERT_EQ(-1, xint_cmpa_ulong(A, 101));

    // Set A to a negative number
    xint_assign_long(A, -100);
    ASSERT_EQ(1, xint_cmpa_ulong(A, 99));
    ASSERT_EQ(0, xint_cmpa_ulong(A, 100));
    ASSERT_EQ(-1, xint_cmpa_ulong(A, 101));
    
    // Set A to a big positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1-1));
    ASSERT_EQ(0, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1));
    ASSERT_EQ(-1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1+1));

    // Set A to a big negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1-1));
    ASSERT_EQ(0, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1));
    ASSERT_EQ(-1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1+1));

    // Set A to an even bigger positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1));

    // Set A to an even bigger negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(1, xint_cmpa_ulong(A, Ox7FFFFFFFFFFFFFF1));

    END_TEST(cmp);
}

int test_cmp(void)
{
    CALL_TEST(cmp, cmpa_ulong)
    END_TEST_GROUP(cmp);
}
