
#include "xint_internal.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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
    int Vn = XINT_ABS(v->size);
    FAST_RESIZE(u, Vn);
    xll_move(u->data, v->data, Vn);
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
    xint_assign_ulong(u, XINT_ABS(v));
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
    int Un = XINT_ABS(u->size);
    
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
    int un = XINT_ABS(u->size);
    int vn = XINT_ABS(v->size);
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
    return un > 0 ? xll_cmp(u->data, v->data, XINT_ABS(un)) : xll_cmp(v->data, u->data, XINT_ABS(un));
}

// Addition and Subtraction functions
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

void xint_add_ulong(xint_t w, const xint_t u, const unsigned long val)
{
    add_or_sub_long(w, u, val, u->size >= 0, 1);
}

void xint_add_long(xint_t w, const xint_t u, const long val)
{
    add_or_sub_long(w, u, XINT_ABS(val), u->size >= 0, val >= 0);
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
    add_or_sub_long(w, u, XINT_ABS(val), u->size >= 0, val < 0);
}

void xint_sub(xint_t w, const xint_t u, const xint_t v)
{
    add_or_sub(w, u, v, !xint_is_neg(u), xint_is_neg(v));
}

int xint_adda(xint_t w, const xint_t u, const xint_t v)
{
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
    int Wn = XINT_MAX(Un, Vn);

    resize(w, Wn);
    int part1_len = XINT_MIN(Un, Vn);
    int part2_len = XINT_ABS(Un - Vn);
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
        int Un = XINT_ABS(u->size);
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
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
    xword_t b = 0;

    int cmp = xint_cmpa(u, v);
    switch (cmp)
    {
        case 0: // U == V
            w->size = 0;
            return 0;
            
        case -1: // U < V
            resize(w, Vn);
            if (Un == 0)
            {
                xll_move(w->data, v->data, Vn);
            }
            else
            {
                b = xll_sub(w->data, v->data, u->data, Un);
                if (Vn-Un)
                {
                    b = xll_sub_1(w->data+Un, v->data+Un, b, Vn-Un);
                }
            }
            break;

        case 1: // U > V
            resize(w, Un);
            if (Vn == 0)
            {
                xll_move(w->data, u->data, Un);
            }
            else
            {
                b = xll_sub(w->data, u->data, v->data, Vn);
                if (Un-Vn)
                {
                    b = xll_sub_1(w->data+Vn, u->data+Vn, b, Un-Vn);
                }
            }
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
        int Un = XINT_ABS(u->size);
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
//    xint_mul(w, u, u);
//    return;
    if (u->size == 0)
    {
        w->size = 0;
        return;
    }
    int Un = XINT_ABS(u->size);
    resize(w, 2 * Un);
    
    xword_t *U = u->data;
    xword_t *W = w->data;

    if (w == u)
    {
        // Move V to the top of W - in reverse because there may be overlap
        xword_t tmp[Un];
        xll_move(tmp, u->data, Un);
        //U = tmp;
        xll_zero(w->data, Un);
        xll_squ_2(W, tmp, Un);
    }
    else
    {
        // Clear the bottom words
        xll_zero(w->data, Un);
        xll_squ_2(W, U, Un);
    }

    trim_zeroes(w);
    return;
}

void xll_mul_wrap(xword_t *W, const xword_t *U, int m, const xword_t *V, int n);

void xll_mul_karatsuba(xword_t *W, const xword_t *U, int Un, const xword_t *V, int Vn)
{
    int u1n = Un / 2;
    int u0n = Un - u1n;
    
    int v1n = Un / 2;
    int v0n = Un - v1n;
    
    XLL_ASSERT((u0n - u1n == 0) || (u0n - u1n == 1));
    //XLL_ASSERT((v0n - u1n == 0) || (u0n - u1n == 1));
    XLL_ASSERT(u0n == v0n);

    xword_t *tmp = alloca(2*u0n*sizeof(xword_t));
    
    // Calculate the diffs first
    int sign_diffs;

    // W = (u1 - u0)
    if ((u1n < u0n) || (xll_cmp(U+u0n, U, u0n) < 0))
    {
        xword_t b = xll_sub(W, U, U+u0n, u1n);
        if (b) xll_sub_1(W+u1n, W+u1n, b, 2*u0n-u1n);
        XLL_ASSERT(b==0);
        sign_diffs = -1;
    }
    else
    {
        xword_t b = xll_sub(W, U+u0n, U, u1n);
        if (b) xll_sub_1(W+u1n, W+u1n, b, 2*u0n-u1n);
        XLL_ASSERT(b==0);
        sign_diffs = 1;
    }
    
    // W + u0n = (v1 - v0)
    if ((v1n < v0n) || (xll_cmp(V+v0n, V, v0n) < 0))
    {
        xword_t b = xll_sub(W+u0n, V, V+v0n, v1n);
        if (b) xll_sub_1(W+u0n+v1n, W+u0n+u1n, b, 2*v0n-v1n);
        XLL_ASSERT(b==0);
        sign_diffs *= -1;
    }
    else
    {
        xword_t b = xll_sub(W+u0n, V+v0n, V, v1n);
        if (b) xll_sub_1(W+u0n+v1n, W+u0n+u1n, b, 2*v0n-v1n);
        XLL_ASSERT(b==0);
        sign_diffs *= 1;
    }

    // tmp = (u1-u0) * (v1-v0)
    xll_mul_wrap(tmp, W, u0n, W+u0n, u0n);
    
    // Calculate the upper and lower products
    xll_mul_wrap(W, U, u0n, V, u0n);
    xll_mul_wrap(W+2*u0n, U+u0n, u0n, V+u0n, u0n);
    
    // Now calculate the middle term
    // mid = u1v1 + u0v0 - ((u1-u0) * (v1-v0))
    // We already have the 3rd term in tmp
    xword_t k;
    if (sign_diffs == -1)
    {
        k = xll_add(tmp, tmp, W, 2*u0n);
        XLL_ASSERT(k==0);
        k = xll_add(tmp, tmp, W+2*u0n, 2*u0n);
        XLL_ASSERT(k==0);
    }
    else if (xll_cmp(tmp, W, 2*u0n) > 0)
    {
        xword_t b;
        b = xll_sub(tmp, tmp, W, 2*u0n);
        XLL_ASSERT(b==0);
        b = xll_sub(tmp, W+2*u0n, tmp, 2*u0n);
        XLL_ASSERT(b==0);
    }
    else
    {
        k = xll_sub(tmp, W, tmp, 2*u0n);
        XLL_ASSERT(k==0);
        k = xll_add(tmp, tmp, W+2*u0n, 2*u0n);
        //XLL_ASSERT(k==0);
    }
    k = xll_add_1(W+3*u0n, W+3*u0n, k, u0n);
    k = xll_add(W+u0n, W+u0n, tmp, 2*u0n);
    k = xll_add_1(W+3*u0n, W+3*u0n, k, u0n);
    XLL_ASSERT(k==0);
    //free(tmp);
}

int kara_cutoff = 100000;

void xll_mul_wrap(xword_t *W, const xword_t *U, int m, const xword_t *V, int n)
{
    if (n > kara_cutoff)
    {
        xll_mul_karatsuba(W, U, m, V, n);
    }
    else
    {
        xll_mul_algm(W, U, m, V, n);
    }
}

void xint_mul(xint_t w, const xint_t u, const xint_t v)
{
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
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
            xword_t k = xll_mul_1(w->data, u->data, Un, v->data[0]);
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
    xword_t tmpv[100];
    xword_t tmpu[100];
    if (w == v)
    {
        // Move V to the top of W - in reverse because there may be overlap
        xll_move(tmpv, V, Vn);
        V = tmpv;
        //V = W + Un;
        //xll_mul(W, U, Un, tmp, Vn);
    }
    if (w == u)
    {
        // Move U to the top of W - in reverse because there may be overlap
        xll_move(tmpu, U, Un);
        // Adjust the V pointer
        U = tmpu;
        //U = W + Vn;
 //       xll_mul(W, V, Vn, tmp, Un);
    }
//    else
    {
        xll_mul_wrap(W, U, Un, V, Vn);
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
    int Un = XINT_ABS(u->size);
    if (v <= XWORD_MAX)
    {
        FAST_RESIZE(w, Un + 1);
        xword_t k = xll_mul_1(w->data, u->data, Un, (xword_t)v);
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
    xint_mul_ulong(w, u, XINT_ABS(v));
    if (v < 0)
    {
        xint_chs(w);
    }
}

void xint_muladd_ulong(xint_t w, xint_t u, unsigned long m, unsigned long a)
{
    if (m <= XWORD_MAX && a <= XWORD_MAX)
    {
        xint_mul_ulong(w, u, m);
        xint_add_ulong(w, w, a);
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
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
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
        assert(XINT_ABS(q->size) == m + 1);
    }

    assert(XINT_ABS(r->size) == m + n + 1);
    assert(XINT_ABS(v->size) == n);
    
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
    int Un = XINT_ABS(u->size);
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

    int Xn = XINT_ABS(x->size);

    if (Xn == 0)
    {
        y->size = 0;
        return 0;
    }

    resize(y, Xn + shift_words + (shift_bits?1:0));
    xword_t *X = x->data;
    int Yn = XINT_ABS(y->size);
    xword_t *Y = y->data;
    
    if (shift_bits == 0)
    {
        xll_move(Y + shift_words, X, Xn);
    }
    else
    {
        Y[Yn - 1] = x_lshift(Y + shift_words, X, Xn, shift_bits);
    }
    xll_zero(Y, shift_words);
    trim_zeroes(y);
    return 0;
}

xword_t xint_rshift(xint_t y, const xint_t x, int numbits)
{
    // Calculate the shift
    int shift_words = numbits / XWORD_BITS;
    int shift_bits = numbits % XWORD_BITS;

    int Xn = XINT_ABS(x->size);

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
        xll_move(Y, X + shift_words, Xn - shift_words);
    }
    else
    {
        x_rshift(Y, X + shift_words, Xn - shift_words, shift_bits);
    }
    trim_zeroes(y);

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

xword_t x_lshift(xword_t *Y, const xword_t *X, int sz, int shift_bits)
{
    xword_t ret = X[sz - 1] >> (XWORD_BITS - shift_bits);
    for (int j=sz-1; j>=1; --j)
    {
        Y[j] = (X[j] << shift_bits) | (X[j - 1] >> (XWORD_BITS - shift_bits));
    }
    Y[0] = X[0] << shift_bits;
    return ret;
}

xword_t x_rshift(xword_t *Y, const xword_t *X, int sz, int shift_bits)
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

#if !defined __arm__
xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
{
    XLL_ASSERT(n != 0);
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
        b = b > U[i];
        b += V[i] > tmp1;
        W[i] = tmp1 - V[i];
        // S3. [Loop on j]
    }
    return b;
}
#endif

#if !defined __arm__
xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n)
{
    XLL_ASSERT(n != 0);
    xword_t b = v;
    for (size_t i=0; i<n; ++i)
    {
        xword_t tmp1 = U[i] - b;
        b = b > U[i];
        W[i] = tmp1;
    }
    return b;
}
#endif

#define UMAAL(__a, __b, __c, __d) \
{ \
    xword_t __t1, __t0; \
    mul_1_1(__t1, __t0, __c, __d); \
    __t0 += __b; \
    __t1 += __t0 < __b; \
    __t0 += __a; \
    __t1 += __t0 < __a; \
    __a = __t0; \
    __b = __t1; \
}

#if !defined __arm__
xword_t xll_mul_add_4(xword_t *W, const xword_t *U, size_t m, const xword_t *V)
{
    xword_t k0 = 0;
    xword_t k1 = 0;
    xword_t k2 = 0;
    xword_t k3 = 0;
    for (int j=0; j<m; ++j)
    {
        UMAAL(W[j], k0, U[j], V[0]);
        UMAAL(k0, k1, U[j], V[1]);
        UMAAL(k1, k2, U[j], V[2]);
        UMAAL(k2, k3, U[j], V[3]);
    }
    W[m] = k0;
    W[m+1] = k1;
    W[m+2] = k2;
    return k3;
}
#endif

#if !defined __arm__
xword_t xll_mul_add_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0)
{
    xword_t k0 = 0;
    xword_t k1 = 0;
    for (int j=0; j<m; ++j)
    {
        UMAAL(W[j], k0, U[j], v0);
        UMAAL(k0, k1, U[j], v1);
    }
    W[m] = k0;
    return k1;
}
#endif

void xll_mul_algm(xword_t *W, const xword_t *U, size_t m, const xword_t *V, size_t n)
{
    // Based on Knuth's algorithm M.
    // As pointed out by Knuth, algorithm M can cope with V[j] being co-located
    // with W[j+n]. In other words, V and W can be co-located as long as V is
    // copied to the upper words of W prior to running the algorithm. The lower
    // m xwords of W will be zeroed

    // M1. [Initialisation.] - clear the bottom part of W
    W[m] = xll_mul_1(W, U, m, V[0]);
    for (size_t j=1; j<n; ++j)
    {
        // M2. [We skip the optional check for zero multiplier]
        if (n - j > 4)
        {
            W[j + m + 3] = xll_mul_add_4(W+j, U, m, V+j);
            j += 3;
        }
        else
        if (n - j > 2)
        {
            W[j + m + 1] = xll_mul_add_2(W+j, U, m, V[j+1], V[j]);
            ++j;
        }
        else
        {
            W[j + m] = xll_mul_add_1(W+j, U, m, V[j]);
        }
        // M6. [Loop on j]
    }
}

#if !defined __arm__
xword_t xll_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v)
{
    // W[] += U[] * v
    // M3. [Initialise i (and k)]
    xword_t k = 0;
    for (size_t i=0; i<m; ++i)
    {
        // M4. [Multiply and add]
        UMAAL(W[i], k, U[i], v);
        // M5. [Loop on i]
    }
    return k;
}
#endif

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

#if !defined __arm__
xword_t xll_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v)
{
    // W[] = U[] * v + k
    // Cut down version of alg M with a single xword for V
    // We assume that U is m long and W is m+1 long.
    // i.e. W is m+n long as per full alg M
    xword_t k = 0;
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
#endif

xword_t xll_mul_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0)
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

xword_t x_div_1(xword_t *Q, const xword_t *U, xword_t V, int n)
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

void xll_squ_2(xword_t *W, const xword_t *U, int Un)
{
    W[0] = 0;
    W[Un] = xll_mul_1(W+1, U+1, Un-1, U[0]);
    for (int j=1; j<Un-1; ++j)
    {
        W[j+Un] = xll_mul_add_1(W+2*j+1, U+j+1, Un-j-1, U[j]);
    }
    W[2*Un-1] = 0;
    x_lshift(W, W, 2*Un, 1);
    xword_t kk = 0;
    for (int j=0; j<Un; ++j)
    {
        UMAAL(W[2*j], kk, U[j], U[j]);
        W[2*j+1] += kk;
        kk = W[2*j+1] < kk;
    }
    XLL_ASSERT(kk==0);
}
