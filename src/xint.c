
#include "xint.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

#define B 0x100000000ULL

static void trim_zeroes(xint_t u);
static int get_highest_word(xint_t x);
static int get_highest_bit(xword_t word);
static int resize(xint_t x, int new_size);
static int add_or_sub(xint_t w, const xint_t u, const xint_t v, int Upos, int Vpos);
static int add_or_sub_long(xint_t w, const xint_t u, unsigned long v, int Upos, int Vpos);

static void x_zero(xword_t *Y, size_t sz);
static void x_move(xword_t *Y, xword_t *X, size_t sz);
static xword_t x_lshift(xword_t *Y, xword_t *X, int sz, int shift_bits);
static xword_t x_rshift(xword_t *Y, xword_t *X, int sz, int shift_bits);
static xword_t x_cmp(const xword_t *U, int Un, const xword_t *V, int Vn);
static xword_t x_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
static xword_t x_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n);
static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n);
static xword_t x_sub_1(xword_t *W, xword_t *U, xword_t v, size_t n);
static xword_t x_mul(xword_t *W, xword_t *U, size_t m, xword_t *V, size_t n);
static xword_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v, xword_t k);
static xdword_t x_mul_2(xword_t *W, xword_t *U, size_t m, xdword_t v);
static xword_t x_mul_add_1(xword_t *W, xword_t *U, size_t m, xword_t v);
static xword_t x_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n);
static xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int m);
static xdword_t x_squ_add_1(xword_t *W, xword_t *U, int sz);

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
void xint_assign_ulong(xint_t u, unsigned long val)
{
    u->size = 0;
    while (val)
    {
        FAST_RESIZE(u, u->size + 1);
        u->data[u->size - 1] = (xword_t)val;
        val >>= XWORD_BITS;
    }
}

void xint_assign_long(xint_t u, long val)
{
    xint_assign_ulong(u, labs(val));
    if (val < 0)
    {
        xint_chs(u);
    }
}

// Compare functions
int xint_cmpa_ulong(const xint_t u, const unsigned long v)
{
    if (v <= XWORD_MAX)
    {
        // fits in a single xword
        xword_t vv = (xword_t)v;
        return x_cmp(u->data, abs(u->size), &vv, 1);
    }
    xword_t vvv[4];
    xint_t vv = { 4, 0, vvv };
    xint_assign_ulong(vv, v);
    return xint_cmp(u, vv);
}

int xint_cmp_ulong(const xint_t u, const unsigned long val)
{
    return u->size < 0 ? -1 : xint_cmpa_ulong(u, val);
}

int xint_cmp_long(const xint_t u, const long val)
{
    if (val >= 0)
    {
        return xint_cmp_ulong(u, val);
    }
    // val is neg
    if (u->size >= 0)
    {
        return 1;
    }
    // Both are neg
    return -xint_cmpa_ulong(u, -val);
}

int xint_cmp(const xint_t u, const xint_t v)
{
    if (u->size != v->size)
    {
        return u->size < v->size ? -1 : 1;
    }
    return u->size > 0 ? x_cmp(u->data, abs(u->size), v->data, abs(v->size)) : -x_cmp(v->data, abs(v->size), u->data, abs(u->size));
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
        xint_t vv = { 4, 0, vvv };
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
    xword_t k = x_add(w->data, u->data, v->data, part1_len);
    k = x_add_1(w->data+part1_len, bigger+part1_len, k, part2_len);
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
        xword_t k = x_add_1(w->data, u->data, (xword_t)v, Un);
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

    int cmp = x_cmp(u->data, Un, v->data, Vn);
    switch (cmp)
    {
        case 0: // U == V
            w->size = 0;
            return 0;
            
        case -1: // U < V
            resize(w, Vn);
            b = x_sub(w->data, v->data, u->data, Un);
            b = x_sub_1(w->data+Un, v->data+Un, b, Vn-Un);
            break;

        case 1: // U > V
            resize(w, Un);
            b = x_sub(w->data, u->data, v->data, Vn);
            b = x_sub_1(w->data+Vn, u->data+Vn, b, Un-Vn);
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
        int cmp = x_cmp(u->data, abs(u->size), &vv, 1);
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
                    b = x_sub_1(w->data, &vv, u->data[0], Un);
                }
                break;

            case 1: // U > V
                resize(w, Un);
                b = x_sub_1(w->data, u->data, vv, Un);
                break;
        }
        assert(b == 0);
        trim_zeroes(w);
        return 0;
    }
    xword_t vvv[4];
    xint_t vv = { 4, 0, vvv };
    xint_assign_ulong(vv, v);
    return xint_suba(w, u, vv);
}

// Multiplication functions
xword_t xint_sqr(xint_t w, const xint_t u)
{
    if (u->size == 0)
    {
        w->size = 0;
        return 0;
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
    xword_t prev_k = 0;
    for (int j=0; j<Un; ++j)
    {
        xdword_t k = x_squ_add_1(W+j+j, U+j, Un-j);
        W[Un + j] = (xword_t)k;
        W[Un + j] += prev_k;
        prev_k = k >> 32;
    }
    assert(prev_k == 0);

    trim_zeroes(w);
    return 0;
}

xword_t xint_mul(xint_t w, const xint_t u, const xint_t v)
{
    int Un = abs(u->size);
    int Vn = abs(v->size);
    int Ws = (u->size * v->size) > 0;
    
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
            return 0;
        }
        else if (Vn == 1)
        {
            FAST_RESIZE(w, Un + 1);
            xword_t k = x_mul_1(w->data, u->data, Un, v->data[0], 0);
            if (k)
            {
                w->data[Un] = k;
            }
            else
            {
                --w->size;
            }
            return 0;
        }
        else
        {
            FAST_RESIZE(w, Un + 2);
            xdword_t k = x_mul_2(w->data, u->data, Un, (xdword_t)v->data[1] << 32 | v->data[0]);
            if (k > XWORD_MAX)
            {
                //FAST_RESIZE(w, Un + 2);
                w->data[Un] = (xword_t)k;
                w->data[Un + 1] = k >> 32;
            }
            else if (k)
            {
                w->data[Un] = (xword_t)k;
                --w->size;
            }
            else
            {
                w->size -= 2;
            }
            return 0;
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
        x_mul(W, U, Un, V, Vn);
    }
    else if (w == u)
    {
        // Move U to the top of W - in reverse because there may be overlap
        x_move(W+Vn, U, Un);
        // Adjust the V pointer
        U = W + Vn;
        x_mul(W, V, Vn, U, Un);
    }
    else
    {
        x_mul(W, U, Un, V, Vn);
    }
    trim_zeroes(w);
    xint_set_sign(w, Ws);
    return 0;
}

xword_t xint_mul_ulong(xint_t w, const xint_t u, unsigned long v)
{
    if (v > XDWORD_MAX)
    {
        xword_t vvv[4];
        xint_t vv = { 4, 0, vvv };
        xint_assign_ulong(vv, v);
        return xint_mul(w, u, vv);
    }

    int Uneg = xint_is_neg(u);
    int Un = abs(u->size);
    if (v <= XWORD_MAX)
    {
        FAST_RESIZE(w, Un + 1);
        xword_t k = x_mul_1(w->data, u->data, Un, (xword_t)v, 0);
        if (k)
        {
            w->data[Un] = k;
        }
        else
        {
            --w->size;
        }
    }
    else if (v <= XDWORD_MAX)
    {
        FAST_RESIZE(w, Un + 2);
        xdword_t k = x_mul_2(w->data, u->data, Un, v);
        if (k > XWORD_MAX)
        {
            //FAST_RESIZE(w, Un + 2);
            w->data[Un] = (xword_t)k;
            w->data[Un + 1] = k >> 32;
        }
        else if (k)
        {
            w->data[Un] = (xword_t)k;
            --w->size;
        }
        else
        {
            w->size -= 2;
        }
    }
    if (Uneg)
    {
        xint_set_neg(w);
    }
    return 0;
}

xword_t xint_mul_long(xint_t w, const xint_t u, long v)
{
    xint_mul_ulong(w, u, labs(v));
    if (v < 0)
    {
        xint_chs(w);
    }
    return 0;
}

xword_t xint_muladd_ulong(xint_t w, xint_t u, unsigned long m, unsigned long a)
{
    if (m <= XWORD_MAX && a <= XWORD_MAX)
    {
        int Un = abs(u->size);
        FAST_RESIZE(w, Un+1);
        xword_t k = x_mul_1(w->data, u->data, Un, (xword_t)m, (xword_t)a);
        if (k)
        {
            w->data[Un] = k;
        }
        else
        {
            --w->size;
        }
        return k;
    }
    return 0;
}

// Division functions
int xint_highest_bit(xint_t x)
{
    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(x);
    int highest_bit = get_highest_bit(x->data[highest_word]);
    return highest_word * 32 + highest_bit;
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
    return xint_div(0, r, u, v);
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
        resize(r, 1);
        r->data[0] = rem;
        return 0;
    }
    int cmp = x_cmp(u->data, Un, v->data, Vn);
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

    if (q)
    {
        resize(q, m + 1);
    }
    resize(r, m + n + 1);
    r->data[m + n] = 0;

    if (q)
    {
        assert(abs(q->size) == m + 1);
    }
    assert(abs(r->size) == m + n + 1);
    assert(abs(v->size) == n);
    
    x_div(q?q->data:r->data + n, r->data, v->data, m, n);
    //x_div(q->data, r->data, v->data, m, n);

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
    resize(q, Un);
    *r = x_div_1(q->data, u->data, v, Un);
    trim_zeroes(q);
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
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

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
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

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

static int get_highest_word(xint_t x)
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

static int get_highest_bit(xword_t word)
{
    if (word)
    {
        return 31 - __builtin_clz(word);
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

static void x_zero(xword_t *Y, size_t sz)
{
    memset(Y, 0, sz * sizeof(xword_t));
}

static void x_move(xword_t *Y, xword_t *X, size_t sz)
{
    memmove(Y, X, sz * sizeof(xword_t));
}

static xword_t x_lshift(xword_t *Y, xword_t *X, int sz, int shift_bits)
{
    xword_t ret = X[sz - 1] >> (32 - shift_bits);
    for (int j=sz-1; j>=1; --j)
    {
        Y[j] = (X[j] << shift_bits) | (X[j - 1] >> (32 - shift_bits));
    }
    Y[0] = X[0] << shift_bits;
    return ret;
}

static xword_t x_rshift(xword_t *Y, xword_t *X, int sz, int shift_bits)
{
    for (int j=0; j<sz - 1; ++j)
    {
        Y[j] = (X[j] >> shift_bits) | (X[j + 1] << (32 - shift_bits));
    }
    Y[sz - 1] = X[sz - 1] >> shift_bits;
    return 0;
}

static xword_t x_cmp(const xword_t *U, int Un, const xword_t *V, int Vn)
{
    if (Un != Vn)
    {
        return Un < Vn ? -1 : 1;
    }
    for (int j=Un-1; j>=0; --j)
    {
        if (U[j] != V[j])
        {
            return U[j] < V[j] ? -1 : 1;
        }
    }
    return 0;
}

static xword_t x_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
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
        k = sum < k ? 1 : 0;
        sum += V[j];
        k += sum < V[j] ? 1 : 0;
        W[j] = sum;
        // A3. [Loop on j]
    }
    // Return Wn
    return k;
}

static xword_t x_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n)
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

static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n)
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
        xword_t tmp2 = tmp1 - V[i];
        b += V[i] > tmp1 ? 1 : 0;
        W[i] = tmp2;
        // S3. [Loop on j]
    }
    return b;
}

static xword_t x_sub_1(xword_t *W, xword_t *U, xword_t v, size_t n)
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

static xword_t x_mul(xword_t *W, xword_t *U, size_t m, xword_t *V, size_t n)
{
    // Based on Knuth's algorithm M.
    // As pointed out by Knuth, algorithm M can cope with V[j] being co-located
    // with W[j+n]. In other words, V and W can be co-located as long as V is
    // copied to the upper words of W prior to running the algorithm. The lower
    // m xwords of W will be zeroed

    // M1. [Initialisation.] - clear the bottom part of W
    W[m] = x_mul_1(W, U, m, V[0], 0);
    for (size_t j=1; j<n; ++j)
    {
        // M2. [We skip the optional check for zero multiplier]
        W[j + m] = x_mul_add_1(W+j, U, m, V[j]);
        // M6. [Loop on j]
    }
    return 0;
}

static xword_t x_mul_add_1(xword_t *W, xword_t *U, size_t m, xword_t v)
{
    // W[] += U[] * v
    // M3. [Initialise i (and k)]
    xword_t k = 0;
    for (size_t i=0; i<m; ++i)
    {
        // M4. [Multiply and add]
        xdword_t t = (xdword_t)U[i] * v + W[i] + k;
        W[i] = (xword_t)t;
        k = t >> 32;
        // M5. [Loop on i]
    }
    return k;
}

static xword_t x_mul_sub_1(xword_t *W, const xword_t *U, size_t n, xword_t v)
{
    // W[] -= U[] * v
    xword_t b = 0;
    for (int i=0; i<n; ++i)
    {
        xdword_t prod = (xdword_t)v * U[i];
        xword_t tmp = W[i] - b;
        b = tmp > W[i] ? 1 : 0;
        W[i] = tmp - (xword_t)prod;
        b += W[i] > tmp ? 1 : 0;
        b += prod >> 32;
    }
    return b;
}

static xword_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v, xword_t k)
{
    // W[] = U[] * v + k
    // Cut down version of alg M with a single xword for V
    // We assume that U is m long and W is m+1 long.
    // i.e. W is m+n long as per full alg M
    for (size_t j=0; j<m; ++j)
    {
        // M4. [Multiply and add]
        xdword_t t = (xdword_t)U[j] * v + k;
        W[j] = (xword_t)t;
        k = t >> 32;
        // M5. [Loop on i]
    }
    return k;
}

static xdword_t x_mul_2(xword_t *W, xword_t *U, size_t m, xdword_t v)
{
    xword_t v0 = (xword_t)v;
    xword_t v1 = v >> 32;
    xdword_t k = 0;
    for (int j=0; j<m; ++j)
    {
        xdword_t p = (xdword_t)U[j] * v0 + (k & 0xffffffff);
        k = (xdword_t)U[j] * v1 + (k >> 32) + (p >> 32);
        W[j] = (xword_t)p;
    }
    return k;
}

static xword_t x_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n)
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
    int bit_shift = 31 - get_highest_bit(V[n-1]);
    xword_t V_nm1 = bit_shift ? V[n-1] << bit_shift | V[n-2] >> (32 - bit_shift) : V[n-1];
    xword_t V_nm2 = bit_shift ? V[n-2] << bit_shift | V[n-3] >> (32 - bit_shift) : V[n-2];

    // D2. [Initialise j.]
    for (int j=m; j>=0; --j)
    {
        // D3. [Calculate q^]
        xdword_t numer = (xdword_t)R[j+n] * B + R[j+n-1];
        xword_t R_jnm2 = R[j+n-2];;
        if (bit_shift)
        {
            numer <<= bit_shift;
            numer |= R[j+n-2] >> (32 - bit_shift);
            R_jnm2 <<= bit_shift;
            if (j+n-3 >= 0)
            {
                R_jnm2 |= R[j+n-3] >> (32 - bit_shift);
            }
        }
        xword_t denom = V_nm1;
        xdword_t qhat = numer / denom;
        xdword_t rhat = numer % denom;
        while (qhat >= B || (qhat * V_nm2 > B * rhat + R_jnm2))
        {
            --qhat;
            rhat += denom;
            if (rhat >= B)
            {
                break;
            }
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
            xword_t k = x_add(R+j, R+j, V, n);
            assert(b - k == 0);
        }
        // D7. Loop on j
    }
    // D8. Un-normalise
    // Not needed
    return 0;
}

static xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int n)
{
    // Renamed Knuth's W to Q
    // S1. [Set r = 0, j = n - 1]
    xword_t R = 0;
    for (int j=n-1; j>=0; --j)
    {
        // S2. [Set wj = (r * B + uj) / v , r = (r * B + uj) % v
        xdword_t tmp = (xdword_t)R * B + U[j];
        if (Q)
        {
            Q[j] = (xword_t)(tmp / V);
        }
        R = tmp % V;
        // S3. [Decrease j by 1, and return to S2]
    }
    return R;
}

static xdword_t x_squ_add_1(xword_t *W, xword_t *U, int sz)
{
    xdword_t k = 0;

    xdword_t t = (xdword_t)U[0] * U[0] + W[0];
    W[0] = t & 0xffffffff;
    k = t >> 32;

    for (int i=1; i<sz; ++i)
    {
        int of = 0;
        xdword_t t = (xdword_t)U[i] * U[0];
        of = ((t & 0x8000000000000000ULL) != 0);
        t <<= 1;
        t += W[i];
        of += t < W[i];
        // Max was fffffffe3d43b851 at b504f333
        t += k;
        // 1C2BC47AE
        of += t < k;
        W[i] = t & 0xffffffff;
        k = t >> 32;
        k |= (xdword_t)of << 32;
    }
    return k;
}
