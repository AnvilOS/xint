
#ifndef XINT_H
#define XINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#if defined __arm__
#define XWORD_SIZE  (4)
#elif defined __x86_64__
#define XWORD_SIZE  (8)
#endif

#if XWORD_SIZE == __SIZEOF_INT__
typedef unsigned int xword_t;
#elif XWORD_SIZE == __SIZEOF_LONG__
typedef unsigned long xword_t;
#elif XWORD_SIZE == __SIZEOF_LONG_LONG__
typedef unsigned long long xword_t;
#endif

#if XWORD_SIZE <= __SIZEOF_LONG__
#define XWORD_HOLDS_LONG
#endif

#if 2*XWORD_SIZE == __SIZEOF_LONG__
typedef unsigned long xdword_t;
#define XDWORD_MAX ((xdword_t)-1)
#define XDWORD_MUL
//#define XDWORD_DIV
#elif 2*XWORD_SIZE == __SIZEOF_LONG_LONG__
typedef unsigned long long xdword_t;
#define XDWORD_MAX ((xdword_t)-1)
#define XDWORD_MUL
//#define XDWORD_DIV
#endif

// Definitions for xword_t size
#define XWORD_MAX (xword_t)-1
#define XWORD_BITS (XWORD_SIZE * 8)
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

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

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
void xint_div(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_trunc(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_floor(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_ceil(xint_t q, xint_t r, const xint_t u, const xint_t v);
void xint_div_ulong(xint_t q, xword_t *r, const xint_t u, unsigned long v);
void xint_mod(xint_t r, const xint_t u, const xint_t v);
void xint_mod_ulong(xword_t *r, const xint_t u, unsigned long v);

// Bit manipulation
int xint_highest_bit_num(const xint_t x);
xword_t xint_lshift(xint_t y, const xint_t x, int numbits);
xword_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
