
#include "cvt_vectors.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void cvt_key_pair_file(void);
void cvt_pkv_file(void);
void cvt_sig_gen_file(void);
void cvt_sig_ver_file(void);

#define BUFSZ (2000)

void cvt_vectors(void)
{
    cvt_key_pair_file();
    cvt_pkv_file();
    cvt_sig_gen_file();
    cvt_sig_ver_file();
}

void cvt_key_pair_file(void)
{
    char buf[BUFSZ];
    getcwd(buf, BUFSZ-1);
    int in_progress = 0;
    int in_progress_2 = 0;
    printf("Loading test vectors from %s\n", buf);
    FILE *fin = fopen("vectors/186-4ecdsatestvectors/KeyPair.rsp", "r");
    FILE *fout = fopen("nist_vectors_key_pair.c", "w");
    fprintf(fout, "\n#include \"nist_vectors.h\"\n\n");
    
    while (1)
    {
        if (fgets(buf, BUFSZ-1, fin) == NULL)
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

void cvt_pkv_file(void)
{
    char buf[BUFSZ];
    getcwd(buf, BUFSZ-1);
    int in_progress = 0;
    int in_progress_2 = 0;
    printf("Loading test vectors from %s\n", buf);
    FILE *fin = fopen("vectors/186-4ecdsatestvectors/PKV.rsp", "r");
    FILE *fout = fopen("nist_vectors_pkv.c", "w");
    fprintf(fout, "\n#include \"nist_vectors.h\"\n\n");
    
    while (1)
    {
        if (fgets(buf, BUFSZ-1, fin) == NULL)
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
            fprintf(fout, "struct pkv pkv_%s[] =\n{\n", name);
            in_progress = 1;
        }
        else
        {
            if (*p == 'N')
            {
                continue;
            }
            //printf("%s\n", p);
            if (*p == 'Q' && *(p+1) == 'x')
            {
                if (in_progress_2)
                {
                    fprintf(fout, "    },\n");
                }
                in_progress_2 = 1;
                fprintf(fout, "    {\n");
            }
            if (*p == 'Q')
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
            else if (*p == 'R')
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
                while (*end != '\n' && *end != '\r')
                {
                    ++end;
                }
                *end = '\0';
                fprintf(fout, "        \"%s\",\n", p);
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

void cvt_sig_gen_file(void)
{
    char buf[BUFSZ];
    getcwd(buf, BUFSZ-1);
    int in_progress = 0;
    int in_progress_2 = 0;
    printf("Loading test vectors from %s\n", buf);
    FILE *fin = fopen("vectors/186-4ecdsatestvectors/SigGen.txt", "r");
    FILE *fout = fopen("nist_vectors_sig_gen.c", "w");
    fprintf(fout, "\n#include \"nist_vectors.h\"\n\n");
    
    while (1)
    {
        if (fgets(buf, BUFSZ-1, fin) == NULL)
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
                if (name[i] == '-' || name[i] == ',')
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
            fprintf(fout, "struct sig_gen sig_gen_%s[] =\n{\n", name);
            in_progress = 1;
        }
        else
        {
            if (*p == 'N')
            {
                continue;
            }
            //printf("%s\n", p);
            if ((*p == 'M' && *(p+1) == 's'))
            {
                if (in_progress_2)
                {
                    fprintf(fout, "    },\n");
                }
                in_progress_2 = 1;
                fprintf(fout, "    {\n");
            }
            if (   (*p == 'M' && *(p+1) == 's')
                || (*p == 'd')
                || (*p == 'Q' && *(p+1) == 'x')
                || (*p == 'Q' && *(p+1) == 'y')
                || (*p == 'k')
                || (*p == 'R')
                || (*p == 'S')
                )
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

void cvt_sig_ver_file(void)
{
    char buf[BUFSZ];
    getcwd(buf, BUFSZ-1);
    int in_progress = 0;
    int in_progress_2 = 0;
    printf("Loading test vectors from %s\n", buf);
    FILE *fin = fopen("vectors/186-4ecdsatestvectors/SigVer.rsp", "r");
    FILE *fout = fopen("nist_vectors_sig_ver.c", "w");
    fprintf(fout, "\n#include \"nist_vectors.h\"\n\n");
    
    while (1)
    {
        if (fgets(buf, BUFSZ-1, fin) == NULL)
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
                if (name[i] == '-' || name[i] == ',')
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
            fprintf(fout, "struct sig_ver sig_ver_%s[] =\n{\n", name);
            in_progress = 1;
        }
        else
        {
            if (*p == 'N')
            {
                continue;
            }
            //printf("%s\n", p);
            if ((*p == 'M' && *(p+1) == 's'))
            {
                if (in_progress_2)
                {
                    fprintf(fout, "    },\n");
                }
                in_progress_2 = 1;
                fprintf(fout, "    {\n");
            }
            if (   (*p == 'M' && *(p+1) == 's')
                || (*p == 'd')
                || (*p == 'Q' && *(p+1) == 'x')
                || (*p == 'Q' && *(p+1) == 'y')
                || (*p == 'k')
                || (*p == 'R' && *(p+1) != 'e')
                || (*p == 'S')
                )
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
            else if (*p == 'R' && *(p+1) == 'e')
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
                while (*end != '\n' && *end != '\r')
                {
                    ++end;
                }
                *end = '\0';
                fprintf(fout, "        \"%s\",\n", p);
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
