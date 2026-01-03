
#include "test_karatsuba.h"

#include "xint.h"
#include "xint_io.h"

#include "test_harness.h"

#include <time.h>

xword_t rand_data[] =
{
    0x747def71d5acf977,
    0x9e150c3e15067881,
    0xbab424e2c9b98173,
    0xb23136341a5fa9bd,
    0xc061dbd62e3f5799,
    0xcd5c03791c168118,
    0x05ba4ccbb9418b4e,
    0x1221e3f36dd3c8f7,
    0x95f0476029ad8f36,
    0xe06a128f7c118d73,
    0x2e924544d37c3210,
    0x3f26755ef365a246,
    0xeaf87ee5bfd7d35a,
    0x3f7aed8fcd6b09e0,
    0xc0ce7d7c2af510ad,
    0xec3de19aa6db3221,
};

TEST_GROUP(karatsuba)

extern int kara_cutoff;

TEST(karatsuba, karatsuba)
{
    xint_t X = XINT_INIT_VAL;
    xint_t Y = XINT_INIT_VAL;
    xint_t Z_algm = XINT_INIT_VAL;
    xint_t Z_kara = XINT_INIT_VAL;
    for (int i=0; i<32; ++i)
    {
        xword_t r1 = rand_data[i%16];
        xint_lshift(X, X, 64);
        xint_add_ulong(X, X, r1);
        xword_t r2 = rand_data[(i+1)%16];
        r2 = (r2 << 19) | (r2 >> 45);
        xint_lshift(Y, Y, 64);
        xint_add_ulong(Y, Y, r2);
    }
    
    kara_cutoff = 10000000;
    
    clock_t start, end;
    start = clock();
    //for (int x=0; x<1000; ++x)
    xint_mul(Z_algm, X, Y);
    end = clock();
    double time_consumed = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf(" (%.0f ms)\n\n", time_consumed);

    kara_cutoff = 8;

    start = clock();
    //for (int x=0; x<1000; ++x)
    xint_mul(Z_kara, X, Y);
    end = clock();
    time_consumed = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf(" (%.0f ms)\n\n", time_consumed);
    
    int d = xint_cmp(Z_algm, Z_kara);
    
    printf("Diff = %d\n", d);
    
    for (int i=0; i<32; ++i)
    {
        printf("%02d: %016lx %016lx %s\n", i, Z_algm->data[i], Z_kara->data[i], Z_algm->data[i] != Z_kara->data[i] ? "***" : "");
    }

//    ASSERT_EQ(0, xint_cmp(c_calc, S));

    END_TEST(karatsuba);
}

int test_karatsuba(void)
{
    CALL_TEST(karatsuba, karatsuba);
    END_TEST_GROUP(karatsuba);
}



