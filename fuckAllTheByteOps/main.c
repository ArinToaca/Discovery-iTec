#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

int main()
{
    printf("Hello world!\n");

    uint32_t sent = 0x20DF8877;

    int i;
    printf("Bits:");
    for(i=31;i>=0;i--)
    {
//    sendBit(562);
    if((sent & (0x1 << 31))!=0)
    {
        printf("1");
    }
    else
    {
        printf("0");
    }

    sent <<=1;
    }

    return 0;
}
