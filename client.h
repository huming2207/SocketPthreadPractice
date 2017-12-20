//
// Created by hu on 12/20/17.
//

#ifndef SOCKETPTHREADPRACTISE_CLIENT_H
#define SOCKETPTHREADPRACTISE_CLIENT_H


void client_init(char *ip_addr, uint16_t port);
void * user_thread(void * args);
void * connection_thread(void * args);
void run_ping(int socket_fd);
void client_send_buffer(int socket_fd, char * buffer);
void client_recv_buffer(int socket_fd);
void get_input();

char * recv_buffer;
char * send_buffer;
char * input_buffer;
char input_char;

int send_buffer_count;


pthread_t input_pt;

#define SOCKS_CLIENT_PORT   12000
#define STRING_BUFFER_SIZE      3072
#define STRING_EXTRA_SPACES 3


#endif //SOCKETPTHREADPRACTISE_CLIENT_H
