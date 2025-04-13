#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h> // for threading
#define MAX 80
#define PORT 8000
#define SA struct sockaddr

// header for aes
#include "aes.h"
#define ECB 1

// function to read key from file
int read_key(uint8_t *key) {
    FILE *key_file = fopen("aes.key", "rb");
    size_t bytes_read = fread(key, 1, 16, key_file);
    fclose(key_file);
}
// buffer to store the key read from aes.key
uint8_t key[16];
// initialize AES context
struct AES_ctx ctx;

// shared file descriptor for communication
int sockfd;

// function to handle receiving messages from the server
void *receiveMessages(void *arg) {
    char buff[MAX];
    for (;;) {
        bzero(buff, MAX); // clear the buffer
        ssize_t bytes_read = read(sockfd, buff, sizeof(buff)); // read message from the server

        // Detect socket closure
        if (bytes_read <= 0) {
            printf("\nConnection closed by peer. Exiting...\n");
            close(sockfd);
            exit(0);
        }

        // decryption
        AES_ECB_decrypt(&ctx, buff);

        printf("\nServer: %s", buff); // display the message
        if (strncmp("exit", buff, 4) == 0) { // check if the server wants to exit
            printf("Server disconnected. Exiting...\n");
            close(sockfd);
            exit(0);
        }
    }
    return NULL;
}

// function to handle sending messages to the server
void *sendMessages(void *arg) {
    char buff[MAX];
    for (;;) {
        bzero(buff, MAX);
        printf("Client: ");
        fgets(buff, MAX, stdin);

        // encryption
        AES_ECB_encrypt(&ctx, buff);

        write(sockfd, buff, strlen(buff));

        if (strncmp("exit", buff, 4) == 0) {
            printf("Client Exit...\n");
            close(sockfd);
            pthread_exit(NULL); // Exit thread instead of whole program
        }
    }
    return NULL;
}

int main() {
    read_key(key);
    AES_init_ctx(&ctx, key);

    struct sockaddr_in servaddr;
    pthread_t sendThread, receiveThread;

    // socket creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));

    // assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect to the server
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else
        printf("Connected to the server..\n");

    // create threads for sending and receiving messages
    pthread_create(&sendThread, NULL, sendMessages, NULL);
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    // join threads
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    // close the client socket
    close(sockfd);
    return 0;
}

