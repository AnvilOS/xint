
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

void xint_init(xint_t u, int capacity);
void xint_delete(xint_t u);

void xint_add(xint_t w, xint_t u, xint_t v);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XINT_H
