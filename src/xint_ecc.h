
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
    void (*xint_mod_fast)(xword_t *w, xword_t *u);
};
typedef struct xint_ecc_curve_s xint_ecc_curve_t;

extern const xint_ecc_curve_t k163;
extern const xint_ecc_curve_t p224;
extern const xint_ecc_curve_t p256;
extern const xint_ecc_curve_t p384;
extern const xint_ecc_curve_t p521;

void xint_mod_std(xword_t *w, xword_t *u, const xint_ecc_curve_t *c);

void xint_mod_fast_k163(xword_t *w, xword_t *u);
void xint_mod_fast_224(xword_t *w, xword_t *u);
void xint_mod_fast_256(xword_t *w, xword_t *u);
void xint_mod_fast_384(xword_t *w, xword_t *u);
void xint_mod_fast_521(xword_t *w, xword_t *u);

void xint_point_init(xint_ecc_point_t p);
void xint_point_delete(xint_ecc_point_t p);
void xint_point_copy(xint_ecc_point_t r, const xint_ecc_point_t p);
void xint_point_negate(xint_ecc_point_t r, xint_ecc_point_t p);

void xint_ecc_mul_scalar(xint_ecc_point_t R, const xword_t *Px, const xword_t *Py, const xint_t k, const xint_ecc_curve_t *c);
void xint_ecc_mul_scalar_joye(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
void ecc_gen_deterministic_k(xint_t k, uint8_t *h1, int hlen, xint_t x, const xint_ecc_curve_t *c);

int xint_ecc_get_public_key(xint_ecc_point_t pub, xint_t priv, const xint_ecc_curve_t *c);


int xint_ecc_sign_det(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_t priv, const xint_ecc_curve_t *c);
int xint_ecc_verify(xint_ecc_sig_t sig, unsigned char *digest, int digest_len, xint_ecc_point_t pub, const xint_ecc_curve_t *c);

void xint_ecc_mul_scalar_plain(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);
void xint_point_add(xint_ecc_point_t r, xint_ecc_point_t q, xint_ecc_point_t p, xint_t m);
void xint_point_double(xint_ecc_point_t r, xint_ecc_point_t p, xint_t a, xint_t m);

struct ecc_point_jacobian
{
    int is_at_infinity;
    xint_t x;
    xint_t y;
    xint_t z;
};
typedef struct ecc_point_jacobian xint_ecc_point_jacobian_t[1];

void xint_ecc_mul_scalar_jacobian(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);

#endif // XINT_ECC_H
