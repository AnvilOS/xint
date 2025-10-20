
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
            break;
        }
        // A2.[Take u mod v.]
        // Set r ← u mod v
        xint_mod(r, tmp_u, tmp_v);
        // Set u ← v
        xint_copy(tmp_u, tmp_v);
        // Set v ← r,
        xint_copy(tmp_v, r);
    }

    xint_copy(w, tmp_u);

    xint_delete(tmp_u);
    xint_delete(tmp_v);
    xint_delete(r);
}

void xint_gcd_ext(xint_t gcd, xint_t ud, xint_t vd, const xint_t u, const xint_t v)
{
    // Knuths Algorithm X (Extended Euclid’s algorithm)
    xint_t q = XINT_INIT_VAL;
    xint_t r = XINT_INIT_VAL;
    
    xint_t t1 = XINT_INIT_VAL;
    xint_t t2 = XINT_INIT_VAL;
    xint_t t3 = XINT_INIT_VAL;
    
    xint_t u1 = XINT_INIT_VAL;
    xint_t u2 = XINT_INIT_VAL;
    xint_t u3 = XINT_INIT_VAL;
    
    xint_t v1 = XINT_INIT_VAL;
    xint_t v2 = XINT_INIT_VAL;
    xint_t v3 = XINT_INIT_VAL;
    
    // X1. [Initialize.] Set (u1, u2, u3) ← (1, 0, u), (v1, v2, v3) ← (0, 1, v)
    xint_assign_long(u1, 1);
    xint_assign_zero(u2);
    xint_copy(u3, u);
    
    xint_assign_zero(v1);
    xint_assign_long(v2, 1);
    xint_copy(v3, v);
    
    while (1)
    {
        // X2. [Is v3 = 0?] If v3 = 0, the algorithm terminates.
        if (xint_is_zero(v3))
        {
            break;
        }
        
        // X3. [Divide, subtract.]
        // Set q ← ⌊u3/v3⌋, and then set
        xint_div(q, r, u3, v3);
        
        // (t1, t2, t3) ← (u1, u2, u3) − (v1, v2, v3) q
        xint_mul(t1, v1, q);
        xint_mul(t2, v2, q);
        xint_mul(t3, v3, q);

        xint_sub(t1, u1, t1);
        xint_sub(t2, u2, t2);
        xint_sub(t3, u3, t3);
        
        // (u1, u2, u3) ← (v1, v2, v3)
        xint_copy(u1, v1);
        xint_copy(u2, v2);
        xint_copy(u3, v3);
        
        // (v1, v2, v3) ← (t1, t2, t3).
        xint_copy(v1, t1);
        xint_copy(v2, t2);
        xint_copy(v3, t3);
        
        // Return to step X2.
    }
    
    xint_copy(ud, u1);
    xint_copy(vd, u2);
    xint_copy(gcd, u3);
    
    xint_delete(q);
    xint_delete(r);

    xint_delete(t1);
    xint_delete(t2);
    xint_delete(t3);

    xint_delete(u1);
    xint_delete(u2);
    xint_delete(u3);

    xint_delete(v1);
    xint_delete(v2);
    xint_delete(v3);
}
