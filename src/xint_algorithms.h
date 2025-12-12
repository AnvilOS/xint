
#ifndef XINT_PRIME_H
#define XINT_PRIME_H

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int xint_is_prime(const xint_t n);
int xint_miller_rabin(const xint_t n, int t);
void xint_gcd(xint_t w, const xint_t u, const xint_t v);
void xint_gcd_ext(xint_t gcd, xint_t ud, xint_t vd, const xint_t u, const xint_t v);
void xint_sqrt(xint_t w, xint_t r, const xint_t u);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_PRIME_H
