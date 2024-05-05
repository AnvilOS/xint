
#include "xint.h"
#include "xint_exp.h"
#include "xint_io.h"
#include "xint_signed.h"

#include "rsa.h"

#include "test_bitwise.h"

#include <stdio.h>

void test_signed(void);

int main(int argc, const char * argv[])
{
    test_rsa();
    test_signed();
    test_bitwise();
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
