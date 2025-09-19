
#ifndef XINT_IO_H
#define XINT_IO_H

#include "xint.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void xint_print_raw(const char *label, xint_t u);
void xint_print(const char *label, const xint_t u);
void xint_print_hex(const char *label, const xint_t u);
char *xint_to_string(const xint_t u, int base);
void xint_from_bin(xint_t x, const uint8_t *s, size_t len);
void xint_from_words(xint_t x, const xword_t *s, size_t len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_IO_H
