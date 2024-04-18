
#ifndef XINT_H
#define XINT_H

#include <stddef.h>
#include <stdint.h>

#define XINT_INIT_VAL { 0, 0, NULL };

typedef uint32_t xword_t;

struct xint_s
{
    short capacity; // Indicates the total size of the xword array
    short size;     // Words currently in use.
    
    // This stores the data in little-endian format. This will simplify
    // growing and shrinking the array
    xword_t *data;
};
typedef struct xint_s xint_t[1];

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Initialisation functions
int xint_init(xint_t u, size_t hint);
void xint_delete(xint_t u);

// Utility functions
int xint_copy(xint_t u, const xint_t v);
void xint_swap(xint_t u, xint_t v);
int xint_is_zero(xint_t x);
void xint_set_size(xint_t u, int sz);
void xint_get_size(xint_t u);
void xint_negate(xint_t u);
void xint_abs(xint_t u);
void xint_set_neg(xint_t u, int sz);

// Assignment

// Signed arithmetic
int xint_add(xint_t w, const xint_t u, const xint_t v);
int xint_sub(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v);

// Absolute arithmetic
int xint_adda(xint_t w, const xint_t u, const xint_t v);
int xint_suba(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul_1(xint_t w, const xint_t x, xword_t n);
uint32_t xint_div(xint_t q, xint_t r, const xint_t u, xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v);
int xint_add_1(xint_t w, xint_t u, xword_t v);
int xint_cmp(const xint_t u, const xint_t v);
int add(xint_t W, xint_t U, xint_t V);
int sub(xint_t W, xint_t U, xint_t V);
int addsub(xint_t W, xint_t U, xint_t V, int Upos, int Vpos);

// Bit manipulation
uint32_t xint_lshift(xint_t y, const xint_t x, int numbits);
uint32_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H

