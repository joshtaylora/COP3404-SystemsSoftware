#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/*
void strcpyTest(char *s, char *t)
{
    while ((*s++ = *t++) != '\0')
        ;
}
*/

void strcpyTest(char *s,char *t)
{
    int i;
    printf("Before copy\n\tx: %s\n\ty: %s\n", s, t);
    i = 0;
    while ((*s = *t) != '\0') {
        printf("s: %d\n", *s);
        printf("  t: %d\n", *t);
        s++;
        t++;
    }
}

int main() {
    char x[20] = "This is a test";
    char y[20];
    strcpyTest(y, x);
    
    printf("After copy\n\tx: %s\n\ty: %s\n", x,y);
    return 0;
}
