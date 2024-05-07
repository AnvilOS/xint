
#include "xint.h"
#include "xint_exp.h"
#include "xint_io.h"
#include "xint_signed.h"
#include "xint_prime.h"

#include "rsa.h"

#include "test_bitwise.h"

#include <stdio.h>

void test_signed(void);

int main(int argc, const char * argv[])
{
    test_rsa();
    test_signed();
    test_bitwise();
    xint_t x = XINT_INIT_VAL;
    xint_assign_uint32(x, 73);
    printf("p73=%d\n", xint_miller_rabin(x, 10));
    xint_assign_uint32(x, 75);
    printf("p75=%d\n", xint_miller_rabin(x, 10));
    xint_assign_uint32(x, 97);
    printf("p97=%d\n", xint_miller_rabin(x, 10));
    xint_assign_uint32(x, 99);
    printf("p99=%d\n", xint_miller_rabin(x, 10));
    xint_t big_prime = XINT_INIT_VAL;
    xint_from_dec_string(big_prime,     "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234568879");
    printf("bigp=%d\n", xint_miller_rabin(big_prime, 10));
    xint_from_dec_string(big_prime,     "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234568877");
    printf("bigc=%d\n", xint_miller_rabin(big_prime, 10));
    xint_t initial = XINT_INIT_VAL;
    xint_from_dec_string(initial, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
    xint_t candidate = XINT_INIT_VAL;
    xint_copy(candidate, initial);
    while (1)
    {
        int result = xint_miller_rabin(candidate, 50);
        if (result == 1)
        {
            break;
        }
        xint_adda_1(candidate, candidate, 1);
    }
    
    xint_t expected = XINT_INIT_VAL;
    xint_from_dec_string(expected,     "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234570637");
    
    printf("Found = %d\n", xint_cmp(candidate, expected));
    xint_t attempts = XINT_INIT_VAL;
    xint_suba(attempts, candidate, initial);
    xint_print("Attempts", attempts);
    return 0;
}

void test_signed(void)
{
    xint_t a = XINT_INIT_VAL;
    xint_t b = XINT_INIT_VAL;
    xint_t c = XINT_INIT_VAL;
    xint_t z = XINT_INIT_VAL;
    
    xint_assign_uint64(a, 500);
    xint_assign_uint64(b, 600);
    xint_assign_uint64(c, 600);
    xint_set_neg(c);
    
    xint_adds(z, a, b); // 1100
    xint_print("Z", z);
    xint_adds(z, a, c); // -100
    xint_print("Z", z);
    xint_adds(z, c, a); // -100
    xint_print("Z", z);
    xint_subs(z, a, b); // -100
    xint_print("Z", z);
    xint_subs(z, a, c); // 1100
    xint_print("Z", z);
    xint_subs(z, c, a); // -1100
    xint_print("Z", z);
}