
#include "test_ecc.h"
#include "test_harness.h"

#include "xint.h"

TEST_GROUP(ecc);

TEST(ecc, simple)
{
    ASSERT_EQ(1, 1);

    END_TEST(ecc);
}

int test_ecc(void)
{
    CALL_TEST(ecc, simple);
    
    END_TEST_GROUP(ecc);
}
