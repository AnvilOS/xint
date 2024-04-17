
#ifndef XINT_H
#define XINT_H

#include <stdint.h>

#define XINT_INIT_VAL { 0, 0, NULL };

typedef uint32_t xword_t;

struct xint_s
{
    short capacity; // Indicates the total size of the xword array
    short size;     // Words currently in use.
    
    // This stores the data in little-endian format. This will simplify
    // growing and shrinking the array
    xword_t *data;
};
typedef struct xint_s xint_t[1];

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void xint_init(xint_t u, size_t capacity);
void xint_delete(xint_t u);

int xint_add(xint_t w, const xint_t u, const xint_t v);
int xint_sub(xint_t w, const xint_t u, const xint_t v);
uint32_t xint_mul(xint_t w, const xint_t u, const xint_t v);
int xint_div_1(xint_t q, xword_t *r, const xint_t u, uint32_t v);
uint32_t xint_lshift(xint_t y, const xint_t x, int numbits);
uint32_t xint_rshift(xint_t y, const xint_t x, int numbits);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
