
#include "xint.h"

#include <stdio.h>

int main(int argc, const char * argv[])
{
    xword_t xdata[] = { 0x1, 0x2, 0x3 };
    xint_t X = { 3, 3, xdata };
    
    xword_t ydata[] = { 0x4, 0x5, 0x6 };
    xint_t Y = { 3, 3, ydata };

    xword_t zdata[4];
    xint_t Z = { 4, 3, zdata };
    
    xint_add(Z, X, Y);

    printf("%d %d %d\n", zdata[2], zdata[1], zdata[0]);
        
    return 0;
}
