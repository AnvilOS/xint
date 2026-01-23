
#include "test_ll_mul.h"

#include "xint.h"
#include "xint_io.h"
#include "xint_internal.h"

#include "test_harness.h"
#include "time_stamp.h"

xword_t u[] =
{
#if defined XDWORD_MAX
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
#else
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
#endif
};

xword_t v[] =
{   
#if defined XDWORD_MAX
    0x4549ee93,
    0x449766fc,
    0x72c1a9ae,
    0x2b0757b9,

    0x426f7a0c,
    0x6205723a,
    0xf00c0d3a,
    0xab1f62b2,
    
    0xd5bbec7b,
    0x939fa4d4,
    0x801f3396,
    0xdb12c507,
    
    0x6d16bb3c,
    0x62ef7db5,
    0x5a944fd8,
    0xfa9c7511,
    
    0x3062fded,
    0x47000d53,
    0xd19239d6,
    0xad6f68cb,
    
    0xe84dbf5e,
    0x12cd415e,
    0x8dde99a8,
#else
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
#endif
};

#define MINUV 2
#define MAXUV 32
xword_t w_c[2*MAXUV];
xword_t w_asm[2*MAXUV];
xword_t w_2x2[2*MAXUV];
xword_t w_sq[2*MAXUV];
xword_t w_sq_asm[2*MAXUV];

TEST_GROUP(ll_mul)

#define TRACE(...)
//#define TRACE printf

TEST(ll_mul, mul_asm)
{
    int errors = 0;
    int cmp;
    STAMP_VARS();
    TRACE("        MUL C  MUL A  MUL2x2\n");
    for (int Vn=MINUV; Vn<=MAXUV; ++Vn)
    {
        for (int Un=MINUV; Un<=MAXUV; ++Un)
        {
            TRACE(" %2dx%2d:", Un, Vn);
            __disable_irq();
            STAMP_BEFORE();
            xll_mul_c(w_c, u, Un, v, Vn);
            STAMP_AFTER();
            __enable_irq();
            TRACE("%7lu", STAMP_DIFF());
#if defined XINT_USE_MUL_ASM
            __disable_irq();
            STAMP_BEFORE();;
            xll_mul_asm(w_asm, u, Un, v, Vn);
            STAMP_AFTER();
            __enable_irq();
            TRACE("%6lu", STAMP_DIFF());
            cmp = xll_cmp(w_c, w_asm, Un+Vn);
            if (cmp) ++errors;
            TRACE("%c", cmp ? '*' : ' ');
            if (Un == 8 && Un == Vn) {
            __disable_irq();
                STAMP_BEFORE();
                uECC_vli_mult(w_asm, u, v, Un);
            STAMP_AFTER();
            __enable_irq();
                TRACE("%7lu", STAMP_DIFF());
            }
#endif
            TRACE("\n");
        }
    }
    ASSERT_EQ(0, errors);

    END_TEST(ll_mul);
}

TEST(ll_mul, squ_asm)
{
    STAMP_VARS();
    TRACE("        MUL C  SQU C  MUL A  SQU A\n");
    for (int Un=MINUV; Un<=MAXUV; ++Un)
    {
        TRACE(" %2dx%2d:", Un, Un);
        __disable_irq();
        STAMP_BEFORE();
        xll_mul_c(w_c, u, Un, u, Un);
        STAMP_AFTER();
        __enable_irq();
        TRACE("%7lu", STAMP_DIFF());
        __disable_irq();
        STAMP_BEFORE();
        xll_squ_c(w_sq, u, Un);
        STAMP_AFTER();
        __enable_irq();
        TRACE("%6lu", STAMP_DIFF());
        TRACE("%c", xll_cmp(w_c, w_sq, 2*Un) ? '*' : ' ');
#if defined XINT_USE_MUL_ASM
        __disable_irq();
        STAMP_BEFORE();
        xll_mul_asm(w_asm, u, Un, u, Un);
        STAMP_AFTER();
        __enable_irq();
        TRACE("%6lu", STAMP_DIFF());
        long ddd = STAMP_DIFF();
        TRACE("%c", xll_cmp(w_c, w_asm, 2*Un) ? '*' : ' ');
#endif
#if defined XINT_USE_MUL_ASM
        __disable_irq();
        STAMP_BEFORE();
        xll_squ_asm(w_sq_asm, u, Un);
        STAMP_AFTER();
        __enable_irq();
        TRACE("%6lu %d", STAMP_DIFF(), ddd - STAMP_DIFF());
        TRACE("%c", xll_cmp(w_c, w_sq_asm, 2*Un) ? '*' : ' ');
#endif
        TRACE("\n");
    }

    ASSERT_EQ(0, 0);

    END_TEST(ll_mul);
}

int test_ll_mul(void)
{
    CALL_TEST(ll_mul, mul_asm);
    CALL_TEST(ll_mul, squ_asm);

    END_TEST_GROUP(ll_mul);
}



