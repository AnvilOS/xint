
#ifndef XINT_ECC_H
#define XINT_ECC_H

#include "xint.h"

void xint_gcd(xint_t w, const xint_t u, const xint_t v);
void xint_gcd_ext(xint_t gcd, xint_t ud, xint_t vd, const xint_t u, const xint_t v);

#endif // XINT_ECC_H
