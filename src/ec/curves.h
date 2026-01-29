
#ifndef CURVES_H
#define CURVES_H

#include "xint_internal.h"
#include "xint_ecc.h"

#if defined XDWORD_MAX
#define X(__a, __b) __a, __b
#else
#define X(__a, __b) (xword_t)__b<<32|__a
#endif

#define CURVE_WORDS(__nbits) (((__nbits-1)/XWORD_BITS)+1)

extern const xint_ecc_curve_t k163;
extern const xint_ecc_curve_t p224;
extern const xint_ecc_curve_t p256;
extern const xint_ecc_curve_t p384;
extern const xint_ecc_curve_t p521;

#endif // CURVES_H
