
#include "test_collatz.h"
#include "test_harness.h"

#include "xint.h"
#include "xint_io.h"

TEST_GROUP(collatz);

TEST(collatz, long_chain)
{
    xint_t C = XINT_INIT_VAL;
    // Load (2^100000) - 1, the number with the longest known Collatz chain
    xint_assign_ulong(C, 1);
    xint_lshift(C, C, 100000);
    xint_sub_ulong(C, C, 1);
    //xint_print("C", C);
    int up = 0;
    int down = 0;
    int total = 0;
    while (1)
    {
        if (C->size && (C->data[0] & 1) == 0)
        {
            // It's even
            xint_rshift(C, C, 1);
            ++up;
        }
        else
        {
            // It's odd
            xint_mul_ulong(C, C, 3);
            xint_add_ulong(C, C, 1);
            ++down;
        }
        ++total;
        if ((total % 10000) == 0)
        {
            //printf("%d %d %d\n", up, down, total);
        }
        if (xint_size(C) <= 10)
        {
            //xint_print("C", C);
        }
        if (!xint_cmp_ulong(C, 1))
        {
            //printf("%d %d %d\n", up, down, total);
            break;
        }
    }
    
    ASSERT_EQ(1344926, total);
    
    END_TEST(collatz);
}

int test_collatz(void)
{
    CALL_TEST(collatz, long_chain);

    END_TEST_GROUP(collatz);
}

