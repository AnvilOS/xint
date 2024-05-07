
#include "xint_prime.h"

#include "xint_exp.h"

#include <stdlib.h>

static void xint_rand(xint_t u, xint_t max);

xword_t p_one[] = { 1 };

xint_t one = { 1, 1, (xword_t *)&p_one };

int xint_miller_rabin(xint_t n, int t)
{
    if ((n->data[0] & 1) == 0)
    {
        return 0;
    }
    // From Menezes alg 4.24
    // MILLER-RABIN(n,t)
    // INPUT: an odd integer n >= 3 and security parameter t >= 1.
    // OUTPUT: an answer “prime” or “composite” to the question: “Is n prime?”
    // Output 1 for prime, 0 for composite
    
    // 1. Write n − 1 = 2^s * r such that r is odd.
    xint_t nm1 = XINT_INIT_VAL;
    xint_suba_1(nm1, n, 1);
    // Remove as many factors of 2 as possible
    int s = 0;
    xint_t r = XINT_INIT_VAL;
    xint_copy(r, nm1);
    while ((r->data[0] & 1) == 0)
    {
        xint_rshift(r, r, 1);
        ++s;
    }

    // 2. For i from 1 to t do the following:
    for (int i=0; i<t; ++i)
    {
        // 2.1 Choose a random integer a, 2 <= a <= n−2.
        xint_t a = XINT_INIT_VAL;
        xint_rand(a, nm1);
        
        // 2.2 Compute y = ar mod n using Algorithm 2.143.
        xint_t y = XINT_INIT_VAL;
        xint_mod_exp(y, a, r, n);

        // 2.3 If y̸ != 1 and y̸ != n−1 then do the following:
        if (xint_cmp(y, one) != 0 && xint_cmp(y, nm1) != 0)
        {
            // j = 1.
            // While j <= s − 1 and y != n − 1 do the following:
            for (int j=0; (j<s) && (xint_cmp(y, nm1) != 0); ++j)
            {
                // Compute y = y^2 mod n.
                xint_sqr(y, y);
                xint_mod(y, y, n);
                // If y == 1 then return(“composite”).
                if (xint_cmp(y, one) == 0)
                {
                    return 0; // composite
                }
                // j = j + 1.
            }
            // If y != n − 1 then return (“composite”).
            if (xint_cmp(y, nm1) != 0)
            {
                return 0; // composite
            }
        }
    }
    // 3. Return(“prime”).
    return 1; // prime
}

void xint_rand(xint_t u, xint_t max)
{
    // Return a rand less than max
    int x = rand() * 50 / 32676;
    if (x == 0) ++x;
    xint_assign_uint32(u, x);
}
