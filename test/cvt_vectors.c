
#include "cvt_vectors.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void cvt_key_pair_file(void);

void cvt_vectors(void)
{
    cvt_key_pair_file();
}

void cvt_key_pair_file(void)
{
    char buf[1000];
    getcwd(buf, 999);
    int in_progress = 0;
    int in_progress_2 = 0;
    printf("Loading test vectors from %s\n", buf);
    FILE *fin = fopen("vectors/186-4ecdsatestvectors/KeyPair.rsp", "r");
    FILE *fout = fopen("nist_vectors_key_pair.c", "w");
    fprintf(fout, "\n#include \"nist_vectors.h\"\n\n");
    
    while (1)
    {
        if (fgets(buf, 999, fin) == NULL)
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
        //printf("%s", p);
        if (*p == '\0')
        {
            continue;
        }
        else if (*p == '[')
        {
            char name[100];
            char *tok = strtok(p, "[]");
            strcpy(name, tok);
            if (strncmp(name, "B.4", 3) == 0)
            {
                continue;
            }
            for (int i=0; i<strlen(name); ++i)
            {
                name[i] = tolower(name[i]);
                if (name[i] == '-')
                {
                    name[i] = '_';
                }
            }
            //printf("%s\n", name);
            if (in_progress_2)
            {
                fprintf(fout, "    },\n");
                in_progress_2 = 0;
            }
            if (in_progress)
            {
                fprintf(fout, "%s\n", "};\n");
                in_progress = 0;
            }
            fprintf(fout, "struct key_pair key_pairs_%s[] =\n{\n", name);
            in_progress = 1;
        }
        else
        {
            if (*p == 'N')
            {
                continue;
            }
            //printf("%s\n", p);
            if (*p == 'd')
            {
                if (in_progress_2)
                {
                    fprintf(fout, "    },\n");
                }
                in_progress_2 = 1;
                fprintf(fout, "    {\n");
            }
            if (*p == 'd' || *p == 'Q')
            {
                ++p;
                while (*p != '=')
                {
                    ++p;
                }
                ++p;
                while (isspace(*p))
                {
                    ++p;
                }
                char *end = p;
                while (isxdigit(*end))
                {
                    ++end;
                }
                *end = '\0';
                fprintf(fout, "        \"0x%s\",\n", p);
            }
        }
    }
    if (in_progress_2)
    {
        fprintf(fout, "    },\n");
    }
    if (in_progress)
    {
        fprintf(fout, "%s\n", "};\n");
    }
    fclose(fin);
    fclose(fout);
}
