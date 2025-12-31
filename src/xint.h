
#ifndef XINT_H
#define XINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#if __arm__
typedef unsigned xword_t;
typedef unsigned long long xdword_t;
#define XDWORD_MAX ((xdword_t)-1)
#define XDWORD_MUL
//#define XDWORD_DIV
#elif __x86_64__
typedef unsigned long xword_t;
#endif

// Definitions for xword_t size
#define XWORD_BITS (sizeof(xword_t) * 8)
#define XWORD_MAX ((xword_t)-1)
#define XWORD_HALF_MASK (XWORD_MAX>>(XWORD_BITS/2))

struct xint_s
{
    int capacity; // Indicates the total size of the xword array
    int size;     // Words currently in use.
    // This stores the data in little-endian format. This will simplify
    // growing and shrinking the array
    xword_t *data;
};
#define XINT_INIT_VAL {{ 0, 0, NULL }};

typedef struct xint_s xint_t[1];

#define XINT_CONST(__a) { sizeof(__a)/sizeof(__a[0]), sizeof(__a)/sizeof(__a[0]), (xword_t *)__a }

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Low level functions
void x_move(xword_t *Y, xword_t *X, size_t sz);
int xll_cmp(const xword_t *U, const xword_t *V, int n);
xword_t xll_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n);
xword_t xll_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
void xll_mul_algm(xword_t *W, const xword_t *U, size_t m, const xword_t *V, size_t n);
void xll_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n);
xword_t xll_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v);
xword_t xll_squ_1(xword_t *W, const xword_t *U, int sz);
xword_t xll_squ_2(xword_t *W, const xword_t *U, int sz);

// Initialisation functions
int xint_init(xint_t u);
int xint_init2(xint_t u, int reserve);
void xint_delete(xint_t u);

// Copy functions
void xint_copy(xint_t u, const xint_t v);
void xint_swap(xint_t u, xint_t v);

// Assignment functions
void xint_assign_ulong(xint_t u, unsigned long val);
void xint_assign_long(xint_t u, long val);
void xint_assign_str(xint_t x, const char *s, int base);

// Utility functions
static inline int xint_is_neg(const xint_t u) { return u->size < 0; }
static inline int xint_is_zero(const xint_t u) { return u->size == 0; }
static inline int xint_is_pos(const xint_t u) { return u->size > 0; }
static inline void xint_chs(xint_t u) { u->size = -u->size; }
static inline void xint_set_pos(xint_t u) { u->size = abs(u->size); }
static inline void xint_set_neg(xint_t u) { u->size = -abs(u->size); }
static inline void xint_abs(xint_t u) { u->size = abs(u->size); }
static inline int xint_size(const xint_t u) { return abs(u->size); }
static inline void xint_assign_zero(xint_t u) { u->size = 0; }

// Compare functions
int xint_cmpa_ulong(const xint_t u, const unsigned long v);
int xint_cmp_ulong(const xint_t u, const unsigned long v);
int xint_cmp_long(const xint_t u, const long v);
int xint_cmpa(const xint_t u, const xint_t v);
int xint_cmp(const xint_t u, const xint_t v);

// Absolute addition and subtraction
int xint_adda_ulong(xint_t w, const xint_t u, const unsigned long val);
int xint_adda(xint_t w, const xint_t u, const xint_t v);
int xint_suba_ulong(xint_t w, const xint_t u, const unsigned long val);
int xint_suba(xint_t w, const xint_t u, const xint_t v);

// Addition and Subtraction functions
void xint_add_ulong(xint_t w, const xint_t u, const unsigned long val);
void xint_add_long(xint_t w, const xint_t u, const long val);
void xint_add(xint_t w, const xint_t u, const xint_t v);
void xint_sub_ulong(xint_t w, const xint_t u, const unsigned long val);
void xint_sub_long(xint_t w, const xint_t u, const long val);
void xint_sub(xint_t w, const xint_t u, const xint_t v);

// Multiplication functions
void xint_sqr(xint_t w, const xint_t u);
void xint_mul(xint_t w, const xint_t u, const xint_t v);
void xint_mul_ulong(xint_t w, const xint_t u, unsigned long v);
void xint_mul_long(xint_t w, const xint_t u, long v);
void xint_muladd_ulong(xint_t w, xint_t u, unsigned long m, unsigned long a);

// Division and modulus functions
void xint_div_trunc(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_floor(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_ceil(xint_t q, xint_t r, const xint_t u, const xint_t v);
xword_t xint_div(xint_t q, xint_t r, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, xword_t v);
xword_t xint_mod(xint_t r, const xint_t u, const xint_t v);
int xint_mod_1(xword_t *r, const xint_t u, xword_t v);

// Bit manipulation
int xint_highest_bit_num(const xint_t x);
xword_t xint_lshift(xint_t y, const xint_t x, int numbits);
xword_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
