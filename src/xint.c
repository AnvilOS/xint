
#include "xint.h"

#include <stdlib.h>
#include <string.h>

static void resize(xint_t x, size_t new_size);

static xword_t x_add(xword_t *W, xword_t *U, xword_t *V, size_t n);
static xword_t x_add_1(xword_t *W, xword_t *U, xword_t v, size_t n);
static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n);
static xword_t x_sub_1(xword_t *W, xword_t *U, xword_t v, size_t n);
static xword_t x_mul(xword_t *W, xword_t *U, size_t m, xword_t *V, size_t n);
static uint32_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v);

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

void xint_add(xint_t w, xint_t u, xint_t v)
{
    x_add(w->data, u->data, v->data, u->size);
}

void xint_sub(xint_t w, xint_t u, xint_t v)
{
    x_sub(w->data, u->data, v->data, u->size);
}

void xint_mul(xint_t w, xint_t u, xint_t v)
{
    w->size = u->size + v->size;
    x_mul(w->data, u->data, u->size, v->data, v->size);
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

static xword_t x_add(xword_t *W, xword_t *U, xword_t *V, size_t n)
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

static xword_t x_add_1(xword_t *W, xword_t *U, xword_t v, size_t n)
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
    for (size_t i=0; i<m; ++i)
    {
        W[i] = 0;
    }
    
    for (size_t j=0; j<n; ++j)
    {
        // M2. [We skip the optional check for zero multiplier]
        
        // M3. [Initialise i (and k)]
        xword_t k = 0;
        for (size_t i=0; i<m; ++i)
        {
            // M4. [Multiply and add]
            uint64_t t = (uint64_t)U[i] * V[j] + W[i + j] + k;
            W[i + j] = (xword_t)t;
            k = t >> 32;
            // M5. [Loop on i]
        }
        W[j + m] = k;
        // M6. [Loop on j]
    }
    return 0;
}

uint32_t x_mul_1(xword_t *W, xword_t *U, size_t m, xword_t v)
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
