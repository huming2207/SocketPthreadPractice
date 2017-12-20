//
// Created by hu on 12/20/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zconf.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <stdbool.h>

#include "client.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Initialise the client
 * @param ip_addr - IP address of the server in string, e.g. 127.0.0.1
 * @param port - Port of the server
 */
void client_init(char *ip_addr, uint16_t port)
{
    pthread_t user_pt, conn_pt;
    struct sockaddr_in sock_addr;
    int socket_fd;
    int connect_ret;

    // Register socket
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(socket_fd < 0) {
        fprintf(stderr, "Socket: register failed, reason: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Wipe and fill the sock_addr
    memset(&sock_addr, 0, sizeof(struct sockaddr_in));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(ip_addr);
    sock_addr.sin_port = htons(port);

    // Connect to server
    connect_ret = connect(socket_fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in));

    if(connect_ret == -1) {
        fprintf(stderr, "Connect: connection failed, reason: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        printf("[DEBUG] Init: connected!");
    }

    // Initialise string buffer
    send_buffer = malloc(STRING_BUFFER_SIZE);
    recv_buffer = malloc(STRING_BUFFER_SIZE);
    input_buffer = malloc(STRING_BUFFER_SIZE);

    if(!send_buffer) {
        fprintf(stdin, "Init: cannot initialise the send buffer, reason: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(!recv_buffer) {
        fprintf(stdin, "Init: cannot initialise the receive buffer, reason: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(!input_buffer) {
        fprintf(stdin, "Init: cannot initialise the input buffer, reason: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set send buffer
    send_buffer_count = 0;

    // Start pthread threads
    pthread_create(&conn_pt, NULL, connection_thread, &socket_fd);
    pthread_create(&user_pt, NULL, user_thread, NULL);

    pthread_join(user_pt, NULL);
    pthread_join(conn_pt, NULL);

    // Might not reach here in this app...
    close(socket_fd);
}

void * user_thread(void * args)
{
    printf("UserThread: Initialisation finished, you can now input something.\n");
    while (true) {

        // Lock the critical section
        pthread_mutex_lock(&mutex);

        input_char = (char)fgetc(stdin);

        if(input_char == 's') {

            // Wipe the send buffer, then move the data from input buffer to send buffer
            printf("Changing send_buffer to: %s", input_buffer);
            memset(send_buffer, '\0', STRING_BUFFER_SIZE);
            strcpy(send_buffer, input_buffer);
            memset(input_buffer, '\0', STRING_BUFFER_SIZE);

        } else if (input_char == 'r') {

            printf("Received buffer: %s\n", recv_buffer);

        } else {
            strcat(input_buffer, &input_char); // Stores user input for other keys
        }

        pthread_mutex_unlock(&mutex);
    }


}

void * connection_thread(void * args)
{
    int socket_fd;

    // Copy the argument
    socket_fd = *(int *)args;

    while(true) {

        // Lock the critical section, if not possible, then forget about it lol...
        pthread_mutex_trylock(&mutex);
        run_ping(socket_fd);
        usleep(500);
        pthread_mutex_unlock(&mutex);
    }
}

void run_ping(int socket_fd)
{
    client_send_buffer(socket_fd, send_buffer);
    client_recv_buffer(socket_fd);
}

void client_send_buffer(int socket_fd, char * buffer)
{
    ssize_t send_size;

    if(!buffer) {
        fprintf(stderr, "Send: send buffer is not correctly initialised, data will not send");
        return;
    }

    // Send some junk data
    send_size = send(socket_fd, buffer, STRING_BUFFER_SIZE, 0);

    // Detect if something wrong
    if(send_size < 0){
        fprintf(stderr, "Send: data receive failed: %s\n", strerror(errno));
        return;
    } else if(send_size == 0) {
        fprintf(stderr, "Send: client disconnected: %s\n", strerror(errno));
        return;
    }

}

void client_recv_buffer(int socket_fd)
{
    ssize_t recv_size;
    memset(recv_buffer, '\0', STRING_BUFFER_SIZE);
    recv_size = recv(socket_fd, recv_buffer, STRING_BUFFER_SIZE, 0);

    // Check the return size
    if(recv_size < 0) {
        fprintf(stdin, "recv: corrupted data received, reason: %s\n", strerror(errno));
    } else if(recv_size == 0 ) {
        printf("recv: server disconnected!");
    }

}