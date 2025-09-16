
#ifndef XINT_H
#define XINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

// Definitions for xword_t size
#define XWORD_BITS (sizeof(xword_t) * 8)
#define XWORD_MAX ((xword_t)-1)
#define XWORD_HALF_MASK (XWORD_MAX>>(XWORD_BITS/2))

#if 1
typedef unsigned xword_t;
typedef unsigned long xdword_t;
#define XDWORD_MAX ((xdword_t)-1)
#else
typedef unsigned long xword_t;
#endif


struct xint_s
{
    int capacity; // Indicates the total size of the xword array
    int size;     // Words currently in use.
    // This stores the data in little-endian format. This will simplify
    // growing and shrinking the array
    xword_t *data;
};
#define XINT_INIT_VAL { 0, 0, NULL };

typedef struct xint_s xint_t[1];

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Low level functions
xword_t xll_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
void xll_mul(xword_t *W, xword_t *U, size_t m, xword_t *V, size_t n);
void xll_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n);

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

// Utility functions
static inline int xint_is_neg(const xint_t u) { return u->size < 0; }
static inline int xint_is_zero(const xint_t u) { return u->size == 0; }
static inline int xint_is_pos(const xint_t u) { return u->size > 0; }
static inline void xint_chs(xint_t u) { u->size = -u->size; }
static inline void xint_set_sign(xint_t u, int s) { u->size *= s; }
static inline void xint_set_pos(xint_t u) { u->size = abs(u->size); }
static inline void xint_set_neg(xint_t u) { u->size = -abs(u->size); }
static inline void xint_abs(xint_t u) { u->size = abs(u->size); }
static inline int xint_size(const xint_t u) { return abs(u->size); }
static inline void xint_set_zero(xint_t u) { u->size = 0; }

// Compare arithmetic
int xint_cmp(const xint_t u, const xint_t v);
int xint_cmpa_ulong(const xint_t u, const unsigned long v);
int xint_cmp_ulong(const xint_t u, const unsigned long v);
int xint_cmp_long(const xint_t u, const long v);

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
xword_t xint_sqr(xint_t w, const xint_t u);
xword_t xint_mul(xint_t w, const xint_t u, const xint_t v);
xword_t xint_mul_ulong(xint_t w, const xint_t u, unsigned long v);
xword_t xint_mul_long(xint_t w, const xint_t u, long v);
xword_t xint_muladd_ulong(xint_t w, xint_t u, unsigned long m, unsigned long a);

// Division and modulus functions
void xint_div_trunc(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_floor(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_ceil(xint_t q, xint_t r, const xint_t u, const xint_t v);
xword_t xint_div(xint_t q, xint_t r, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, xword_t v);
xword_t xint_mod(xint_t r, const xint_t u, const xint_t v);
int xint_mod_1(xword_t *r, const xint_t u, xword_t v);

// Bit manipulation
int xint_highest_bit(xint_t x);
xword_t xint_lshift(xint_t y, const xint_t x, int numbits);
xword_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
