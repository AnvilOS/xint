
#ifndef XINT_PRIME_H
#define XINT_PRIME_H

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int xint_is_prime(xint_t n);
int xint_miller_rabin(xint_t n, int t);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_PRIME_H
