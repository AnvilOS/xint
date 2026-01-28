
#include "cvt_vectors.h"

#include "test_low_level.h"
#include "test_init_ass.h"
#include "test_cmp.h"
#include "test_abs_arith.h"
#include "test_signed.h"
#include "test_bitwise.h"
#include "test_rsa.h"
#include "test_prime.h"
#include "test_ecc.h"
#include "test_collatz.h"
#include "test_karatsuba.h"
#include "test_ll_mul.h"

int main(int argc, const char * argv[])
{
    cvt_vectors();
    test_low_level();
    test_init_ass();
    test_cmp();
    test_abs_arith();
    test_signed();
    test_bitwise();
    test_rsa();
    test_prime();
    test_ecc();
    test_collatz();
    test_karatsuba();
    test_ll_mul();
}
