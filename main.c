#include "client.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{

  char *ptr;
  uint16_t port_num;

  if (argc < 3) {
    printf("Usage: SocketPthreadPractise IpAddress Port");
    return EXIT_FAILURE;
  }

  port_num = (uint16_t) strtol(argv[2], &ptr, 10);
  client_init(argv[1], port_num);

  return EXIT_SUCCESS;
}