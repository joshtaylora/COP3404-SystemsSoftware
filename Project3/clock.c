#include <stdio.h>
#include <stdlib.h>

int main()
{
    int timer = 0x811274;
    int sbit = 0x800000;

    if ((timer&sbit) > 0x7FFFFF)
    {
        printf("timer val in 24hr: %X\n", timer);
    }
    else if ((timer&sbit) < 0x800000)
    {
        printf("timer val in 12hr: %X\n", timer);
    }
    else
    {
        printf("ERROR\n");
    }
    
}