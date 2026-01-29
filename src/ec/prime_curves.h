
#ifndef PRIME_CURVES_H
#define PRIME_CURVES_H

#include "curves.h"

void xint_mod_std(xword_t *w, xword_t *u, const xint_ecc_curve_t *c);

void xint_mod_fast_224(xword_t *w, xword_t *u);
void xint_mod_fast_256(xword_t *w, xword_t *u);
void xint_mod_fast_384(xword_t *w, xword_t *u);
void xint_mod_fast_521(xword_t *w, xword_t *u);

#endif // PRIME_CURVES_H
