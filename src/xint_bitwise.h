
#ifndef XINT_BITWISE_H
#define XINT_BITWISE_H

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int xint_get_bit(const xint_t u, int bit);
int xint_set_bit(xint_t u, int bit);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_BITWISE_H
