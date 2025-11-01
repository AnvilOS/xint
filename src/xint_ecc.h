
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

//#define OLD

struct xint_ecc_curve_s
{
    unsigned nbits;
    char *p;
    char *a;
    char *b;
    char *Gx;
    char *Gy;
    char *n;
    char *h;
#ifdef OLD
    void (*point_add)(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m);
    void (*point_double)(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m);
#else
    void (*point_add)(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t q, const xint_ecc_point_jacobian_t p, const xint_t m);
    void (*point_double)(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p, const xint_t a, const xint_t m);
#endif
};
typedef struct xint_ecc_curve_s xint_ecc_curve_t;

extern xint_ecc_curve_t p224;
extern xint_ecc_curve_t p256;
extern xint_ecc_curve_t p384;
extern xint_ecc_curve_t p521;

int xint_mod_inverse(xint_t w, const xint_t u, const xint_t v);

void xint_point_init(xint_ecc_point_t p);
void xint_point_delete(xint_ecc_point_t p);
void xint_point_copy(xint_ecc_point_t r, const xint_ecc_point_t p);
void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p);
void xint_point_add_pcurve(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m);
void xint_point_double_pcurve(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m);
#ifdef OLD
void xint_point_add_pcurve_jacobian(xint_ecc_point_t r, const xint_ecc_point_t q, const xint_ecc_point_t p, const xint_t m);
void xint_point_double_pcurve_jacobian(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_t a, const xint_t m);
#else
void xint_point_add_pcurve_jacobian(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t q, const xint_ecc_point_jacobian_t p, const xint_t m);
void xint_point_double_pcurve_jacobian(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p, const xint_t a, const xint_t m);
#endif
void xint_ecc_mul_scalar(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, xint_ecc_curve_t curve);

#endif // XINT_ECC_H
