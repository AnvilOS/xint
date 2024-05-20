
#ifndef XINT_H
#define XINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define XINT_INIT_VAL { 0, 0, NULL };

static const int bits_per_xword = 32;
typedef uint32_t xword_t;

struct xint_s
{
    int capacity; // Indicates the total size of the xword array
    int size;     // Words currently in use.
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
int xint_init(xint_t u);
int xint_init2(xint_t u, int reserve);
void xint_delete(xint_t u);

// Copy functions
int xint_copy(xint_t u, const xint_t v);
void xint_swap(xint_t u, xint_t v);

// Assignment functions
void xint_assign_uint32(xint_t u, uint32_t val);
void xint_assign_uint64(xint_t u, uint64_t val);
void xint_assign_ulong(xint_t u, unsigned long val);
void xint_assign_long(xint_t u, long val);

// Utility functions
static inline int xint_is_neg(const xint_t u) { return u->size < 0; }
static inline int xint_is_zero(const xint_t u) { return u->size == 0; }
static inline int xint_is_pos(const xint_t u) { return u->size > 0; }
static inline void xint_chs(xint_t u) { u->size = -u->size; }
static inline void xint_set_pos(xint_t u) { u->size = abs(u->size); }
static inline void xint_set_neg(xint_t u) { u->size = -abs(u->size); }
static inline void xint_abs(xint_t u) { u->size = abs(u->size); }
static inline int xint_size(const xint_t u) { return abs(u->size); }
static inline void xint_set_zero(xint_t u) { u->size = 0; }

// Signed arithmetic
int xint_add(xint_t w, const xint_t u, const xint_t v);
int xint_sub(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v);

// Absolute arithmetic
int xint_adda(xint_t w, const xint_t u, const xint_t v);
int xint_adda_1(xint_t w, const xint_t u, xword_t v);
int xint_suba(xint_t w, const xint_t u, const xint_t v);
int xint_suba_1(xint_t w, const xint_t u, xword_t v);
uint32_t xint_sqr(xint_t w, const xint_t u);
uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul_1(xint_t w, const xint_t x, xword_t n);
uint32_t xint_mul_2(xint_t w, const xint_t x, uint64_t n);
uint32_t xint_div(xint_t q, xint_t r, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v);
uint32_t xint_mod(xint_t r, const xint_t u, const xint_t v);
int xint_mod_1(xword_t *r, const xint_t u, uint32_t v);
int xint_add_1(xint_t w, xint_t u, xword_t v);
int xint_cmp_uint32(const xint_t u, uint32_t v);
int xint_cmp(const xint_t u, const xint_t v);
int add(xint_t W, xint_t U, xint_t V);
int sub(xint_t W, xint_t U, xint_t V);
int addsub(xint_t W, xint_t U, xint_t V, int Upos, int Vpos);
uint32_t xint_mul_1_add_1(xint_t w, xint_t u, unsigned m, unsigned a);

// Bit manipulation
int xint_highest_bit(xint_t x);
uint32_t xint_lshift(xint_t y, const xint_t x, int numbits);
uint32_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
