
#include "cvt_vectors.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void cvt_vectors(void)
{
    char buf[1000];
    getcwd(buf, 999);
    printf("Loading test vectors from %s\n", buf);
    FILE *f = fopen("KeyPair.rsp", "r");
    
    while (1)
    {
        if (fgets(buf, 999, f) == NULL)
        {
            break;
        }
        char *p = buf;
        while (isspace(*p))
        {
            ++p;
        }
        if (*p == '#')
        {
            continue;
        }
        printf("%s", p);
        
        if (*p == '[')
        {
            char name[100];
            char *tok = strtok(p, "[]");
            strcpy(name, tok);
            for (int i=0; i<strlen(name); ++i)
            {
                name[i] = tolower(name[i]);
                if (name[i] == '-')
                {
                    name[i] = '_';
                }
            }
            printf("%s", name);
        }

    }
    
}
