
#include "xint.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

#define B 0x100000000ULL

static void trim_zeroes(xint_t u);
static int get_highest_word(xint_t x);
static int get_highest_bit(uint32_t word);
static void resize(xint_t x, size_t new_size);

static xword_t x_lshift(xword_t *Y, xword_t *X, size_t sz, int shift_bits);
static xword_t x_rshift(xword_t *Y, xword_t *X, size_t sz, int shift_bits);

static xword_t x_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n);
static xword_t x_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n);
static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n);
static xword_t x_sub_1(xword_t *W, xword_t *U, xword_t v, size_t n);
static xword_t x_mul(xword_t *W, xword_t *U, size_t m, xword_t *V, size_t n);
static uint32_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v);
static uint32_t xint_mul_add_1(xword_t *W, xword_t *U, size_t m, xword_t v);
static xword_t x_sub_mul_1(xword_t *W, const xword_t *U, size_t n, xword_t v);
static uint32_t x_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n);
static uint32_t x_div_1(xword_t *Q, xword_t *R, const xword_t *U, xword_t V, int m);

void xint_init(xint_t u, size_t hint)
{
    u->capacity = 0;
    u->size = 0;
    u->data = NULL;
    if (hint)
    {
        u->capacity = hint;
        u->data = malloc(sizeof(uint32_t) * u->capacity);
    }
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

int xint_is_zero(xint_t x)
{
    for (int j=0; j<x->size; ++j)
    {
        if (x->data[j])
        {
            return 0;
        }
    }
    return 1;
}

void xint_copy(xint_t u, const xint_t v)
{
    // Copy v to u
    resize(u, v->size);
    memcpy(u->data, v->data, u->size * sizeof(uint32_t));
}

void xint_swap(xint_t u, xint_t v)
{
    // Swap the internals of u and v
    xint_t T;
    *T = *u;
    *u = *v;
    *v = *T;
}

int xint_add(xint_t w, const xint_t u, const xint_t v)
{
    size_t Un = u->size;
    size_t Vn = v->size;
    xword_t k;

    resize(w, MAX(Un, Vn));
    size_t part1_len = MIN(Un, Vn);
    size_t part2_len = Un>Vn?Un-Vn:Vn-Un;
    k = x_add(w->data, u->data, v->data, part1_len);
    k = x_add_1(w->data+part1_len, u->data+part1_len, k, part2_len);
    // Make sure W is long enough for the carry
    if (k)
    {
        resize(w, w->size + 1);
        w->data[w->size - 1] = k;
    }
    return 0;
}

int xint_add_1(xint_t w, xint_t u, xword_t v)
{
    size_t Un = u->size;
    xword_t k;

    resize(w, Un);
    k = x_add_1(w->data, u->data, v, w->size);
    if (k)
    {
        resize(w, w->size + 1);
        w->data[w->size - 1] = k;
    }
    return 0;
}

int xint_cmp(const xint_t u, const xint_t v)
{
    if (u == v)
    {
        return 0;
    }
    if (u->size != v->size)
    {
        return u->size < v->size ? -1 : 1;
    }
    for (int j=u->size-1; j>=0; --j)
    {
        if (u->data[j] != v->data[j])
        {
            return u->data[j] < v->data[j] ? -1 : 1;
        }
    }
    return 0;
}

int xint_sub(xint_t w, const xint_t u, const xint_t v)
{
    size_t Un = u->size;
    size_t Vn = v->size;
    uint32_t b;

    int cmp = xint_cmp(u, v);
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
    //assert(b == 0);
    trim_zeroes(w);
    return cmp;
}

uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v)
{
    if (v->size == 0 || u->size == 0)
    {
        w->size = 0;
        return 0;
    }
    
    size_t Un = u->size;
    size_t Vn = v->size;
    
    resize(w, Un + Vn);
    
    xword_t *U = u->data;
    xword_t *V = v->data;
    xword_t *W = w->data;
    
    if (Vn == 1)
    {
        x_mul_1(W, U, Un, V[0]);
        trim_zeroes(w);
        return 0;
    }
    
    if (Un == 1)
    {
        x_mul_1(W, V, Vn, U[0]);
        trim_zeroes(w);
        return 0;
    }
    
    // As pointed out by Knuth, algorithm M can cope with V[j] being in the same
    // location as W[j+n]. In other words, V can be copied to the upper words
    // of W as long as there is zeroed space at the bottom of W to hold Un words
    if (w == v)
    {
        // Move V to the top of W - in reverse because there may be overlap
        for (int i=Vn-1; i>=0; --i)
        {
            W[i+Un] = V[i];
        }
        // Adjust the V pointer
        V = W + Un;
    }
    else if (w == u)
    {
        // Swap the references to U and V - note that the u passed in is never actually
        // modified. We are just renaming the arguments before running the algorithm
        size_t t = Un;
        Un = Vn;
        Vn = t;
        xword_t *T = U;
        U = V;
        V = T;
        
        // Move V to the top of W - in reverse because there may be overlap
        for (int i=Vn-1; i>=0; --i)
        {
            W[i+Un] = V[i];
        }
        // Adjust the V pointer
        V = W + Un;
    }
    
    x_mul(W, U, Un, V, Vn);
    trim_zeroes(w);
    return 0;
}

uint32_t xint_mul_1(xint_t w, const xint_t x, xword_t n)
{
    if (n == 0)
    {
        w->size = 0;
        return 0;
    }
    size_t Xn = x->size;
    resize(w, Xn + 1);
    x_mul_1(w->data, x->data, Xn, n);
    trim_zeroes(w);
    return 0;
}

uint32_t xint_div(xint_t q, xint_t r, xint_t u, xint_t v)
{
    // As per Knuth's algorithm D
    // u[0] to u[m+n-1]
    // v[0] to v[n-1]
    // q[0] to q[m]
    // r[0] to r[n-1]
    // Pick off the easy ones
    if (u->size == 0)
    {
        q->size = 0;
        r->size = 0;
        return 0;
    }

    if (v->size <= 1)
    {
        // Use the algorithm from exercise 16
        resize(r, 1);
        xint_div_1(q, r->data, u, v->data[0]);
        return 0;
    }

    int n = v->size;
    int m = u->size - v->size;

    // D1. [Normalise.]
    // Use Knuth's suggestion of a power of 2 for d. For v1 to be > b/2
    // we need v1 to have its top bit set.

    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(v);
    int highest_bit = get_highest_bit(v->data[highest_word]);

    int norm = 0;
    if ((v->data[v->size-1] & 0x80000000) == 0)
    {
        // Move both u and v to the left so that the top bit of V is set
        // Note that we use r for normalised u from now on
        xint_lshift(r, u, 31 - highest_bit); // r may sometimes grow
        xint_lshift(v, v, 31 - highest_bit); // v will never grow
        norm = 1;
    }
    else
    {
        xint_copy(r, u); // r may sometimes grow
    }

    resize(q, m + 1);

    resize(r, r->size + 1);
    r->data[r->size - 1] = 0;

    x_div(q->data, r->data, v->data, m, n);

    // D8. Un-normalise
    if (norm)
    {
        xint_rshift(v, v, 31 - highest_bit);
        xint_rshift(r, r, 31 - highest_bit);
    }
    r->size = n;

    trim_zeroes(q);
    trim_zeroes(r);
    trim_zeroes(v);

    return 0;
}

int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v)
{
    // This is from Knuth's recommended exercise 16
    if (u->size == 0)
    {
        q->size = 0;
        *r = 0;
        return 0;
    }
    if (v == 0)
    {
        return -1;
    }
    x_div_1(q->data, r, u->data, v, u->size);
    trim_zeroes(q);
    return 0;
}

uint32_t xint_lshift(xint_t y, const xint_t x, int numbits)
{
    if (x->size == 0)
    {
        return 0;
    }

    if (numbits == 0)
    {
        if (y != x)
        {
            xint_copy(y, x);
        }
        return 0;
    }

    // Calculate the shift
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

    resize(y, x->size + shift_words + (shift_bits?1:0));

    uint32_t *X = x->data;
    uint32_t *Y = y->data;
    
    if (shift_bits == 0)
    {
        for (int j=y->size-1; j>=shift_words; --j)
        {
            Y[j] = X[j - shift_words];
        }
    }
    else
    {
        x_lshift(Y + shift_words, X, y->size - shift_words, shift_bits);
    }
    memset(Y, 0, shift_words * sizeof(uint32_t));
    trim_zeroes(y);
    return 0;
}

uint32_t xint_rshift(xint_t y, const xint_t x, int numbits)
{
    if (x->size == 0)
    {
        return 0;
    }
    // Calculate the shift
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

    // If all of x will be shifted out ...
    if (x->size <= shift_words)
    {
        // set y to 0
        y->size = 0;
        return 0;
    }
    
    uint32_t *X = x->data;
    uint32_t *Y = y->data;
    // We need y->size to be at least x->size - shift_words in size
    // Note that if x and y are the same xint, resize
    // won't be called and x->size won't change
    if (y->size < x->size - shift_words)
    {
        resize(y, x->size - shift_words);
    }
    if (shift_bits == 0)
    {
        for (int j=0; j<x->size - shift_words; ++j)
        {
            y->data[j] = x->data[j + shift_words];
        }
    }
    else
    {
        x_rshift(Y, X + shift_words, x->size - shift_words, shift_bits);
    }
    resize(y, x->size - shift_words);
    trim_zeroes(y);

    return 0;
}

static void trim_zeroes(xint_t u)
{
    for (int j=u->size-1; j>=0; --j)
    {
        if (u->data[j] != 0)
        {
            u->size = j + 1;
            return;
        }
    }
    u->size = 0;
}

static int get_highest_word(xint_t x)
{
    for (int j=x->size-1; j>=0; --j)
    {
        if (x->data[j])
        {
            return j;
        }
    }
    return -1;
}

static int get_highest_bit(uint32_t word)
{
    if (word)
    {
        return 31 - __builtin_clz(word);
    }
    return -1;
}

static void resize(xint_t x, size_t new_size)
{
    if (new_size > x->size)
    {
        if (new_size > x->capacity)
        {
            if (x->data)
            {
                x->capacity = new_size;
                x->data = realloc(x->data, sizeof(uint32_t) * x->capacity);
                // Clear the new words XXX: can we get rid of this or make it an option?
                memset(&x->data[x->size], 0, sizeof(uint32_t) * (new_size - x->size));
            }
            else
            {
                x->capacity = new_size<20?20:new_size;
                x->data = malloc(sizeof(uint32_t) * x->capacity);
                // Clear the new words XXX: can we get rid of this or make it an option?
                memset(&x->data[x->size], 0, sizeof(uint32_t) * (new_size - x->size));
            }
        }
    }
    x->size = new_size;
}

static xword_t x_lshift(xword_t *Y, xword_t *X, size_t sz, int shift_bits)
{
    Y[sz - 1] = X[sz - 2] >> (32 - shift_bits);
    for (int j=sz-2; j>=1; --j)
    {
        Y[j] = (X[j] << shift_bits) | (X[j - 1] >> (32 - shift_bits));
    }
    Y[0] = X[0] << shift_bits;
    return 0;
}

static xword_t x_rshift(xword_t *Y, xword_t *X, size_t sz, int shift_bits)
{
    for (int j=0; j<sz - 1; ++j)
    {
        Y[j] = (X[j] >> shift_bits) | (X[j + 1] << (32 - shift_bits));
    }
    Y[sz - 1] = X[sz - 1] >> shift_bits;
    return 0;
}

static xword_t x_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
{
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
    uint32_t b = 0;
    for (size_t i=0; i<n; ++i)
    {
        // S2. [Sub digits.] Set Wj to (Uj - Vj + k) % b
        //                   and  k to (Uj - Vj + k) / b
        // Unlike Knuth we record positive borrow instead of
        // negative carry
        // We check for underflow by comparing the minuend and subtrahend
        uint32_t tmp1 = U[i] - b;
        b = b > U[i] ? 1 : 0;
        uint32_t tmp2 = tmp1 - V[i];
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
    x_mul_1(W, U, m, V[0]);
    for (size_t j=1; j<n; ++j)
    {
        // M2. [We skip the optional check for zero multiplier]
        W[j + m] = xint_mul_add_1(W+j, U, m, V[j]);
        // M6. [Loop on j]
    }
    return 0;
}

static uint32_t xint_mul_add_1(xword_t *W, xword_t *U, size_t m, xword_t v)
{
    // M3. [Initialise i (and k)]
    xword_t k = 0;
    for (size_t i=0; i<m; ++i)
    {
        // M4. [Multiply and add]
        uint64_t t = (uint64_t)U[i] * v + W[i] + k;
        W[i] = (xword_t)t;
        k = t >> 32;
        // M5. [Loop on i]
    }
    return k;
}

static xword_t x_mul_sub_1(xword_t *W, const xword_t *U, size_t n, xword_t v)
{
    uint32_t b = 0;
    for (int i=0; i<n; ++i)
    {
        uint64_t prod = (uint64_t)v * U[i];
        uint32_t tmp = W[i] - b;
        b = tmp > W[i] ? 1 : 0;
        W[i] = tmp - (uint32_t)prod;
        b += W[i] > tmp ? 1 : 0;
        b += prod >> 32;
    }
    return b;
}

static uint32_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v)
{
    // Cut down version of alg M with a single xword for V
    // We assume that U is m long and W is m+1 long.
    // i.e. W is m+n long as per full alg M
    xword_t k = 0;
    for (size_t j=0; j<m; ++j)
    {
        // M4. [Multiply and add]
        uint64_t t = (uint64_t)U[j] * v + k;
        W[j] = (xword_t)t;
        k = t >> 32;
        // M5. [Loop on i]
    }
    W[m] = k;
    return 0;
}

static uint32_t x_div(xword_t *Q, xword_t *R, const xword_t *V, int m, int n)
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

    // D2. [Initialise j.]
    for (int j=m; j>=0; --j)
    {
        // D3. [Calculate q^]
        uint64_t numer = (uint64_t)R[j+n] * B + R[j+n-1];
        uint32_t denom = V[n-1];
        uint64_t qhat = numer / denom;
        uint64_t rhat = numer % denom;
        while (qhat >= B || (qhat * V[n-2] > B * rhat + R[j+n-2]))
        {
            --qhat;
            rhat += V[n-1];
            if (rhat >= B)
            {
                break;
            }
        }
        
        // D4. [Multiply and subtract.]
        // Replace u(j+n to j) by u(j+n to j) - qhat * v(n-1 to 0)
        // Since r is u we have r = r - qhat * v
        uint32_t b = x_mul_sub_1(R+j, V, n, (uint32_t)qhat);
        b = b - R[j+n];
        assert(b == 0 || b == 1);
        
        Q[j] = (uint32_t)qhat;
        
        // D5. Test remainder
        if (b)
        {
            // D6. [Add back.]
            // Decrease Qj by one
            --Q[j];
            // Add V(Vn-1 to 0) to U(Vn+j to j)
            uint32_t k = x_add(R+j, R+j, V, n);
            assert(b - k == 0);
        }
        // D7. Loop on j
    }
    return 0;
}

static uint32_t x_div_1(xword_t *Q, xword_t *R, const xword_t *U, xword_t V, int m)
{
    // Renamed Knuth's W to Q
    // S1. [Set r = 0, j = n - 1]
    *R = 0;
    for (int j=m; j>=0; --j)
    {
        // S2. [Set wj = (r * B + uj) / v , r = (r * B + uj) % v
        uint64_t tmp = (uint64_t)*R * B + U[j];
        Q[j] = (uint32_t)(tmp / V);
        *R = tmp % V;
        // S3. [Decrease j by 1, and return to S2]
    }
    return 0;
}
