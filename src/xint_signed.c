
#include "xint_signed.h"

static int add_signed(xint_t W, xint_t U, xint_t V, int Upos, int Vpos);

int adds(xint_t W, xint_t U, xint_t V)
{
    return add_signed(W, U, V, !xint_is_neg(U), !xint_is_neg(V));
}

int subs(xint_t W, xint_t U, xint_t V)
{
    return add_signed(W, U, V, !xint_is_neg(U), xint_is_neg(V));
}

int add_signed(xint_t W, xint_t U, xint_t V, int Upos, int Vpos)
{
    if (Upos == Vpos)
    {
        xint_adda(W, U, V);
        if (!Upos)
        {
            xint_chs(W);
        }
    }
    else
    {
        if (Upos)
        {
            if (xint_suba(W, U, V) < 0)
            {
                xint_chs(W);
            }
        }
        else
        {
            if (xint_suba(W, V, U) < 0)
            {
                xint_chs(W);
            }
        }
    }
    return -1;
}
