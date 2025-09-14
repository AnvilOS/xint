
#include "xint_exp.h"
#include "xint_io.h"

#include <stdio.h>

uint32_t xint_exp_1_rl(xint_t x, int a, int e)
{
    // x = x . a ^ b
    xint_t tmp;
    xint_init(tmp);

    xint_assign_ulong(x, 1);
    xint_assign_ulong(tmp, a);
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
    xint_assign_ulong(x, 1);
    int highest_bit = 31 - __builtin_clz(e);
    uint32_t mask = 1 << highest_bit;
    while (mask)
    {
        xint_sqr(x, x);
        if (e & mask)
        {
            xint_mul_ulong(x, x, a);
        }
        mask >>= 1;
    }
    return 0;
}

uint32_t xint_mod_exp(xint_t x, const xint_t base, const xint_t exp, const xint_t mod)
{
    int max_win_sz = 6;
    xword_t window_mask = ((xword_t)-1) >> (XWORD_BITS - max_win_sz);
    int wordnum = xint_size(exp) - 1;
    
    // Figure out where the msb is
    xword_t curr_word = exp->data[wordnum--];
    xword_t mask = (xword_t)1 << (XWORD_BITS - 1);
    while (!(curr_word & mask))
    {
        mask >>= 1;
    }

    xint_assign_ulong(x, 1);
    xint_t g[window_mask + 2];

    xint_init(g[1]);
    xint_copy(g[1], base);
    xint_mod(g[1], g[1], mod);

    xint_init(g[2]);
    xint_sqr(g[2], g[1]);
    xint_mod(g[2], g[2], mod);

    for (int i=1; i<(window_mask>>1)+1; ++i)
    {
        xint_init(g[2*i+1]);
        xint_mul(g[2*i+1], g[2*i-1], g[2]);
        xint_mod(g[2*i+1], g[2*i+1], mod);
    }

    int window = 0;
    int trailing_zeros = 0;
    int nbits = 0;

    while (1)
    {
        // Get the current bit
        xword_t bit = (curr_word & mask) != 0;
        mask >>= 1;

        if (!window && (bit == 0))
        {
            xint_sqr(x, x);
            xint_mod(x, x, mod);
        }
        else
        {
            // We are in a window
            window <<= 1;
            window |= bit;
            if (bit == 0)
            {
                ++trailing_zeros;
            }
            else
            {
                trailing_zeros = 0;
            }
            if ((++nbits == max_win_sz) || ((wordnum < 0) && (mask == 0)))
            {
                // Remove the trailing zeroes
                window >>= trailing_zeros;
                for (int j=0; j<nbits-trailing_zeros; ++j)
                {
                    xint_sqr(x, x);
                    xint_mod(x, x, mod);
                }
                xint_mul(x, x, g[window]);
                xint_mod(x, x, mod);
                for (int j=0; j<trailing_zeros; ++j)
                {
                    xint_sqr(x, x);
                    xint_mod(x, x, mod);
                }
                window = 0;
                nbits = 0;
            }
        }
        if (mask == 0)
        {
            if (wordnum < 0)
            {
                break;
            }
            curr_word = exp->data[wordnum--];
            mask = (xword_t)1 << (XWORD_BITS - 1);
        }
    }
    xint_delete(g[1]);
    xint_delete(g[2]);
    for (int i=1; i<(window_mask>>1)+1; ++i)
    {
        xint_delete(g[2*i+1]);
    }
    return 0;
}

uint32_t xint_mod_exp_kary(xint_t x, const xint_t base, const xint_t exp, const xint_t mod)
{
    int win_sz = 1;
    uint32_t window_mask = ((uint32_t)-1) >> (32 - win_sz);
    
    int nwords = xint_size(exp);
    int wordnum = nwords - 1;
    
    // Figure out where the msb is
    xword_t curr_word = exp->data[wordnum];
    --wordnum;
    uint32_t mask = 1 << 31;
    int remaining_in_word = 32;
    while (!(curr_word & mask))
    {
        mask >>= 1;
        --remaining_in_word;
    }

    xint_assign_ulong(x, 1);
    xint_t g[window_mask + 1];
    xint_init(g[0]);
    xint_assign_ulong(g[0], 1);
    for (int i=1; i<window_mask+1; ++i)
    {
        xint_init(g[i]);
        xint_mul(g[i], g[i-1], base);
    }
    
    while (1)
    {
        // Get win_sz bits
        xword_t window;
        if (remaining_in_word >= win_sz)
        {
            window = curr_word >> (remaining_in_word - win_sz);
            remaining_in_word -= win_sz;
        }
        else
        {
            window = curr_word;
            if (wordnum >= 0)
            {
                int bits_needed = win_sz - remaining_in_word;
                window <<= bits_needed;
                curr_word = exp->data[wordnum];
                --wordnum;
                remaining_in_word = 32 - bits_needed;
                window |= curr_word >> remaining_in_word;
            }
            else if (remaining_in_word)
            {
                window <<= 32 - remaining_in_word;
                window >>= 32 - remaining_in_word;
                win_sz = remaining_in_word;
                remaining_in_word = 0;
            }
            else
            {
                return 0;
            }
        }
        window &= window_mask;
                
        for (int i=0; i<win_sz; ++i)
        {
            xint_sqr(x, x);
            xint_mod(x, x, mod);
        }
        if (window)
        {
            xint_mul(x, x, g[window]);
            xint_mod(x, x, mod);
        }
    }
    return 0;
}

uint32_t xint_mod_exp_old(xint_t x, const xint_t base, const xint_t exp, const xint_t mod)
{
    // This is the vanilla wikipedia algorithm
    xint_assign_ulong(x, 1);
    for (int j=xint_size(exp)-1; j>=0; --j)
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
