//
// Created by hu on 12/20/17.
//

#ifndef SOCKETPTHREADPRACTISE_CLIENT_H
#define SOCKETPTHREADPRACTISE_CLIENT_H

#include <stdint.h>
#include <zconf.h>

#define STRING_BUFFER_SIZE      3072

void client_init(char *ip_addr, uint16_t port);
void *user_thread(void *args);
void *connection_thread(void *args);
void *save_thread(void *args);

char input_buffer[STRING_BUFFER_SIZE];
char *send_buffer;
char *recv_buffer;
char input_char;

static unsigned long long send_counter;
static unsigned long long save_counter;


#endif //SOCKETPTHREADPRACTISE_CLIENT_H
