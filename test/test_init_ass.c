
#include "test_init_ass.h"
#include "test_harness.h"

#include "xint.h"

#include <string.h>

TEST_GROUP(init_ass);

TEST(init_ass, init)
{
    xint_t A = XINT_INIT_VAL;
    ASSERT_EQ(0, A->size);
    ASSERT_EQ(0, A->capacity);
    ASSERT_PTR_EQ(NULL, A->data);
    
    xint_t B;
    xint_init(B);
    ASSERT_EQ(0, B->size);
    ASSERT_EQ(0, B->capacity);
    ASSERT_PTR_EQ(NULL, B->data);

    xint_t C;
    xint_init2(C, 10);
    ASSERT_EQ(0, C->size);
    ASSERT_EQ(10, C->capacity);
    ASSERT_PTR_NE(NULL, C->data);
    memset(C->data, 0xff, sizeof(xword_t)*C->capacity);
    
    xint_delete(A);
    ASSERT_EQ(0, A->size);
    ASSERT_EQ(0, A->capacity);
    ASSERT_PTR_EQ(NULL, A->data);

    xint_delete(B);
    ASSERT_EQ(0, B->size);
    ASSERT_EQ(0, B->capacity);
    ASSERT_PTR_EQ(NULL, B->data);

    xint_delete(C);
    ASSERT_EQ(0, C->size);
    ASSERT_EQ(0, C->capacity);
    ASSERT_PTR_EQ(NULL, C->data);
    
    END_TEST(init_ass);
}

TEST(init_ass, assign)
{
    // Load the biggest unsigned log
    xint_t A = XINT_INIT_VAL;
    xint_assign_ulong(A, (unsigned long)-1);
    if (sizeof(unsigned long) == sizeof(xword_t))
    {
        ASSERT_EQ(1, A->size);
        ASSERT_EQ((xword_t)-1, A->data[0]);
    }
    else
    {
        ASSERT_EQ(2, A->size);
        ASSERT_EQ((xword_t)-1, A->data[0]);
        ASSERT_EQ((xword_t)-1, A->data[1]);
    }
    
    // Load some unsigned longs
    xint_assign_ulong(A, 0);
    ASSERT_EQ(0, A->size);

    xint_assign_ulong(A, 3);
    ASSERT_EQ(1, A->size);
    ASSERT_EQ(3, A->data[0]);

    xint_assign_long(A, 0);
    ASSERT_EQ(0, A->size);

    xint_assign_long(A, 3);
    ASSERT_EQ(1, A->size);
    ASSERT_EQ(3, A->data[0]);

    xint_assign_long(A, -3);
    ASSERT_EQ(-1, A->size);
    ASSERT_EQ(3, A->data[0]);

    unsigned long x = ((unsigned long)-1) >> 10;
    xint_assign_long(A, -x);
    if (sizeof(unsigned long) == sizeof(xword_t))
    {
        ASSERT_EQ(-1, A->size);
        ASSERT_EQ(x, A->data[0]);
    }
    else
    {
        ASSERT_EQ(-2, A->size);
        ASSERT_EQ((xword_t)-1, A->data[0]);
        ASSERT_EQ(((xword_t)-1)>>10, A->data[1]);
    }
    
    xint_delete(A);
    
    END_TEST(init_ass);
}

TEST(init_ass, assign_str)
{
    xint_t A = XINT_INIT_VAL;
    xint_assign_str(A, "123456789012345678901234567890", 10);
    if (sizeof(unsigned long) == sizeof(xword_t))
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(2, A->size);
        ASSERT_EQ(0x000000018ee90ff6UL, A->data[1]);
        ASSERT_EQ(0xc373e0ee4e3f0ad2UL, A->data[0]);
    }
    else
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(4, A->size);
        ASSERT_EQ(0x00000001, A->data[3]);
        ASSERT_EQ(0x8ee90ff6, A->data[2]);
        ASSERT_EQ(0xc373e0ee, A->data[1]);
        ASSERT_EQ(0x4e3f0ad2, A->data[0]);
    }
    xint_delete(A);
    
    xint_assign_str(A, "+123456789012345678901234567890", 10);
    if (sizeof(unsigned long) == sizeof(xword_t))
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(2, A->size);
        ASSERT_EQ(0x000000018ee90ff6UL, A->data[1]);
        ASSERT_EQ(0xc373e0ee4e3f0ad2UL, A->data[0]);
    }
    else
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(4, A->size);
        ASSERT_EQ(0x00000001, A->data[3]);
        ASSERT_EQ(0x8ee90ff6, A->data[2]);
        ASSERT_EQ(0xc373e0ee, A->data[1]);
        ASSERT_EQ(0x4e3f0ad2, A->data[0]);
    }
    xint_delete(A);
    
    xint_assign_str(A, "-123456789012345678901234567890", 10);
    if (sizeof(unsigned long) == sizeof(xword_t))
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(-2, A->size);
        ASSERT_EQ(0x000000018ee90ff6UL, A->data[1]);
        ASSERT_EQ(0xc373e0ee4e3f0ad2UL, A->data[0]);
    }
    else
    {
        // 1 8ee90ff6 c373e0ee 4e3f0ad2
        ASSERT_EQ(-4, A->size);
        ASSERT_EQ(0x00000001, A->data[3]);
        ASSERT_EQ(0x8ee90ff6, A->data[2]);
        ASSERT_EQ(0xc373e0ee, A->data[1]);
        ASSERT_EQ(0x4e3f0ad2, A->data[0]);
    }
    xint_delete(A);

    END_TEST(init_ass);
}

#define Ox8000__0002    (((xword_t)1<<(XWORD_BITS-1))+2)
#define OxFFFF__FFFD    ((xword_t)-3)

TEST(init_ass, copy_swap)
{
    xint_t A = XINT_INIT_VAL;
    xint_t B = XINT_INIT_VAL;

    xint_assign_ulong(A, Ox8000__0002);
    xint_add(A, A, A);
    ASSERT_EQ(2, A->size);
    
    xint_copy(B, A);
    ASSERT_EQ(B->size, A->size);
    ASSERT_PTR_NE(B->data, A->data);
    ASSERT_EQ(B->data[0], A->data[0]);
    ASSERT_EQ(B->data[1], A->data[1]);
    
    xint_t C = XINT_INIT_VAL;
    xint_t D = XINT_INIT_VAL;

    xint_assign_ulong(C, OxFFFF__FFFD);
    ASSERT_EQ(1, C->size);

    xint_copy(D, C);
    ASSERT_EQ(D->size, C->size);
    ASSERT_PTR_NE(D->data, C->data);
    ASSERT_EQ(D->data[0], C->data[0]);
    
    // Now swap B and D
    xint_swap(B, D);
    
    ASSERT_EQ(D->size, A->size);
    ASSERT_PTR_NE(D->data, A->data);
    ASSERT_EQ(D->data[0], A->data[0]);
    ASSERT_EQ(D->data[1], A->data[1]);

    ASSERT_EQ(B->size, C->size);
    ASSERT_PTR_NE(B->data, C->data);
    ASSERT_EQ(B->data[0], C->data[0]);

    xint_delete(A);
    xint_delete(B);
    xint_delete(C);
    xint_delete(D);

    END_TEST(init_ass);
}

int test_init_ass(void)
{
    CALL_TEST(init_ass, init);
    CALL_TEST(init_ass, assign);
    CALL_TEST(init_ass, assign_str);
    CALL_TEST(init_ass, copy_swap);

    END_TEST_GROUP(init_ass);
}
