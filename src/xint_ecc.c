
#include "xint_ecc.h"
#include "xint_io.h"

void xint_gcd(xint_t w, const xint_t u, const xint_t v)
{
    // Knuths Algorithm A (Modern Euclidean algorithm)
    xint_t tmp_u = XINT_INIT_VAL;
    xint_t tmp_v = XINT_INIT_VAL;
    xint_t r = XINT_INIT_VAL;
    xint_copy(tmp_u, u);
    xint_copy(tmp_v, v);
    while (1)
    {
        // A1. [v = 0?] If v = 0, the algorithm terminates with u as the answer.
        if (xint_is_zero(tmp_v))
        {
            xint_copy(w, tmp_u);
            xint_delete(tmp_u);
            xint_delete(tmp_v);
            xint_delete(r);
            return;
        }
        // A2.[Take u mod v.]
        // Set r ← u mod v
        xint_mod(r, tmp_u, tmp_v);
        // Set u ← v
        xint_copy(tmp_u, tmp_v);
        // Set v ← r,
        xint_copy(tmp_v, r);
    }
}
