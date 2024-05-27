
#include "xint_prime.h"

#include "xint_exp.h"

#include <stdlib.h>

static void xint_rand(xint_t u, xint_t max);

static int primes[] =
{
    3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
    157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
    239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419,
    421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503,
    509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607,
    613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
    709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811,
    821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911,
    919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013,
    1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091,
    1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181,
    1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277,
    1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361,
    1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451,
    1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531,
    1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609,
    1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699,
    1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789,
    1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889,
    1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997,
    1999,
};

int xint_is_prime(xint_t n)
{
    if (xint_cmp_long(n, 2) == 0)
    {
        // 2 is kind of an exception
        return 1;
    }
    
    if ((n->data[0] & 1) == 0)
    {
        // Even numbers are composite
        return 0;
    }
    
    for (int i=0; i<sizeof(primes)/sizeof(primes[0]); ++i)
    {
        if (xint_cmp_long(n, primes[i]) == 0)
        {
            return 1;
        }
        uint32_t r;
        xint_mod_1(&r, n, primes[i]);
        if (r == 0)
        {
            return 0;
        }
    }
    return xint_miller_rabin(n, 10);
}

int xint_miller_rabin(xint_t n, int t)
{
    // From Menezes alg 4.24
    // MILLER-RABIN(n,t)
    // INPUT: an odd integer n >= 3 and security parameter t >= 1.
    // OUTPUT: an answer “prime” or “composite” to the question: “Is n prime?”
    // Output 1 for prime, 0 for composite
    
    // 1. Write n − 1 = 2^s * r such that r is odd.
    int ret = 0;
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
    xint_t a = XINT_INIT_VAL;
    xint_t y = XINT_INIT_VAL;
    for (int i=0; i<t; ++i)
    {
        // 2.1 Choose a random integer a, 2 <= a <= n−2.
        xint_rand(a, nm1);
        
        // 2.2 Compute y = a^r mod n using Algorithm 2.143.
        xint_mod_exp(y, a, r, n);

        // 2.3 If y̸ != 1 and y̸ != n−1 then do the following:
        if (xint_cmp_long(y, 1) != 0 && xint_cmpa(y, nm1) != 0)
        {
            // j = 1.
            // While j <= s − 1 and y != n − 1 do the following:
            for (int j=0; (j<s) && (xint_cmpa(y, nm1) != 0); ++j)
            {
                // Compute y = y^2 mod n.
                xint_sqr(y, y);
                xint_mod(y, y, n);
                // If y == 1 then return(“composite”).
                if (xint_cmp_long(y, 1) == 0)
                {
                    ret = 0;
                    goto cleanup;
                }
                // j = j + 1.
            }
            // If y != n − 1 then return (“composite”).
            if (xint_cmpa(y, nm1) != 0)
            {
                ret = 0;
                goto cleanup;
            }
        }
    }
    // 3. Return(“prime”).
    ret = 1;
    
cleanup:
    xint_delete(nm1);
    xint_delete(r);
    xint_delete(a);
    xint_delete(y);
    return ret;
}

void xint_rand(xint_t u, xint_t max)
{
    // Return a rand less than max
    int x = rand() * 50 / 32676;
    if (x == 0) ++x;
    xint_assign_long(u, x);
}
