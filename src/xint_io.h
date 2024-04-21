
#ifndef XINT_IO_C
#define XINT_IO_C

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void xint_print_raw(const char *label, xint_t u);
void xint_print(const char *label, const xint_t u);
void xint_print_hex(const char *label, const xint_t u);
char *xint_to_string(const xint_t u, int base);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_IO_C
