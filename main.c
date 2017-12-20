#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "client.h"

int main (int argc, char *argv[])
{
    char * ptr;

    if(argc < 3) {
        printf("Usage: SocketPthreadPractise IpAddress Port");
        return EXIT_FAILURE;
    }

    uint16_t port_num = (uint16_t)strtol(argv[2], &ptr, 10);
    client_init(argv[1], port_num);
}