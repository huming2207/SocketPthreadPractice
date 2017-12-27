//
// Created by hu on 12/20/17.
//

#ifndef SOCKETPTHREADPRACTISE_CLIENT_H
#define SOCKETPTHREADPRACTISE_CLIENT_H

#include <stdint.h>
#include <zconf.h>


void client_init(char *ip_addr, uint16_t port);

void *user_thread(void *args);

void *connection_thread(void *args);

void client_send_buffer(char *buffer);

void client_recv_buffer();

char *recv_buffer;

char *send_buffer;

char *input_buffer;

char input_char;

static unsigned long send_counter;

#define STRING_BUFFER_SIZE      3072

#endif //SOCKETPTHREADPRACTISE_CLIENT_H
