
#include "xint_signed.h"

static int add_signed(xint_t w, xint_t u, xint_t v, int Upos, int Vpos);

int xint_adds(xint_t w, xint_t u, xint_t v)
{
    return add_signed(w, u, v, !xint_is_neg(u), !xint_is_neg(v));
}

int xint_subs(xint_t w, xint_t u, xint_t v)
{
    return add_signed(w, u, v, !xint_is_neg(u), xint_is_neg(v));
}

int add_signed(xint_t w, xint_t u, xint_t v, int upos, int vpos)
{
    if (upos == vpos)
    {
        xint_adda(w, u, v);
        upos ? xint_set_pos(w) : xint_set_neg(w);
    }
    else
    {
        if (xint_suba(w, u, v) < 0)
        {
            upos ? xint_set_neg(w) : xint_set_pos(w);
        }
        else
        {
            upos ? xint_set_pos(w) : xint_set_neg(w);
        }
    }
    return -1;
}
