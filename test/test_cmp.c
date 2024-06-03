
#include "test_cmp.h"

#include "xint.h"

#include "test_harness.h"

TEST_GROUP(cmp);

// Tests for
// xint_cmp_ulong
// xint_cmp_long
// xint_cmpa_ulong
// xint_cmpa_1
// xint_cmpa
// xint_cmp

static xword_t x3[3] = { 1, 2, 3 };
static const xint_t neg3 = { 3, -3, x3 };
static const xint_t pos3 = { 3, 3, x3 };
static xword_t x1[1] = { 2 };
static const xint_t neg1 = { 1, -1, x1 };
static const xint_t pos1 = { 1, 1, x1 };

TEST(cmp, ulong)
{
    ASSERT_EQ(-1, xint_cmp_ulong(neg3, 3));
    ASSERT_EQ(1, xint_cmp_ulong(pos3, 3));

    
    
    
    END_TEST(cmp);
}

int test_cmp(void)
{
    CALL_TEST(cmp, ulong)
    END_TEST_GROUP(cmp);
}
