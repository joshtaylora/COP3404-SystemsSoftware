#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

#define SymbolDef "([A-Z]{6})\\s+([A-Z]+)\\s+(\\w)$"

#define lineReg "([A-Z]{6})"

int main(int argc, char *argv[])
{
    FILE *inputFile;
    char line[1024];
    int retval = 0;
    regex_t re;

    regmatch_t rm[4];
    printf ("--RIGHT BEFORE regcomp CALLED\n");
    
    if (regcomp(&re, lineReg, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex '%s'\n", lineReg);
        return EXIT_FAILURE;
    }

    inputFile = fopen(argv[1], "r");
    if (inputFile == 0)
    {
        fprintf(stderr, "Failed to open file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    while( ( fgets( line, 1024, inputFile ) ) != NULL )
    {
        line[strlen(line) - 1] = '\0';

        if ( ( retval = regexec(&re, line, 4, rm, 0) ) == 0 )
        {
            printf("Line: %.*s\n", (int)(rm[0].rm_eo - rm[0].rm_so), line + rm[0].rm_so);

            char *start = line + rm[1].rm_so;
            char *end = line + rm[1].rm_eo;

            while (start < end)
            {
                size_t len = strcspn(start, " ");
                
                if ( start + len > end )
                    len = end - start;
                printf("Token: %.*s\n", (int)len, start);
                start += len;
                start += strspn(start, " ");
            }
        }
    }
    return 1;

}

