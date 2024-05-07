
#ifndef XINT_EXP_C
#define XINT_EXP_C

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

uint32_t xint_exp_1_rl(xint_t x, int a, int e);
uint32_t xint_exp_1_lr(xint_t x, int a, int e);

uint32_t xint_mod_exp(xint_t x, const xint_t base, const xint_t exp, const xint_t mod);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_EXP_C
