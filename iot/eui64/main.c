#include <stdio.h>
#include "eui64.h"

int main ( int argc, char *argv[] )
{
    char eui64[EUI64_STRING_SIZE];

    eui64_toString(eui64, sizeof(eui64));
    printf("The proxy device ID is %s\n", eui64);

    return 0;
}
