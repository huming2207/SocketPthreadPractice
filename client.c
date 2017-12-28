//
// Created by hu on 12/20/17.
//
#include "client.h"
#include "data_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>

#include <netinet/in.h>
#include <arpa/inet.h>


/**
 * The mutex lock to control the data in two threads
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t user_pt, conn_pt, save_pt;

int socket_fd;

/**
 * Initialise the client with procedure:
 * socket ==> connect ==> recv/send (in pthread threads) ==> close (if possible)
 *
 * @param ip_addr - IP address of the server in string, e.g. 127.0.0.1
 * @param port - Port of the server
 */
void client_init(char *ip_addr, uint16_t port)
{

  struct sockaddr_in sock_addr;
  int connect_ret;
  uint64_t time_taken;

  // Register socket
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    fprintf(stderr, "Socket: register failed, reason: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Wipe and fill the sock_addr
  memset(&sock_addr, 0, sizeof(struct sockaddr_in));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = inet_addr(ip_addr);
  sock_addr.sin_port = htons(port);

  // Connect to server
  connect_ret = connect(socket_fd, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr_in));

  if (connect_ret == -1) {
    fprintf(stderr, "Connect: connection failed, reason: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  } else {
    printf("[DEBUG] Init: connected!\n");
  }

  // Start pthread threads
  pthread_create(&conn_pt, NULL, connection_thread, &socket_fd);
  pthread_create(&user_pt, NULL, user_thread, NULL);
  pthread_create(&save_pt, NULL, save_thread, NULL);

  pthread_join(user_pt, NULL);
  pthread_join(conn_pt, NULL);
  pthread_join(save_pt, NULL);

  // Might not reach here in this app...
  close(socket_fd);
}

/**
 * User thread, detect user input
 * @param args - Nothing, just a place holder
 * @return Nothing, just a place holder
 */
void *user_thread(void *args)
{

  printf("UserThread: Initialisation finished, you can now input something.\n");
  while (true) {

    input_char = (char) fgetc(stdin);

    if (input_char == 's') {

      if(!send_buffer) {
        fprintf(stderr, "buffer: socket buffer is not yet initialised!");
      } else {
        // Wipe the send send_buffer, then move the data from input send_buffer to send send_buffer
        // Lock the critical section
        pthread_mutex_lock(&mutex);

        printf("Changing send_buffer to: %s\n", input_buffer);
        memset(send_buffer, '\0', STRING_BUFFER_SIZE);
        strcpy(send_buffer, input_buffer);

        // Critical section ends here
        pthread_mutex_unlock(&mutex);

        memset(input_buffer, '\0', STRING_BUFFER_SIZE);
      }

    } else if (input_char == 'r') {

      printf("Received send_buffer: %s, already sent %llu times, saved %llu times, enqueued %llu items!\n",
             recv_buffer,
             send_counter,
             save_counter, data_node_count);

    } else if (input_char != '\n'){
      strcat(input_buffer, &input_char); // Stores user input for other keys, do not put new line chars in
    }
  }
}

/**
 * Connection thread, deal with communication
 * @param args - Socket file descriptor
 * @return Nothing
 */
void *connection_thread(void *args)
{
  ssize_t send_size;
  ssize_t recv_size;

  send_buffer = calloc(STRING_BUFFER_SIZE, sizeof(char));
  recv_buffer = calloc(STRING_BUFFER_SIZE, sizeof(char));

  if (!send_buffer) {
    fprintf(stderr, "Send: send send_buffer is not correctly initialised, data will not send");
    pthread_exit(NULL);
  } else {
    strcpy(send_buffer, "ping"); // Copy some initial placeholder to the buffer
  }

  while (true) {

    // Lock the critical section, if not possible, then forget about it lol...
    pthread_mutex_trylock(&mutex);

    // Send some junk data first
    send_size = send(socket_fd, send_buffer, STRING_BUFFER_SIZE, 0);
    pthread_mutex_unlock(&mutex);

    // Detect if something wrong
    if (send_size < 0) {
      fprintf(stderr, "Send: data receive failed: %s\n", strerror(errno));
      pthread_exit(NULL);
    } else if (send_size == 0) {
      fprintf(stderr, "Send: client disconnected!");
      pthread_exit(NULL);
    }

    // Start to receive
    recv_size = recv(socket_fd, recv_buffer, STRING_BUFFER_SIZE, 0);

    // Enqueue buffer to data queue/list after received
    if(!data_list_enqueue(recv_buffer)) {
      fprintf(stderr, "Queue: data enqueue failed.\n");
    }

    // Check the return size
    if (recv_size < 0) {
      fprintf(stdin, "Recv: corrupted data received, reason: %s\n", strerror(errno));
    } else if (recv_size == 0) {
      printf("Recv: server disconnected!");
    }

    send_counter += 1;
  }
}

void *save_thread(void *args)
{
  while(true) {

    FILE *file;
    char *buffer;

    // Shut up the compiler
    buffer = NULL;

    file = fopen(SOCKS_LOG_FILE, "a+");
    if(!file) {
      fprintf(stderr, "File: open file failed, reason: %s\n", strerror(errno));
      pthread_exit(NULL);
    }

    // Write the buffer list to file and clear it up
    while (data_node_count > 0) {

      // Copy the buffer
      buffer = data_list_dequeue();

      if (!buffer) {
        printf("File: No item left in the queue!\n");
      } else if(strlen(buffer) > 0) {
        fprintf(file, "%s\n", buffer);
        save_counter += 1;
      }

    }

    fflush(file);
    fclose(file);
    free(buffer);
    sleep(5);
  }

}