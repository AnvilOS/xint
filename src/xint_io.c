
#include "xint_io.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void xint_to_buf(unsigned char *buf, int len, xint_t u)
{
    for (long j=0, n=len-1; j<xint_size(u); ++j)
    {
        xword_t w = u->data[j];
        for (int i=0; i<sizeof(xword_t); ++i)
        {
            buf[n--] = w;
            if (n < 0)
            {
                break;
            }
            w >>= 8;
        }
    }
}

void xint_print_raw(const char *label, const xint_t u)
{
    printf("%s = [ ", label);
    for (long j=xint_size(u)-1; j>=0; --j)
    {
        printf(XWORD_BITS==32?"%08x":"%016lx", u->data[j]);
        if (j)
        {
            printf(", ");
        }
    }
    printf(" ]");
    printf("\n");
}

void xint_print(const char *label, const xint_t u)
{
    printf("%s: ", label);
    if (xint_is_neg(u))
    {
        printf("-");
    }
    const char *str = xint_to_string(u, 10);
    const char *p = str;
    int nchars = (int)strlen(str);
    int first = nchars % 3;
    if (first == 0)
    {
        first = 3;
    }
    printf("%.*s", first, p);
    nchars -= first;
    p += first;
    for (int i=0; i<nchars; i+=3)
    {
        printf(",%.3s", p);
        p += 3;
    }
    printf("\n");
    free((void*)str);
}

void xint_print_hex(const char *label, const xint_t u)
{
    printf("%s: ", label);
    const char *str = xint_to_string(u, 16);
    printf("%s\n", str);
    free((void*)str);
}

char *xint_to_string(const xint_t u, int base)
{
    xint_t TEMP;
    char *str;
    int n = 0;

    xint_init(TEMP);
    xint_copy(TEMP, u);

    // Figure out how much space we need
    size_t needed;
    if (base == 10)
    {
        // needed = size * log10(2^32)
        //        = size * 32 * log10(2)
        //        = size * 9.633
        needed = xint_size(TEMP) * XWORD_BITS / 32 * 9633 / 1000;
    }
    else if (base == 16)
    {
        // needed = size * log16(2^32)
        //        = size * 32 * log16(2)
        //        = size * 32 * 0.25
        needed = xint_size(TEMP) * XWORD_BITS / 32 * 8;
    }
    else
    {
        return NULL;
    }
    needed += 2;
    
    str = (char *)malloc(needed);
    
    while (1)
    {
        xword_t ch;
        xint_div_1(TEMP, &ch, TEMP, base);
        if (ch >= 0 && ch <= 9)
        {
            str[n] = ch + '0';
        }
        else if (ch >= 10 && ch <= 15)
        {
            str[n] = ch - 10 + 'a';
        }
        else
        {
            str[n] = '*';
        }
        if (n + 1 >= needed)
        {
            printf("OOOOOM\n");
            exit(1);
        }
        ++n;
        if (xint_is_zero(TEMP))
        {
            str[n] = 0;
            break;
        }
    }
    
    // Reverse the string
    size_t len = strlen(str);
    for (int i=0; i<len/2; ++i)
    {
        char tmpc = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmpc;
    }
    
    xint_delete(TEMP);
    return str;
}

void xint_from_bin(xint_t x, const uint8_t *p, size_t len)
{
    x->size = 0;
    while (len--)
    {
        xint_muladd_ulong(x, x, 256, *p);
        ++p;
    }
}

void xint_from_words(xint_t x, const xword_t *p, size_t len)
{
    x->size = 0;
    while (len--)
    {
        xint_lshift(x, x, XWORD_BITS);
        xint_add_ulong(x, x, *p);
        ++p;
    }
}
