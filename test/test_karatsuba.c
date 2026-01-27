
#include "test_karatsuba.h"

#include "xint.h"
#include "xint_io.h"

#include "test_harness.h"

#include "time_stamp.h"

#if defined XDWORD_MAX
xword_t rand_data[] =
{
    0x747def71,
    0x9e150c3e,
    0xbab424e2,
    0xb2313634,
    
    0xc061dbd6,
    0xcd5c0379,
    0x05ba4ccb,
    0x1221e3f3,
    
    0x95f04760,
    0xe06a128f,
    0x2e924544,
    0x3f26755e,
    
    0xeaf87ee5,
    0x3f7aed8f,
    0xc0ce7d7c,
    0xec3de19a,
    
    0x8051142b,
    0xf7998a81,
    0xdeac21e0,
    0x5b30f86e,
    
    0x0ba75bdb,
    0x9102b059,
    0xb2dfd7db,
};
#else
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
    
    0x8051142bd184bf9b,
    0xf7998a811ba7b353,
    0xdeac21e0d4c29508,
    0x5b30f86e0bc1d064,
    
    0x0ba75bdb2fa18b8b,
    0x9102b059ec111e61,
    0xb2dfd7db1ae3c955,
};
#endif

TEST_GROUP(karatsuba)

extern int kara_cutoff;

static const int sz = 64;

TEST(karatsuba, karatsuba)
{
    xint_t X = XINT_INIT_VAL;
    xint_t Y = XINT_INIT_VAL;
    xint_t Z_algm = XINT_INIT_VAL;
    xint_t Z_kara = XINT_INIT_VAL;
    for (int i=0; i<sz; ++i)
    {
        xword_t r1 = rand_data[i%23];
#if defined XDWORD_MAX
        xint_lshift(X, X, 32);
#else
        xint_lshift(X, X, 64);
#endif
        xint_add_ulong(X, X, r1);
        xword_t r2 = rand_data[(i+1)%23];
#if defined XDWORD_MAX
        r2 = (r2 << 9) | (r2 >> 23);
        xint_lshift(Y, Y, 32);
#else
        r2 = (r2 << 29) | (r2 >> 35);
        xint_lshift(Y, Y, 64);
#endif
        xint_add_ulong(Y, Y, r2);
    }
    
    kara_cutoff = 10000000;
    
    STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
        xint_mul(Z_algm, X, Y);
    STAMP_AFTER();
    __enable_irq();
    printf("Schoolbook : %u\n", STAMP_DIFF());

    kara_cutoff = 16;

    STAMP_BEFORE();
        xint_mul(Z_kara, X, Y);
    STAMP_AFTER();
    __enable_irq();
    printf("Karatsuba : %u\n", STAMP_DIFF());
    
    int d = xint_cmp(Z_algm, Z_kara);
    //printf("Diff = %d\n", d);
  
    if (d)
    {
        for (int i=0; i<sz; ++i)
        {
#if defined XDWORD_MAX
            printf("%02d: %08x %08x %s\n", i, Z_algm->data[i], Z_kara->data[i], Z_algm->data[i] != Z_kara->data[i] ? "***" : "");
#else
            printf("%02d: %016lx %016lx %s\n", i, Z_algm->data[i], Z_kara->data[i], Z_algm->data[i] != Z_kara->data[i] ? "***" : "");
#endif
        }
    }

    kara_cutoff = 10000000;

    ASSERT_EQ(0, d);

    END_TEST(karatsuba);
}

int test_karatsuba(void)
{
    CALL_TEST(karatsuba, karatsuba);
    END_TEST_GROUP(karatsuba);
}



