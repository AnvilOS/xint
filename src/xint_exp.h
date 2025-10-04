
#ifndef XINT_EXP_C
#define XINT_EXP_C

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void xint_exp_1_rl(xint_t x, int a, int e);
void xint_exp_1_lr(xint_t x, int a, int e);

void xint_mod_exp(xint_t x, const xint_t base, const xint_t exp, const xint_t mod);
void xint_mod_exp_kary(xint_t x, const xint_t base, const xint_t exp, const xint_t mod);
void xint_mod_exp_old(xint_t x, const xint_t base, const xint_t exp, const xint_t mod);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_EXP_C
