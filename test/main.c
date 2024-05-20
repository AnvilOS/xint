
#include "test_abs_arith.h"
#include "test_signed.h"
#include "test_bitwise.h"
#include "test_rsa.h"
#include "test_prime.h"

int main(int argc, const char * argv[])
{
    test_abs_arith();
    test_signed();
    test_bitwise();
    test_rsa();
    test_prime();
}
