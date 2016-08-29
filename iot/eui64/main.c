#include <stdio.h>
#include <string.h>
#include "eui64.h"

char* argDeviceType = NULL;
char *argEui64Bytes = NULL;

int main ( int argc, char *argv[] )
{
    char eui64[EUI64_STRING_SIZE];

    if ( argc > 1 && argv[1] != 0 )
    {
	argEui64Bytes = strdup(argv[1]);
    }

    if ( argc > 2 && argv[2] != 0 )
    {
	argDeviceType = strdup(argv[2]);
    }

    eui64_toString(eui64, sizeof(eui64));
    printf("The proxy device ID is %s\n", eui64);

    return 0;
}
