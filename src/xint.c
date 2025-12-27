
#include "xint.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

static void trim_zeroes(xint_t u);
static int get_highest_word(const xint_t x);
static int get_highest_bit(const xword_t word);
static int resize(xint_t x, int new_size);
static int add_or_sub(xint_t w, const xint_t u, const xint_t v, int Upos, int Vpos);
static int add_or_sub_long(xint_t w, const xint_t u, unsigned long v, int Upos, int Vpos);

static void x_zero(xword_t *Y, size_t sz);
static void x_move(xword_t *Y, xword_t *X, size_t sz);
static xword_t x_lshift(xword_t *Y, const xword_t *X, int sz, int shift_bits);
static xword_t x_rshift(xword_t *Y, const xword_t *X, int sz, int shift_bits);
xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n);
xword_t xll_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v, xword_t k);
static xword_t xll_mul_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0);
static xword_t xll_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v);
static xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int m);
xword_t xll_squ_1(xword_t *W, const xword_t *U, int sz);
xword_t xll_squ_2(xword_t *W, const xword_t *U, int sz);

#define XINT_SWAP(__type, __a, __b) \
do {                                \
    __type t = (__a);               \
    (__a) = (__b);                  \
    (__b) = t;                      \
} while (0)

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

// Initialisation functions
int xint_init(xint_t u)
{
    u->capacity = 0;
    u->size = 0;
    u->data = NULL;
    return 0;
}

int xint_init2(xint_t u, int reserve)
{
    u->capacity = 0;
    u->size = 0;
    u->data = NULL;
    if (reserve)
    {
        if ((u->data = malloc(sizeof(xword_t) * reserve)) == NULL)
        {
            return -1;
        }
        u->capacity = reserve;
    }
    return 0;
}

void xint_delete(xint_t u)
{
    if (u->data)
    {
        free(u->data);
        u->data = NULL;
    }
    u->capacity = 0;
    u->size = 0;
}

// Copy functions
void xint_copy(xint_t u, const xint_t v)
{
    // Copy v to u
    if (u == v)
    {
        return;
    }
    int Vn = abs(v->size);
    FAST_RESIZE(u, Vn);
    x_move(u->data, v->data, Vn);
    u->size = v->size;
}

void xint_swap(xint_t u, xint_t v)
{
    // Swap the internals of u and v
    xint_t T;
    *T = *u;
    *u = *v;
    *v = *T;
}

// Assignment functions
void xint_assign_ulong(xint_t u, unsigned long v)
{
    u->size = 0;
    while (v)
    {
        FAST_RESIZE(u, u->size + 1);
        u->data[u->size - 1] = (xword_t)v;
        if (sizeof(v) <= sizeof(xword_t))
        {
            break;
        }
        v >>= XWORD_BITS;
    }
}

void xint_assign_long(xint_t u, long v)
{
    xint_assign_ulong(u, labs(v));
    if (v < 0)
    {
        xint_chs(u);
    }
}

void xint_assign_str(xint_t x, const char *s, int base)
{
    int neg = 0;
    if (s && (*s == '-' || *s == '+'))
    {
        if (*s == '-')
        {
            neg = 1;
        }
        ++s;
    }
        
    if (base == 0)
    {
        base = 10;
        if (*s == '0')
        {
            base = 8;
            ++s;
            if (tolower(*s) == 'x')
            {
                base = 16;
                ++s;
            }
        }
    }
    else if (base < 1 || base > 36)
    {
        return;
    }
    
    x->size = 0;
    while (*s)
    {
        int val;
        if (*s == ' ')
        {
            ++s;
            continue;
        }
        else if (*s >= '0' && *s <= '9')
        {
            val = *s - '0';
        }
        else
        {
            val = tolower(*s) - 'a' + 10;
        }
        xint_muladd_ulong(x, x, base, val);
        ++s;
    }
    if (neg)
    {
        x->size = -x->size;
    }
}

// Compare functions
int xint_cmpa_ulong(const xint_t u, const unsigned long v)
{
    int Un = abs(u->size);
    
    if (Un == 0)
    {
        return v == 0 ? 0 : -1;
    }
    unsigned long uu = 0;
    if (sizeof(v) <= sizeof(xword_t))
    {
        if (Un > 1)
        {
            return 1;
        }
    }
    else
    {
        if (Un > 2)
        {
            return 1;
        }
        if (Un == 2)
        {
            uu = u->data[1];
            uu <<= XWORD_BITS;
        }
    }
    uu |= u->data[0];
    if (uu == v)
    {
        return 0;
    }
    return uu < v ? -1 : 1;
}

int xint_cmp_ulong(const xint_t u, const unsigned long val)
{
    if (u->size < 0)
    {
        return -1;
    }
    // Both are positive
    return xint_cmpa_ulong(u, val);
}

int xint_cmp_long(const xint_t u, const long val)
{
    if (val >= 0)
    {
        return xint_cmp_ulong(u, val);
    }
    // else val is neg
    if (u->size >= 0)
    {
        // clearly u is bigger
        return 1;
    }
    // Both are negative
    return -xint_cmpa_ulong(u, -val);
}

int xint_cmpa(const xint_t u, const xint_t v)
{
    int un = abs(u->size);
    int vn = abs(v->size);
    if (un != vn)
    {
        return un < vn ? -1 : 1;
    }
    return xll_cmp(u->data, v->data, un);
}

int xint_cmp(const xint_t u, const xint_t v)
{
    int un = u->size;
    int vn = v->size;
    if (un != vn)
    {
        return un < vn ? -1 : 1;
    }
    return un > 0 ? xll_cmp(u->data, v->data, abs(un)) : xll_cmp(v->data, u->data, abs(un));
}

// Addition and Subtraction functions
void xint_add_ulong(xint_t w, const xint_t u, const unsigned long val)
{
    add_or_sub_long(w, u, val, u->size >= 0, 1);
}

void xint_add_long(xint_t w, const xint_t u, const long val)
{
    add_or_sub_long(w, u, labs(val), u->size >= 0, val >= 0);
}

void xint_add(xint_t w, const xint_t u, const xint_t v)
{
    add_or_sub(w, u, v, !xint_is_neg(u), !xint_is_neg(v));
}

void xint_sub_ulong(xint_t w, const xint_t u, const unsigned long val)
{
    add_or_sub_long(w, u, val, u->size >= 0, 0);
}

void xint_sub_long(xint_t w, const xint_t u, const long val)
{
    add_or_sub_long(w, u, labs(val), u->size >= 0, val < 0);
}

void xint_sub(xint_t w, const xint_t u, const xint_t v)
{
    add_or_sub(w, u, v, !xint_is_neg(u), xint_is_neg(v));
}

static int add_or_sub_long(xint_t w, const xint_t u, unsigned long v, int upos, int vpos)
{
    if (v > XWORD_MAX)
    {
        xword_t vvv[4];
        xint_t vv = {{ 4, 0, vvv }};
        xint_assign_ulong(vv, v);
        add_or_sub(w, u, vv, upos, vpos);
        return 0;
    }
    if (upos == vpos)
    {
        xint_adda_ulong(w, u, v);
        upos ? xint_set_pos(w) : xint_set_neg(w);
    }
    else if (xint_suba_ulong(w, u, v) < 0)
    {
        upos ? xint_set_neg(w) : xint_set_pos(w);
    }
    else
    {
        upos ? xint_set_pos(w) : xint_set_neg(w);
    }
    return -1;
}

static int add_or_sub(xint_t w, const xint_t u, const xint_t v, int upos, int vpos)
{
    if (upos == vpos)
    {
        xint_adda(w, u, v);
        upos ? xint_set_pos(w) : xint_set_neg(w);
    }
    else if (xint_suba(w, u, v) < 0)
    {
        upos ? xint_set_neg(w) : xint_set_pos(w);
    }
    else
    {
        upos ? xint_set_pos(w) : xint_set_neg(w);
    }
    return -1;
}

int xint_adda(xint_t w, const xint_t u, const xint_t v)
{
    int Un = abs(u->size);
    int Vn = abs(v->size);
    int Wn = MAX(Un, Vn);

    resize(w, Wn);
    int part1_len = MIN(Un, Vn);
    int part2_len = abs(Un - Vn);
    xword_t *bigger = Un>Vn ? u->data : v->data;
    xword_t k = xll_add(w->data, u->data, v->data, part1_len);
    k = xll_add_1(w->data+part1_len, bigger+part1_len, k, part2_len);
    if (k)
    {
        resize(w, Wn + 1);
        w->data[Wn] = k;
    }
    return 0;
}

int xint_adda_ulong(xint_t w, const xint_t u, const unsigned long v)
{
    if (v <= XWORD_MAX)
    {
        // fits in a single xword
        int Un = abs(u->size);
        // This is the only failure point
        if (resize(w, Un) == -1)
        {
            return -1;
        }
        xword_t k = xll_add_1(w->data, u->data, (xword_t)v, Un);
        if (k)
        {
            resize(w, Un + 1);
            w->data[Un] = k;
        }
        return 0;
    }
    xword_t vvv[4];
    xint_t vv = { 4, 0, vvv };
    xint_assign_ulong(vv, v);
    return xint_adda(w, u, vv);
}

int xint_suba(xint_t w, const xint_t u, const xint_t v)
{
    // XXX: what about if u or v is 0
    int Un = abs(u->size);
    int Vn = abs(v->size);
    xword_t b = 0;

    int cmp = xint_cmpa(u, v);
    switch (cmp)
    {
        case 0: // U == V
            w->size = 0;
            return 0;
            
        case -1: // U < V
            resize(w, Vn);
            b = xll_sub(w->data, v->data, u->data, Un);
            b = xll_sub_1(w->data+Un, v->data+Un, b, Vn-Un);
            break;

        case 1: // U > V
            resize(w, Un);
            b = xll_sub(w->data, u->data, v->data, Vn);
            b = xll_sub_1(w->data+Vn, u->data+Vn, b, Un-Vn);
            break;
    }
    assert(b == 0);
    trim_zeroes(w);
    return cmp;
}

int xint_suba_ulong(xint_t w, const xint_t u, const unsigned long v)
{
    if (v <= XWORD_MAX)
    {
        // fits in a single xword
        int Un = abs(u->size);
        xword_t b = 0;

        xword_t vv = (xword_t)v;
        int cmp = xint_cmpa_ulong(u, v);
        switch (cmp)
        {
            case 0: // U == V
                w->size = 0;
                return 0;
                
            case -1: // U < V
                if (u->size == 0)
                {
                    xint_assign_ulong(w, v);
                }
                else
                {
                    resize(w, 1);
                    b = xll_sub_1(w->data, &vv, u->data[0], Un);
                }
                break;

            case 1: // U > V
                resize(w, Un);
                b = xll_sub_1(w->data, u->data, vv, Un);
                break;
        }
        assert(b == 0);
        trim_zeroes(w);
        return 0;
    }
    xword_t vvv[4];
    xint_t vv = {{ 4, 0, vvv }};
    xint_assign_ulong(vv, v);
    return xint_suba(w, u, vv);
}

// Multiplication functions
void xint_sqr(xint_t w, const xint_t u)
{
    if (u->size == 0)
    {
        w->size = 0;
        return;
    }
    int Un = abs(u->size);
    resize(w, 2 * Un);
    
    xword_t *U = u->data;
    xword_t *W = w->data;

    if (w == u)
    {
        // Move V to the top of W - in reverse because there may be overlap
        x_move(w->data+Un, u->data, Un);
        U = W + Un;
    }

    // Clear the bottom words
    x_zero(w->data, Un);
    xword_t k = xll_squ_1(W, U, Un);
    assert(k == 0);

    trim_zeroes(w);
    return;
}

void xint_mul(xint_t w, const xint_t u, const xint_t v)
{
    int Un = abs(u->size);
    int Vn = abs(v->size);
    int Wneg = (u->size * v->size) < 0;
    
    if (Un < Vn)
    {
        XINT_SWAP(const struct xint_s *, u, v);
        XINT_SWAP(int, Un, Vn);
    }

    if (Vn <= 2)
    {
        if (Vn == 0)
        {
            w->size = 0;
            return;
        }
        else if (Vn == 1)
        {
            FAST_RESIZE(w, Un + 1);
            xword_t k = xll_mul_1(w->data, u->data, Un, v->data[0], 0);
            if (k)
            {
                w->data[Un] = k;
            }
            else
            {
                --w->size;
            }
            if (Wneg)
            {
                xint_set_neg(w);
            }
            return;
        }
        else
        {
            FAST_RESIZE(w, Un + 2);
            xword_t k = xll_mul_2(w->data, u->data, Un + 1, v->data[1], v->data[0]);
            if (k)
            {
                w->data[Un+1] = k;
            }
            else
            {
                --w->size;
            }
            if (Wneg)
            {
                xint_set_neg(w);
            }
            return;
        }
    }
    
    if (u == v)
    {
        return xint_sqr(w, u);
    }

    FAST_RESIZE(w, Un + Vn);

    xword_t *U = u->data;
    xword_t *V = v->data;
    xword_t *W = w->data;
    
    // As pointed out by Knuth, algorithm M can cope with V[j] being in the same
    // location as W[j+n]. In other words, V can be copied to the upper words
    // of W as long as there is zeroed space at the bottom of W to hold Un words
    if (w == v)
    {
        // Move V to the top of W - in reverse because there may be overlap
        x_move(W+Un, V, Vn);
        V = W + Un;
        xll_mul(W, U, Un, V, Vn);
    }
    else if (w == u)
    {
        // Move U to the top of W - in reverse because there may be overlap
        x_move(W+Vn, U, Un);
        // Adjust the V pointer
        U = W + Vn;
        xll_mul(W, V, Vn, U, Un);
    }
    else
    {
        xll_mul(W, U, Un, V, Vn);
    }
    trim_zeroes(w);
    if (Wneg)
    {
        xint_set_neg(w);
    }
}

void xint_mul_ulong(xint_t w, const xint_t u, unsigned long v)
{
    if (u->size == 0 || v == 0)
    {
        w->size = 0;
        return;
    }

    int Uneg = xint_is_neg(u);
    int Un = abs(u->size);
    if (v <= XWORD_MAX)
    {
        FAST_RESIZE(w, Un + 1);
        xword_t k = xll_mul_1(w->data, u->data, Un, (xword_t)v, 0);
        if (k)
        {
            w->data[Un] = k;
        }
        else
        {
            --w->size;
        }
    }
#if defined XDWORD_MAX
    else if (v <= XDWORD_MAX)
    {
        FAST_RESIZE(w, Un + 2);
        xword_t k = xll_mul_2(w->data, u->data, Un + 1, v>>(XWORD_BITS), (xword_t)v);
        if (k)
        {
            w->data[Un+1] = k;
        }
        else
        {
            --w->size;
        }
    }
#endif
    if (Uneg)
    {
        xint_set_neg(w);
    }
}

void xint_mul_long(xint_t w, const xint_t u, long v)
{
    xint_mul_ulong(w, u, labs(v));
    if (v < 0)
    {
        xint_chs(w);
    }
}

void xint_muladd_ulong(xint_t w, xint_t u, unsigned long m, unsigned long a)
{
    if (m <= XWORD_MAX && a <= XWORD_MAX)
    {
        int Un = abs(u->size);
        FAST_RESIZE(w, Un+1);
        xword_t k = xll_mul_1(w->data, u->data, Un, (xword_t)m, (xword_t)a);
        if (k)
        {
            w->data[Un] = k;
        }
        else
        {
            --w->size;
        }
    }
}

// Division functions
int xint_highest_bit_num(const xint_t x)
{
    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(x);
    int highest_bit = get_highest_bit(x->data[highest_word]);
    return highest_word * XWORD_BITS + highest_bit;
}

void xint_div_trunc(xint_t q, xint_t r, const xint_t u, const xint_t v)
{
    int Us = u->size >= 0;
    int Vs = v->size >= 0;
    xint_div(q, r, u, v);
    if (Us != Vs)
    {
        xint_set_neg(q);
    }
    int Qs = q->size >= 0;
    if (Qs != Vs)
    {
        xint_set_neg(r);
    }
}

void xint_div_floor(xint_t q, xint_t r, const xint_t u, const xint_t v)
{
    int Vs = v->size >= 0;
    xint_div_trunc(q, r, u, v);
    int Rs = r->size >= 0;
    if (Rs != Vs)
    {
        // floor =  u/v - (u%v!=0 && (u^v)<0);
        xint_sub_ulong(q, q, 1);
        xint_add(r, r, v);
    }
}

void xint_div_ceil(xint_t q, xint_t r, const xint_t u, const xint_t v)
{
    int Vs = v->size >= 0;
    xint_div_trunc(q, r, u, v);
    int Rs = r->size >= 0;
    if (Rs != Vs)
    {
        //ceil u/v + (u%v!=0 && (u^v)>0);
        xint_add_ulong(q, q, 1);
        xint_sub(r, r, v);
    }
}

xword_t xint_mod(xint_t r, const xint_t u, const xint_t v)
{
    xint_t q = XINT_INIT_VAL;
    xint_is_pos(v) ? xint_div_floor(q, r, u, v) : xint_div_ceil(q, r, u, v);
    xint_delete(q);
    return 0;
}

xword_t xint_div(xint_t q, xint_t r, const xint_t u, const xint_t v)
{
    // As per Knuth's algorithm D
    // u[0] to u[m+n-1]
    // v[0] to v[n-1]
    // q[0] to q[m]
    // r[0] to r[n-1]
    
    // Algortihm D doesn't work for vn <= 1
    int Un = abs(u->size);
    int Vn = abs(v->size);
    if (Vn <= 1)
    {
        // Use the algorithm from exercise 16
        xword_t rem;
        xint_div_1(q, &rem, u, v->data[0]);
        if (rem)
        {
            resize(r, 1);
            r->data[0] = rem;
        }
        else
        {
            r->size = 0;
        }
        return 0;
    }

    int cmp = xint_cmpa(u, v);
    if (cmp <= 0)
    {
        if (cmp == 0)
        {
            if (q)
            {
                resize(q, 1);
                q->data[0] = 1;
            }
            r->size = 0;
            return 0;
        }
        if (q)
        {
            q->size = 0;
        }
        xint_copy(r, u);
        return 0;
    }

    int n = Vn;
    int m = Un - Vn;

    xint_copy(r, u); // r may sometimes grow

    // Increase the size of R by one
    resize(r, m + n + 1);
    r->data[m + n] = 0;

    if (q)
    {
        resize(q, m + 1);
        assert(abs(q->size) == m + 1);
    }

    assert(abs(r->size) == m + n + 1);
    assert(abs(v->size) == n);
    
    xll_div(q?q->data:r->data + n, r->data, v->data, m, n);

    resize(r, n);

    if (q)
    {
        trim_zeroes(q);
    }
    trim_zeroes(r);

    return 0;
}

int xint_div_1(xint_t q, xword_t *r, const xint_t u, xword_t v)
{
    // This is from Knuth's recommended exercise 16
    if (v == 0)
    {
        return -1;
    }
    int Un = abs(u->size);
    if (q)
    {
        resize(q, Un);
    }
    *r = x_div_1(q?q->data:0, u->data, v, Un);
    if (q)
    {
        trim_zeroes(q);
    }
    return 0;
}

int xint_mod_1(xword_t *r, const xint_t u, xword_t v)
{
    // This is from Knuth's recommended exercise 16
    if (v == 0)
    {
        return -1;
    }
    int Un = xint_size(u);
    *r = x_div_1(0, u->data, v, Un);
    return 0;
}

// Bitwise functions
xword_t xint_lshift(xint_t y, const xint_t x, int numbits)
{
    // Calculate the shift
    int shift_words = numbits / XWORD_BITS;
    int shift_bits = numbits % XWORD_BITS;

    int Xn = abs(x->size);

    if (Xn == 0)
    {
        y->size = 0;
        return 0;
    }

    resize(y, Xn + shift_words + (shift_bits?1:0));
    xword_t *X = x->data;
    int Yn = abs(y->size);
    xword_t *Y = y->data;
    
    if (shift_bits == 0)
    {
        x_move(Y + shift_words, X, Xn);
    }
    else
    {
        Y[Yn - 1] = x_lshift(Y + shift_words, X, Xn, shift_bits);
    }
    x_zero(Y, shift_words);
    trim_zeroes(y);
    return 0;
}

xword_t xint_rshift(xint_t y, const xint_t x, int numbits)
{
    // Calculate the shift
    int shift_words = numbits / XWORD_BITS;
    int shift_bits = numbits % XWORD_BITS;

    int Xn = abs(x->size);

    // If all of x will be shifted out ...
    if (Xn <= shift_words)
    {
        // set y to 0
        y->size = 0;
        return 0;
    }

    resize(y, Xn - shift_words);

    xword_t *X = x->data;
    xword_t *Y = y->data;

    if (shift_bits == 0)
    {
        x_move(Y, X + shift_words, Xn - shift_words);
    }
    else
    {
        x_rshift(Y, X + shift_words, Xn - shift_words, shift_bits);
    }
    trim_zeroes(y);

    return 0;
}

static void trim_zeroes(xint_t u)
{
    int Un = abs(u->size);
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

static int get_highest_word(const xint_t x)
{
    int Xn = abs(x->size);
    for (int j=Xn-1; j>=0; --j)
    {
        if (x->data[j])
        {
            return j;
        }
    }
    return -1;
}

static int get_highest_bit(const xword_t word)
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

void _fast_resize(xint_t x, int new_size)
{
    if (new_size > x->capacity)
    {
        void *new_mem = realloc(x->data, sizeof(xword_t) * new_size);
        if (new_mem == NULL)
        {
            return;
        }
        x->capacity = new_size;
        x->data = new_mem;
    }
    x->size = new_size;
}

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

static void x_zero(xword_t *Y, size_t sz)
{
    memset(Y, 0, sz * sizeof(xword_t));
}

static void x_move(xword_t *Y, xword_t *X, size_t sz)
{
    memmove(Y, X, sz * sizeof(xword_t));
}

static xword_t x_lshift(xword_t *Y, const xword_t *X, int sz, int shift_bits)
{
    xword_t ret = X[sz - 1] >> (XWORD_BITS - shift_bits);
    for (int j=sz-1; j>=1; --j)
    {
        Y[j] = (X[j] << shift_bits) | (X[j - 1] >> (XWORD_BITS - shift_bits));
    }
    Y[0] = X[0] << shift_bits;
    return ret;
}

static xword_t x_rshift(xword_t *Y, const xword_t *X, int sz, int shift_bits)
{
    for (int j=0; j<sz - 1; ++j)
    {
        Y[j] = (X[j] >> shift_bits) | (X[j + 1] << (XWORD_BITS - shift_bits));
    }
    Y[sz - 1] = X[sz - 1] >> shift_bits;
    return 0;
}

int xll_cmp(const xword_t *U, const xword_t *V, int n)
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

xword_t xll_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
{
    // W[] = U[] + V[]
    // This function will work if any or all of the xints are
    // in the same place in memory. e.g. a = a + a will work
    // A1. [Initialise.]
    xword_t k = 0;
    for (size_t j=0; j<n; ++j)
    {
        // A2. [Add digits.] Set Wj to (Uj + Vj + k) % b
        //                   and  k to (Uj + Vj + k) / b
        // We check for overflow by noting the sum to be less
        // than an addend
        xword_t sum = U[j] + k;
        k = sum < k;
        sum += V[j];
        k += sum < V[j];
        W[j] = sum;
        // A3. [Loop on j]
    }
    // Return Wn
    return k;
}

xword_t xll_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n)
{
    // W[] = U[] + v
    xword_t k = v;
    for (size_t j=0; j<n; ++j)
    {
        xword_t sum = U[j] + k;
        k = sum < k ? 1 : 0;
        W[j] = sum;
    }
    return k;
}

xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
{
    // This function will work if any or all of the xints are
    // in the same place in memory. e.g. a = a - a will work
    // S1. [Initialise.]
    xword_t b = 0;
    for (size_t i=0; i<n; ++i)
    {
        // S2. [Sub digits.] Set Wj to (Uj - Vj + k) % b
        //                   and  k to (Uj - Vj + k) / b
        // Unlike Knuth we record positive borrow instead of
        // negative carry
        // We check for underflow by comparing the minuend and subtrahend
        xword_t tmp1 = U[i] - b;
        b = b > U[i] ? 1 : 0;
        b += V[i] > tmp1 ? 1 : 0;
        W[i] = tmp1 - V[i];
        // S3. [Loop on j]
    }
    return b;
}

xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n)
{
    xword_t b = v;
    for (size_t i=0; i<n; ++i)
    {
        xword_t tmp1 = U[i] - b;
        b = b > U[i] ? 1 : 0;
        W[i] = tmp1;
    }
    return b;
}

static xword_t xll_mul_add_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0)
{
    xword_t k0 = 0;
    xword_t k1 = 0;
    for (int j=0; j<m-1; ++j)
    {
        xword_t p1, p0;
        mul_1_1(p1, p0, U[j], v0);
        p0 += k0;
        p1 += p0 < k0;

        p0 += W[j];
        p1 += p0 < W[j];

        xword_t t1, t0;
        mul_1_1(t1, t0, U[j], v1);
        t0 += k1;
        t1 += t0 < k1;
        t0 += p1;
        t1 += t0 < p1;
        
        k0 = t0;
        k1 = t1;
        W[j] = p0;
    }
    //assert(k0 || k1);
    W[m-1] = k0;
    return k1;
}

void xll_mul(xword_t *W, const xword_t *U, size_t m, const xword_t *V, size_t n)
{
    // Based on Knuth's algorithm M.
    // As pointed out by Knuth, algorithm M can cope with V[j] being co-located
    // with W[j+n]. In other words, V and W can be co-located as long as V is
    // copied to the upper words of W prior to running the algorithm. The lower
    // m xwords of W will be zeroed

    // M1. [Initialisation.] - clear the bottom part of W
    W[m] = xll_mul_1(W, U, m, V[0], 0);
    for (size_t j=1; j<n; ++j)
    {
        // M2. [We skip the optional check for zero multiplier]
        if (n - j > 2)
        {
            W[j + m +1] = xll_mul_add_2(W+j, U, m+1, V[j+1], V[j]);
            ++j;
        }
        else
        {
            W[j + m] = xll_mul_add_1(W+j, U, m, V[j]);
        }
        // M6. [Loop on j]
    }
}

xword_t xll_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v)
{
    // W[] += U[] * v
    // M3. [Initialise i (and k)]
    xword_t k = 0;
    for (size_t i=0; i<m; ++i)
    {
        // M4. [Multiply and add]
        xword_t hi, lo;
        mul_1_1(hi, lo, U[i], v);
        lo += k;
        hi += lo < k;
        W[i] += lo;
        hi += W[i] < lo;
        k = hi;
        // M5. [Loop on i]
    }
    return k;
}

xword_t x_mul_sub_1(xword_t *W, const xword_t *U, size_t n, xword_t v)
{
    // W[] -= U[] * v
    xword_t b = 0;
    for (int i=0; i<n; ++i)
    {
        // W[] = W[] - U[] * v
        xword_t hi, lo;
        mul_1_1(hi, lo, U[i], v);
        xword_t tmp = W[i] - b;
        b = tmp > W[i];
        W[i] = tmp - lo;
        b += W[i] > tmp;
        b += hi;
    }
    return b;
}

xword_t xll_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v, xword_t k)
{
    // W[] = U[] * v + k
    // Cut down version of alg M with a single xword for V
    // We assume that U is m long and W is m+1 long.
    // i.e. W is m+n long as per full alg M
    for (size_t j=0; j<m; ++j)
    {
        // M4. [Multiply and add]
        xword_t hi;
        mul_1_1(hi, W[j], U[j], v);
        W[j] += k;
        k = (W[j] < k) + hi;
        // M5. [Loop on i]
    }
    return k;
}

static xword_t xll_mul_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0)
{
    xword_t k0 = 0;
    xword_t k1 = 0;
    for (int j=0; j<m-1; ++j)
    {
        xword_t p1, p0;
        mul_1_1(p1, p0, U[j], v0);
        p0 += k0;
        p1 += p0 < k0;
        xword_t t1, t0;
        mul_1_1(t1, t0, U[j], v1);
        t0 += k1;
        t1 += t0 < k1;
        t0 += p1;
        t1 += t0 < p1;
        k0 = t0;
        k1 = t1;
        W[j] = p0;
    }
    assert(k0 || k1);
    W[m-1] = k0;
    return k1;
}

void xll_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n)
{
    // As per Knuth's algorithm D
    // Assumptions
    // U and R are co-located. This should be done by the caller during
    // normalisation or explicitly as a copy.
    // When the algorithn completes only the lower n-1 words will be valid
    // The caller will need to de-normalise R and V
    // Q[0] to Q[m]
    // R[0] to R[m+n-1]
    // V[0] to V[n-1]

    // D1. [Normalise.]
    // Use Knuth's suggestion of a power of 2 for d. For v1 to be > b/2
    // we need v1 to have its top bit set
    // Instead of shifting both U and V to the left we follow Knuth's suggestion
    // in problem 37 in 4.3.1
    int bit_shift = (XWORD_BITS-1) - get_highest_bit(V[n-1]);
    xword_t V_nm1 = V[n-1];
    xword_t V_nm2 = V[n-2];
    if (bit_shift)
    {
        // Normalise V
        V_nm1 = V_nm1 << bit_shift | V_nm2 >> (XWORD_BITS - bit_shift);
        V_nm2 = V_nm2 << bit_shift;
        if (n > 2)
        {
            V_nm2 |= V[n-3] >> (XWORD_BITS - bit_shift);
        }
    }

    // D2. [Initialise j.]
    for (int j=m; j>=0; --j)
    {
        // D3. [Calculate q^]
        xword_t n1 = R[j+n];
        xword_t n0 = R[j+n-1];
        xword_t R_jnm2 = R[j+n-2];;
        if (bit_shift)
        {
            // This is where we normalise U (or R)
            n1 = n1 << bit_shift | n0 >> (XWORD_BITS - bit_shift);
            n0 = n0 << bit_shift | R[j+n-2] >> (XWORD_BITS - bit_shift);
            R_jnm2 <<= bit_shift;
            if (j+n-3 >= 0)
            {
                R_jnm2 |= R[j+n-3] >> (XWORD_BITS - bit_shift);
            }
        }
        xword_t qhat_hi, qhat;
        xword_t rhat;
        div_2_1(qhat_hi, qhat, rhat, n1, n0, V_nm1);

        xword_t p1, p0;
        mul_1_1(p1, p0, qhat, V_nm2);
        while (qhat_hi || (p1 > rhat) || ((p1 == rhat) && (p0 > R_jnm2)) )
        {
            --qhat;
            if (qhat == XWORD_MAX)
            {
                --qhat_hi;
            }
            rhat += V_nm1;
            if (rhat < V_nm1)
            {
                break;
            }
            mul_1_1(p1, p0, qhat, V_nm2);
        }

        // D4. [Multiply and subtract.]
        // Replace u(j+n to j) by u(j+n to j) - qhat * v(n-1 to 0)
        // Since r is u we have r = r - qhat * v
        xword_t b = x_mul_sub_1(R+j, V, n, (xword_t)qhat);
        b = b - R[j+n];
        assert(b == 0 || b == 1);

        Q[j] = (xword_t)qhat;

        // D5. Test remainder
        if (b)
        {
            // D6. [Add back.]
            // Decrease Qj by one
            --Q[j];
            // Add V(Vn-1 to 0) to U(Vn+j to j)
            xword_t k = xll_add(R+j, R+j, V, n);
            assert(b - k == 0);
        }
        // D7. Loop on j
    }
    // D8. Un-normalise
    // Not needed
}

static xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int n)
{
    // Renamed Knuth's W to Q
    // S1. [Set r = 0, j = n - 1]
    xword_t R = 0;
    int bit_shift = (XWORD_BITS-1) - get_highest_bit(V);
    if (bit_shift)
    {
        V <<= bit_shift;
    }
    for (int j=n-1; j>=0; --j)
    {
        // S2. [Set wj = (r * B + uj) / v , r = (r * B + uj) % v
        xword_t qh, ql;
        xword_t uu = U[j];
        if (bit_shift)
        {
            R |= uu >> (XWORD_BITS - bit_shift);
            uu <<= bit_shift;
            if (j>1)
            {
                uu |= U[j-1] >> (XWORD_BITS - bit_shift);
            }
        }
        div_2_1(qh, ql, R, R, uu, V);
        if (Q)
        {
            Q[j] = ql;
        }
        // S3. [Decrease j by 1, and return to S2]
    }
    return R >> bit_shift;
}

xword_t xll_squ_1(xword_t *WW, const xword_t *UU, int Un)
{
    xword_t kk = 0;
    for (int j=0; j<Un; ++j)
    {
        xword_t *W = WW+j+j;
        const xword_t *U = UU+j;
        xword_t k1 = 0;
        xword_t k0 = 0;
 
        xword_t tmp;
        // k, W[0] = W[0] + U[0] * U[0]
        mul_1_1(k0, tmp, UU[j], UU[j]);
        WW[2*j] += tmp;
        k0 += WW[2*j] < tmp;
        
        for (int i=1; i<Un-j; ++i)
        {
            xword_t t2, t1, t0;
            // t2, t1, t0 = U[i] * U[0]
            mul_1_1(t1, t0, U[i], U[0]);
            
            // t2, t1, t0 = 2 * [ t2, t1, t0 ]
            t2 = t1 >> (XWORD_BITS-1);
            t1 <<= 1;
            t1 |= t0 >> (XWORD_BITS-1);
            t0 <<= 1;
            
            // t2, t1, t0 += k1, k0
            // Max t0 is almost ffffffff
            // Max k0 is almost ffffffff
            // Max t1 is fffffffd at b504f333
            // Max k1 is 1
            // Therefore t0 += k0 can overflow into t1
            // but t1 += k1 + (t0 < k0) can never overflow into t2
            t0 += k0;
            t1 += k1 + (t0 < k0);
            
            // t2, t1, t0 += W[i]
            t0 += W[i];
            t1 += t0 < W[i];
            t2 += t1 < (t0 < W[i]);
            
            W[i] = t0;
            k0 = t1;
            k1 = t2;
        }
        W[Un-j] = k0 + kk;
        kk = k1;
    }
    return 0;
}

xword_t xll_squ_2(xword_t *W, const xword_t *U, int Un)
{
    W[0] = 0;
    W[Un] = xll_mul_1(W+1, U+1, Un-1, U[0], 0);
    for (int j=1; j<Un; ++j)
    {
        W[j+Un] = xll_mul_add_1(W+2*j+1, U+j+1, Un-j-1, U[j]);
    }
    x_lshift(W, W, 2*Un, 1);
    xword_t kk = 0;
    for (int j=0; j<Un; ++j)
    {
        xword_t t0;
        xword_t t1;
        // k, W[0] = W[0] + U[0] * U[0]
        mul_1_1(t1, t0, U[j], U[j]);
        t0 += kk;
        t1 += t0 < kk;
        W[2*j] += t0;
        W[2*j+1] += W[2*j] < t0;
        W[2*j+1] += t1;
        kk = W[2*j+1] < t1;
    }
    return 0;
}
