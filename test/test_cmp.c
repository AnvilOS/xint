
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

    xint_delete(A);

    END_TEST(cmp);
}

TEST(cmp, cmp_ulong)
{
    xint_t A = XINT_INIT_VAL;

    // Start with A = 0
    ASSERT_EQ(0, xint_cmp_ulong(A, 0));
    ASSERT_EQ(-1, xint_cmp_ulong(A, 1));
    
    // Set A to a positive number
    xint_assign_ulong(A, 100);
    ASSERT_EQ(1, xint_cmp_ulong(A, 99));
    ASSERT_EQ(0, xint_cmp_ulong(A, 100));
    ASSERT_EQ(-1, xint_cmp_ulong(A, 101));

    // Set A to a negative number
    xint_assign_long(A, -100);
    ASSERT_EQ(-1, xint_cmp_ulong(A, 99));
    ASSERT_EQ(-1, xint_cmp_ulong(A, 100));
    ASSERT_EQ(-1, xint_cmp_ulong(A, 101));
    
    // Set A to a big positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1-1));
    ASSERT_EQ(0, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1));
    ASSERT_EQ(-1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1+1));

    // Set A to a big negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(-1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1-1));
    ASSERT_EQ(-1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1));
    ASSERT_EQ(-1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1+1));

    // Set A to an even bigger positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1));

    // Set A to an even bigger negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(-1, xint_cmp_ulong(A, Ox7FFFFFFFFFFFFFF1));

    xint_delete(A);

    END_TEST(cmp);
}

TEST(cmp, cmp_long)
{
    xint_t A = XINT_INIT_VAL;

    // Start with A = 0
    ASSERT_EQ(1, xint_cmp_long(A, -1));
    ASSERT_EQ(0, xint_cmp_long(A, 0));
    ASSERT_EQ(-1, xint_cmp_long(A, 1));
    
    // Set A to a positive number
    xint_assign_ulong(A, 100);
    ASSERT_EQ(1, xint_cmp_long(A, -1));
    ASSERT_EQ(1, xint_cmp_long(A, 99));
    ASSERT_EQ(0, xint_cmp_long(A, 100));
    ASSERT_EQ(-1, xint_cmp_long(A, 101));

    // Set A to a negative number
    xint_assign_long(A, -100);
    ASSERT_EQ(1, xint_cmp_long(A, -101));
    ASSERT_EQ(0, xint_cmp_long(A, -100));
    ASSERT_EQ(-1, xint_cmp_long(A, -99));
    ASSERT_EQ(-1, xint_cmp_long(A, 0));
    ASSERT_EQ(-1, xint_cmp_long(A, 1));

    // Set A to a big positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(1, xint_cmp_long(A, -1));
    ASSERT_EQ(1, xint_cmp_long(A, Ox7FFFFFFFFFFFFFF1-1));
    ASSERT_EQ(0, xint_cmp_long(A, Ox7FFFFFFFFFFFFFF1));
    ASSERT_EQ(-1, xint_cmp_long(A, Ox7FFFFFFFFFFFFFF1+1));

    // Set A to a big negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    ASSERT_EQ(1, xint_cmp_long(A, -(Ox7FFFFFFFFFFFFFF1+1)));
    ASSERT_EQ(0, xint_cmp_long(A, -(Ox7FFFFFFFFFFFFFF1)));
    ASSERT_EQ(-1, xint_cmp_long(A, -(Ox7FFFFFFFFFFFFFF1-1)));
    ASSERT_EQ(-1, xint_cmp_long(A, 0));
    ASSERT_EQ(-1, xint_cmp_long(A, 1));

    // Set A to an even bigger positive number
    xint_assign_ulong(A, Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(1, xint_cmp_long(A, Ox7FFFFFFFFFFFFFF1));

    // Set A to an even bigger negative number
    xint_assign_long(A, -Ox7FFFFFFFFFFFFFF1);
    xint_add(A, A, A);
    xint_add(A, A, A);
    ASSERT_EQ(-1, xint_cmp_long(A, Ox7FFFFFFFFFFFFFF1));

    xint_delete(A);

    END_TEST(cmp);
}

TEST(cmp, cmpa_cmp)
{
    xint_t A = XINT_INIT_VAL;
    xint_t B = XINT_INIT_VAL;
    xint_t C = XINT_INIT_VAL;
    xint_t D = XINT_INIT_VAL;
    xint_t E = XINT_INIT_VAL;
    xint_t F = XINT_INIT_VAL;
    xint_t G = XINT_INIT_VAL;
    xint_t H = XINT_INIT_VAL;
    xint_t I = XINT_INIT_VAL;
    xint_t J = XINT_INIT_VAL;
    xint_t K = XINT_INIT_VAL;
    xint_t L = XINT_INIT_VAL;
    xint_t M = XINT_INIT_VAL;

    xint_assign_str(A, "-123456789012345678901234567891", 0);
    xint_assign_str(B, "-123456789012345678901234567890", 0);
    xint_assign_str(C, "-123456789012345678901234567889", 0);
    
    xint_assign_str(D, "-1234567891", 0);
    xint_assign_str(E, "-1234567890", 0);
    xint_assign_str(F, "-1234567889", 0);

    xint_assign_str(G, "0", 0);

    xint_assign_str(H, "+1234567889", 0);
    xint_assign_str(I, "+1234567890", 0);
    xint_assign_str(J, "+1234567891", 0);

    xint_assign_str(K, "+123456789012345678901234567889", 0);
    xint_assign_str(L, "+123456789012345678901234567890", 0);
    xint_assign_str(M, "+123456789012345678901234567891", 0);
    
    ASSERT_EQ(-1, xint_cmpa(B, A));
    ASSERT_EQ( 0, xint_cmpa(B, B));
    ASSERT_EQ( 1, xint_cmpa(B, C));
    ASSERT_EQ( 1, xint_cmpa(B, D));
    ASSERT_EQ( 1, xint_cmpa(B, E));
    ASSERT_EQ( 1, xint_cmpa(B, F));
    ASSERT_EQ( 1, xint_cmpa(B, G));
    ASSERT_EQ( 1, xint_cmpa(B, H));
    ASSERT_EQ( 1, xint_cmpa(B, I));
    ASSERT_EQ( 1, xint_cmpa(B, J));
    ASSERT_EQ( 1, xint_cmpa(B, K));
    ASSERT_EQ( 0, xint_cmpa(B, L));
    ASSERT_EQ(-1, xint_cmpa(B, M));

    ASSERT_EQ( 1, xint_cmp(B, A));
    ASSERT_EQ( 0, xint_cmp(B, B));
    ASSERT_EQ(-1, xint_cmp(B, C));
    ASSERT_EQ(-1, xint_cmp(B, D));
    ASSERT_EQ(-1, xint_cmp(B, E));
    ASSERT_EQ(-1, xint_cmp(B, F));
    ASSERT_EQ(-1, xint_cmp(B, G));
    ASSERT_EQ(-1, xint_cmp(B, H));
    ASSERT_EQ(-1, xint_cmp(B, I));
    ASSERT_EQ(-1, xint_cmp(B, J));
    ASSERT_EQ(-1, xint_cmp(B, K));
    ASSERT_EQ(-1, xint_cmp(B, L));
    ASSERT_EQ(-1, xint_cmp(B, M));

    ASSERT_EQ(-1, xint_cmpa(E, A));
    ASSERT_EQ(-1, xint_cmpa(E, B));
    ASSERT_EQ(-1, xint_cmpa(E, C));
    ASSERT_EQ(-1, xint_cmpa(E, D));
    ASSERT_EQ( 0, xint_cmpa(E, E));
    ASSERT_EQ( 1, xint_cmpa(E, F));
    ASSERT_EQ( 1, xint_cmpa(E, G));
    ASSERT_EQ( 1, xint_cmpa(E, H));
    ASSERT_EQ( 0, xint_cmpa(E, I));
    ASSERT_EQ(-1, xint_cmpa(E, J));
    ASSERT_EQ(-1, xint_cmpa(E, K));
    ASSERT_EQ(-1, xint_cmpa(E, L));
    ASSERT_EQ(-1, xint_cmpa(E, M));

    ASSERT_EQ( 1, xint_cmp(E, A));
    ASSERT_EQ( 1, xint_cmp(E, B));
    ASSERT_EQ( 1, xint_cmp(E, C));
    ASSERT_EQ( 1, xint_cmp(E, D));
    ASSERT_EQ( 0, xint_cmp(E, E));
    ASSERT_EQ(-1, xint_cmp(E, F));
    ASSERT_EQ(-1, xint_cmp(E, G));
    ASSERT_EQ(-1, xint_cmp(E, H));
    ASSERT_EQ(-1, xint_cmp(E, I));
    ASSERT_EQ(-1, xint_cmp(E, J));
    ASSERT_EQ(-1, xint_cmp(E, K));
    ASSERT_EQ(-1, xint_cmp(E, L));
    ASSERT_EQ(-1, xint_cmp(E, M));

    ASSERT_EQ(-1, xint_cmpa(G, A));
    ASSERT_EQ(-1, xint_cmpa(G, B));
    ASSERT_EQ(-1, xint_cmpa(G, C));
    ASSERT_EQ(-1, xint_cmpa(G, D));
    ASSERT_EQ(-1, xint_cmpa(G, E));
    ASSERT_EQ(-1, xint_cmpa(G, F));
    ASSERT_EQ( 0, xint_cmpa(G, G));
    ASSERT_EQ(-1, xint_cmpa(G, H));
    ASSERT_EQ(-1, xint_cmpa(G, I));
    ASSERT_EQ(-1, xint_cmpa(G, J));
    ASSERT_EQ(-1, xint_cmpa(G, K));
    ASSERT_EQ(-1, xint_cmpa(G, L));
    ASSERT_EQ(-1, xint_cmpa(G, M));

    ASSERT_EQ( 1, xint_cmp(G, A));
    ASSERT_EQ( 1, xint_cmp(G, B));
    ASSERT_EQ( 1, xint_cmp(G, C));
    ASSERT_EQ( 1, xint_cmp(G, D));
    ASSERT_EQ( 1, xint_cmp(G, E));
    ASSERT_EQ( 1, xint_cmp(G, F));
    ASSERT_EQ( 0, xint_cmp(G, G));
    ASSERT_EQ(-1, xint_cmp(G, H));
    ASSERT_EQ(-1, xint_cmp(G, I));
    ASSERT_EQ(-1, xint_cmp(G, J));
    ASSERT_EQ(-1, xint_cmp(G, K));
    ASSERT_EQ(-1, xint_cmp(G, L));
    ASSERT_EQ(-1, xint_cmp(G, M));

    ASSERT_EQ(-1, xint_cmpa(I, A));
    ASSERT_EQ(-1, xint_cmpa(I, B));
    ASSERT_EQ(-1, xint_cmpa(I, C));
    ASSERT_EQ(-1, xint_cmpa(I, D));
    ASSERT_EQ( 0, xint_cmpa(I, E));
    ASSERT_EQ( 1, xint_cmpa(I, F));
    ASSERT_EQ( 1, xint_cmpa(I, G));
    ASSERT_EQ( 1, xint_cmpa(I, H));
    ASSERT_EQ( 0, xint_cmpa(I, I));
    ASSERT_EQ(-1, xint_cmpa(I, J));
    ASSERT_EQ(-1, xint_cmpa(I, K));
    ASSERT_EQ(-1, xint_cmpa(I, L));
    ASSERT_EQ(-1, xint_cmpa(I, M));

    ASSERT_EQ( 1, xint_cmp(I, A));
    ASSERT_EQ( 1, xint_cmp(I, B));
    ASSERT_EQ( 1, xint_cmp(I, C));
    ASSERT_EQ( 1, xint_cmp(I, D));
    ASSERT_EQ( 1, xint_cmp(I, E));
    ASSERT_EQ( 1, xint_cmp(I, F));
    ASSERT_EQ( 1, xint_cmp(I, G));
    ASSERT_EQ( 1, xint_cmp(I, H));
    ASSERT_EQ( 0, xint_cmp(I, I));
    ASSERT_EQ(-1, xint_cmp(I, J));
    ASSERT_EQ(-1, xint_cmp(I, K));
    ASSERT_EQ(-1, xint_cmp(I, L));
    ASSERT_EQ(-1, xint_cmp(I, M));

    ASSERT_EQ(-1, xint_cmpa(L, A));
    ASSERT_EQ( 0, xint_cmpa(L, B));
    ASSERT_EQ( 1, xint_cmpa(L, C));
    ASSERT_EQ( 1, xint_cmpa(L, D));
    ASSERT_EQ( 1, xint_cmpa(L, E));
    ASSERT_EQ( 1, xint_cmpa(L, F));
    ASSERT_EQ( 1, xint_cmpa(L, G));
    ASSERT_EQ( 1, xint_cmpa(L, H));
    ASSERT_EQ( 1, xint_cmpa(L, I));
    ASSERT_EQ( 1, xint_cmpa(L, J));
    ASSERT_EQ( 1, xint_cmpa(L, K));
    ASSERT_EQ( 0, xint_cmpa(L, L));
    ASSERT_EQ(-1, xint_cmpa(L, M));

    ASSERT_EQ( 1, xint_cmp(L, A));
    ASSERT_EQ( 1, xint_cmp(L, B));
    ASSERT_EQ( 1, xint_cmp(L, C));
    ASSERT_EQ( 1, xint_cmp(L, D));
    ASSERT_EQ( 1, xint_cmp(L, E));
    ASSERT_EQ( 1, xint_cmp(L, F));
    ASSERT_EQ( 1, xint_cmp(L, G));
    ASSERT_EQ( 1, xint_cmp(L, H));
    ASSERT_EQ( 1, xint_cmp(L, I));
    ASSERT_EQ( 1, xint_cmp(L, J));
    ASSERT_EQ( 1, xint_cmp(L, K));
    ASSERT_EQ( 0, xint_cmp(L, L));
    ASSERT_EQ(-1, xint_cmp(L, M));

    xint_delete(A);
    xint_delete(B);
    xint_delete(C);
    xint_delete(D);
    xint_delete(E);
    xint_delete(F);
    xint_delete(G);
    xint_delete(H);
    xint_delete(I);
    xint_delete(J);
    xint_delete(K);
    xint_delete(L);
    xint_delete(M);
    
    END_TEST(cmp);
}

int test_cmp(void)
{
    CALL_TEST(cmp, cmpa_ulong);
    CALL_TEST(cmp, cmp_ulong);
    CALL_TEST(cmp, cmp_long);
    CALL_TEST(cmp, cmpa_cmp);
    END_TEST_GROUP(cmp);
}
