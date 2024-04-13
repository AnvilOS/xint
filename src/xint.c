
#include "xint.h"

#include <stdlib.h>

static xword_t x_add(xword_t *W, xword_t *U, xword_t *V, size_t n);
static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n);

void xint_init(xint_t u, int hint)
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

void xint_add(xint_t w, xint_t u, xint_t v)
{
    x_add(w->data, u->data, v->data, u->size);
}

void xint_sub(xint_t w, xint_t u, xint_t v)
{
    x_sub(w->data, u->data, v->data, u->size);
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
        uint32_t sum = U[j] + k;
        k = sum < k ? 1 : 0;
        sum += V[j];
        k += sum < V[j] ? 1 : 0;
        W[j] = sum;
        // A3. [Loop on j]
    }
    // Return Wn
    return k;
}

static xword_t x_sub(xword_t *W, xword_t *U, xword_t *V, size_t n)
{
    // This function will work if any or all of the xints are
    // in the same place in memory. e.g. a = a - a will work
    // S1. [Initialise.]
    uint32_t b = 0;
    for (int i=0; i<n; ++i)
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
