
#ifndef XINT_INTERNAL_H
#define XINT_INTERNAL_H

#include "xint.h"

typedef struct xint_s *xint_ptr;
typedef const struct xint_s *const_xint_ptr;

#define XINT_MIN(__a, __b) ((__a)<(__b)?(__a):(__b))
#define XINT_MAX(__a, __b) ((__a)>(__b)?(__a):(__b))

#define XINT_ABS(__a) ((__a)>=0?(__a):-(__a))

#define XLL_ASSERT(__a) assert(__a)

#define XINT_SWAP(__type, __a, __b) \
do { \
    __type t = (__a); \
    (__a) = (__b); \
    (__b) = t; \
} while (0)

#if defined XDWORD_MUL
#define mul_1_1(__k, __w, __u, __v) \
do { \
    xdword_t __x = (xdword_t)__u * __v; \
    __w = (xword_t)__x; \
    __k = (xword_t)(__x >> (XWORD_BITS)); \
} while (0)
#else
#define mul_1_1(__k, __w, __u, __v) \
do { \
    xword_t __uh = __u >> (XWORD_BITS/2); \
    xword_t __ul = __u & XWORD_HALF_MASK; \
    xword_t __vh = __v >> (XWORD_BITS/2); \
    xword_t __vl = __v & XWORD_HALF_MASK; \
    xword_t __lo = __ul * __vl; \
    xword_t __hi = __uh * __vh; \
    xword_t __mid = __uh * __vl; \
    xword_t __tmp = __ul * __vh; \
    __mid += __lo >> (XWORD_BITS/2); \
    __mid += __tmp; \
    __hi += ((xword_t)(__mid < __tmp)) << (XWORD_BITS/2); \
    __w = (__lo & XWORD_HALF_MASK) | (__mid << (XWORD_BITS/2)); \
    __k = __hi + (__mid >> (XWORD_BITS/2)); \
} while (0)
#endif

#if defined XDWORD_DIV
#define div_2_1(__qh, __ql, __r, __u1, __u0, __v) \
do { \
    xdword_t __u = (xdword_t)__u1 << XWORD_BITS | __u0; \
    xdword_t __full_q = __u / __v; \
    __qh = __full_q >> XWORD_BITS; \
    __ql = (xword_t)__full_q; \
    __r = __u % __v; \
} while (0)
#else
#define div_2_1(__qh, __ql, __r, __u1, __u0, __v) \
do { \
    assert(__u1 <= __v); \
    xword_t __v1 = __v >> (XWORD_BITS/2); \
    xword_t __v0 = __v & XWORD_HALF_MASK; \
    xword_t __u0_1 = __u0 >> (XWORD_BITS/2); \
    xword_t __u0_0 = __u0 & XWORD_HALF_MASK; \
    xword_t __q1 = __u1 / __v1; \
    xword_t __rhat = __u1 % __v1; \
    while (__q1 * __v0 > (__rhat << (XWORD_BITS/2) | __u0_1)) \
    { \
        --__q1; \
        __rhat += __v1; \
        if (__rhat >= XWORD_HALF_MASK + 1) \
        { \
            break; \
        } \
    } \
    xword_t __numer = (__u1 << (XWORD_BITS/2)) + __u0_1 - __q1 * __v; \
    xword_t __q0 = __numer / __v1; \
    __rhat = __numer % __v1; \
    while (__q0 >= XWORD_HALF_MASK + 1 || __q0 * __v0 > (__rhat << (XWORD_BITS/2) | __u0_0)) \
    { \
        --__q0; \
        __rhat += __v1; \
        if (__rhat >= XWORD_HALF_MASK + 1) \
        { \
            break; \
        } \
    } \
    __numer = (__numer << (XWORD_BITS/2)) + __u0_0 - __q0 * __v; \
    __r = __numer; \
    __ql = __q1 << (XWORD_BITS/2) | __q0; \
    __qh = __q1 >> (XWORD_BITS/2); \
} while (0)
#endif

#define FAST_RESIZE(__x, __s)       \
do                                  \
{                                   \
    if ((__s) <= (__x)->capacity)   \
    {                               \
        (__x)->size = __s;          \
    }                               \
    else                            \
    {                               \
        _fast_resize((__x), (__s)); \
    }                               \
} while (0);

void _fast_resize(xint_t x, int new_size);

// Low level functions
static inline void xll_zero(xword_t *Y, size_t sz)
{
    for (int i=0; i< sz; ++i)
    {
        Y[i] = 0;
    }
}

static inline void xll_move(xword_t *Y, xword_t *X, size_t sz)
{
    for (int i=0; i< sz; ++i)
    {
        Y[i] = X[i];
    }
}

static inline int get_highest_word(const xint_t x)
{
    int Xn = XINT_ABS(x->size);
    for (int j=Xn-1; j>=0; --j)
    {
        if (x->data[j])
        {
            return j;
        }
    }
    return -1;
}

static inline int get_highest_bit(const xword_t word)
{
    if (word)
    {
        if (sizeof(xword_t) == 8)
            return (XWORD_BITS - 1) - __builtin_clzl(word);
        else
            return (XWORD_BITS - 1) - __builtin_clz(word);
    }
    return -1;
}

static int resize(xint_t x, int new_size)
{
    if (new_size > x->capacity)
    {
        void *new_mem = realloc(x->data, sizeof(xword_t) * new_size);
        if (new_mem == NULL)
        {
            return -1;
        }
        x->capacity = new_size;
        x->data = new_mem;
    }
    x->size = x->size < 0 ? -new_size : new_size;
    return 0;
}

static void trim_zeroes(xint_t u)
{
    int Un = XINT_ABS(u->size);
    for (int j=Un-1; j>=0; --j)
    {
        if (u->data[j] != 0)
        {
            resize(u, j + 1);
            return;
        }
    }
    u->size = 0;
}

static int xll_cmp(const xword_t *U, const xword_t *V, int n)
{
    for (int j=n-1; j>=0; --j)
    {
        if (U[j] != V[j])
        {
            return U[j] < V[j] ? -1 : 1;
        }
    }
    return 0;
}

//int xll_cmp(const xword_t *U, const xword_t *V, int n);
xword_t xll_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n);
xword_t xll_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
void xll_mul_algm(xword_t *W, const xword_t *U, size_t m, const xword_t *V, size_t n);
void xll_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n);
xword_t xll_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v);
xword_t xll_mul_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0);
void xll_squ_2(xword_t *W, const xword_t *U, int sz);
xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int m);
xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n);
xword_t xll_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v);
xword_t xll_mul_add_4(xword_t *W, const xword_t *U, size_t m, const xword_t *V);
xword_t x_lshift(xword_t *Y, const xword_t *X, int sz, int shift_bits);
xword_t x_rshift(xword_t *Y, const xword_t *X, int sz, int shift_bits);


#endif // XINT_INTERNAL_H
