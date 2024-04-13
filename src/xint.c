
#include "xint.h"

#include <stdlib.h>

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
