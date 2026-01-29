
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
int xint_mod_inverse(xint_t w, const xint_t u, const xint_t v);
void xint_mod_add(xint_t w, const xint_t u, const xint_t v, const xint_t m);
void xint_mod_sub(xint_t w, const xint_t u, const xint_t v, const xint_t m);
void xint_mod_mul(xint_t w, const xint_t u, const xint_t v, const xint_t m);
void xint_mod_mul_ulong(xint_t w, const xint_t u, unsigned long v, const xint_t m);
void xint_mod_lshift(xint_t w, const xint_t u, int nbits, const xint_t m);
void xint_mod_sqr(xint_t w, const xint_t u, const xint_t m);
void xint_sqrt(xint_t w, xint_t r, const xint_t u);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_PRIME_H
