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

void run_ping();

void client_send_buffer(char *buffer);

void client_recv_buffer();

char *recv_buffer;

char *send_buffer;

char *input_buffer;

char input_char;

static int send_buffer_count;

pthread_t input_pt;

#define SOCKS_CLIENT_PORT   12000
#define STRING_BUFFER_SIZE      3072

#endif //SOCKETPTHREADPRACTISE_CLIENT_H
