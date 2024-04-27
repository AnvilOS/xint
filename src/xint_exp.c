
#include "xint_exp.h"

uint32_t xint_exp_1_rl(xint_t x, int a, int e)
{
    // x = x . a ^ b
    xint_t tmp;
    xint_init(tmp, 20);

    xint_assign_uint64(x, 1);
    xint_assign_uint64(tmp, a);
    while (e)
    {
        if (e & 1)
        {
            xint_mul(x, x, tmp);
        }
        e >>= 1;
        xint_sqr(tmp, tmp);
    }
    return 0;
}

uint32_t xint_exp_1_lr(xint_t x, int a, int e)
{
    xint_assign_uint64(x, 1);
    int highest_bit = 31 - __builtin_clz(e);
    uint32_t mask = 1 << highest_bit;
    while (mask)
    {
        xint_sqr(x, x);
        if (e & mask)
        {
            xint_mul_1(x, x, a);
        }
        mask >>= 1;
    }
    return 0;
}

uint32_t xint_mod_exp(xint_t x, xint_t base, xint_t exp, xint_t mod)
{
    // This is the vanilla wikipedia algorithm
    xint_assign_uint64(x, 1);
    for (int j=exp->size-1; j>=0; --j)
    {
        xword_t word = exp->data[j];
        uint32_t mask = 1 << 31;
        while (mask)
        {
            xint_sqr(x, x);
            xint_mod(x, x, mod);
            if (word & mask)
            {
                xint_mul(x, x, base);
                xint_mod(x, x, mod);
            }
            mask >>= 1;
        }
    }
    return 0;
}
