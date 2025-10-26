
#include "xint_bitwise.h"

int xint_get_bit(xint_t u, int bit)
{
    // Fistly find our bit
    int xword_no = bit / XWORD_BITS;
    int bit_no = bit % XWORD_BITS;
    int Un = abs(u->size);

    if (xword_no >= Un)
    {
        // We are out of range so we are in the sign extend area
        return 0;
    }
    
    xword_t xword = u->data[xword_no];
    int bitval = (xword >> bit_no) & 1;
    return bitval;
}

int xint_set_bit(xint_t u, int bit)
{
    // Fistly find our bit
    int xword_no = bit / XWORD_BITS;
    int bit_no = bit % XWORD_BITS;
    int Un = abs(u->size);
    
    if (xword_no >= Un)
    {
        return 0;
    }

    xword_t xword = u->data[xword_no];
    int bitval = (xword >> bit_no) & 1;

    u->data[xword_no] |= 1 << bit_no;
    return bitval;
}

