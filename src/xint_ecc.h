
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

struct ecc_sig
{
    xint_t r;
    xint_t s;
};
typedef struct ecc_sig xint_ecc_sig_t[1];

struct ecc_point_jacobian
{
    xword_t *x;
    xword_t *y;
    xword_t *z;
    int is_at_infinity;
    int nwords;
};
typedef struct ecc_point_jacobian xint_ecc_point_jacobian_t[1];

void xint_point_init(xint_ecc_point_t p);
void xint_point_delete(xint_ecc_point_t p);
void xint_point_copy(xint_ecc_point_t r, const xint_ecc_point_t p);
void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p);

typedef struct xint_ecc_curve_s xint_ecc_curve_t;

void xint_ecc_mul_scalar_joye(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
void ecc_gen_deterministic_k(xint_t k, uint8_t *h1, int hlen, xint_t x, const xint_ecc_curve_t *c);

int xint_ecc_get_public_key(xint_ecc_point_t pub, xint_t priv, const xint_ecc_curve_t *c);

int xint_ecc_sign_det(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_t priv, const xint_ecc_curve_t *c);
int xint_ecc_verify(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_ecc_point_t pub, const xint_ecc_curve_t *c);

void xint_ecc_mul_scalar_plain(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);

void xint_point_jacobian_init(xint_ecc_point_jacobian_t p, int nwords);
void from_jacobian(xint_ecc_point_t w, const xint_ecc_point_jacobian_t u, const xint_ecc_curve_t *c);
void xint_point_jacobian_init(xint_ecc_point_jacobian_t p, int nwords);
void xint_point_jacobian_delete(xint_ecc_point_jacobian_t p);
void xint_point_jacobian_copy(xint_ecc_point_jacobian_t r, const xint_ecc_point_jacobian_t p);
void to_jacobian(xint_ecc_point_jacobian_t w, const xint_ecc_point_t u);

void xint_ecc_mul_scalar_jacobian(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
void xint_ecc_mul_scalar_shamir(xint_ecc_point_t R, const xint_ecc_point_t S, const xint_ecc_point_t G, const xint_t u1, const xint_t u2, const xint_ecc_curve_t *c);

#endif // XINT_ECC_H
