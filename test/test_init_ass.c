
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






int test_init_ass(void)
{
    CALL_TEST(init_ass, init);
    END_TEST_GROUP(init_ass);
}
