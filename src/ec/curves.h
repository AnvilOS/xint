
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

struct xint_ecc_curve_s
{
    unsigned nbits;
    unsigned nwords;
    const xword_t *p;
    const xword_t *a;
    const xword_t *b;
    const xword_t *Gx;
    const xword_t *Gy;
    const xword_t *n;
    const xword_t *h;
    void (*affine_add)(xint_ecc_point_t Rj, const xint_ecc_point_t Pj, const xint_ecc_point_t Qj, const xint_ecc_curve_t *c);
    void (*affine_double)(xint_ecc_point_t Rj, const xint_ecc_point_t Pj, const xint_ecc_curve_t *c);
    void (*xint_mod_fast)(xword_t *w, xword_t *u);
    void (*jacobian_add)(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c);
    void (*jacobian_double)(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c);
    const xword_t low_polinomial;
    const int *exp;
    const int num_exp;
    const xword_t *x;
    void (*scalar_mul)(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
    const xword_t *sqrt_b;
};

extern const xint_ecc_curve_t p192;
extern const xint_ecc_curve_t p224;
extern const xint_ecc_curve_t p256;
extern const xint_ecc_curve_t p384;
extern const xint_ecc_curve_t p521;

extern const xint_ecc_curve_t k163;
extern const xint_ecc_curve_t k233;
extern const xint_ecc_curve_t k283;
extern const xint_ecc_curve_t k409;
extern const xint_ecc_curve_t k571;

extern const xint_ecc_curve_t b163;
extern const xint_ecc_curve_t b233;
extern const xint_ecc_curve_t b283;
extern const xint_ecc_curve_t b409;
extern const xint_ecc_curve_t b571;

#endif // CURVES_H
