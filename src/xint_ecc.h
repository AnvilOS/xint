
#ifndef XINT_ECC_H
#define XINT_ECC_H

#include "xint.h"

struct ecc_point
{
    int is_at_infinity;
    xint_t x;
    xint_t y;
};
typedef struct ecc_point xint_ecc_point_t[1];

struct ecc_point_jacobian
{
    int is_at_infinity;
    xint_t x;
    xint_t y;
    xint_t z;
};
typedef struct ecc_point_jacobian xint_ecc_point_jacobian_t[1];

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
    void (*point_add)(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t q, const xint_ecc_point_jacobian_t p, const struct xint_ecc_curve_s curve);
    void (*point_double)(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p, const struct xint_ecc_curve_s curve);
    void (*xint_mod_fast)(xword_t *w, xword_t *u);
};
typedef struct xint_ecc_curve_s xint_ecc_curve_t;

extern const xint_ecc_curve_t p224;
extern const xint_ecc_curve_t p256;
extern const xint_ecc_curve_t p384;
extern const xint_ecc_curve_t p521;

int xint_mod_inverse(xint_t w, const xint_t u, const xint_t v);

void xint_mod_std(xword_t *w, xword_t *u, const xint_ecc_curve_t *c);

void xint_mod_fast_224(xword_t *w, xword_t *u);
void xint_mod_fast_256(xword_t *w, xword_t *u);
void xint_mod_fast_384(xword_t *w, xword_t *u);
void xint_mod_fast_521(xword_t *w, xword_t *u);

void xint_point_init(xint_ecc_point_t p);
void xint_point_delete(xint_ecc_point_t p);
void xint_point_copy(xint_ecc_point_t r, const xint_ecc_point_t p);
void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p);
void xint_point_add(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t q, const xint_ecc_point_jacobian_t p, const xint_ecc_curve_t *c);
void xint_point_double(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p, const xint_ecc_curve_t *c);
void xint_ecc_mul_scalar(xint_ecc_point_t R, const xword_t *Px, const xword_t *Py, const xint_t k, const xint_ecc_curve_t *c);
void xint_ecc_mul_scalar_joye(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
void ecc_gen_deterministic_k(char *m, char *x, xint_t q_int, int qlen);

#endif // XINT_ECC_H
