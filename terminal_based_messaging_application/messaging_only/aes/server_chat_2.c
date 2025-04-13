#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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
int connfd;

// function to handle receiving messages from the client
void *receiveMessages(void *arg) {
    char buff[MAX];
    for (;;) {
        bzero(buff, MAX); // clear the buffer
        ssize_t bytes_read = read(connfd, buff, sizeof(buff)); // read message from the client

        // Detect socket closure
        if (bytes_read <= 0) {
            printf("\nConnection closed by peer. Exiting...\n");
            close(connfd);
            exit(0);
        }

        // decryption
        AES_ECB_decrypt(&ctx, buff);

        printf("\nClient: %s", buff); // display the message
        if (strncmp("exit", buff, 4) == 0) { // check if the client wants to exit
            printf("Client disconnected. Exiting...\n");
            close(connfd);
            exit(0);
        }
    }
    return NULL;
}

// function to handle sending messages to the client
void *sendMessages(void *arg) {
    char buff[MAX];
    for (;;) {
        bzero(buff, MAX);
        printf("Server: ");
        fgets(buff, MAX, stdin);

        // encryption
        AES_ECB_encrypt(&ctx, buff);

        write(connfd, buff, strlen(buff));

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            close(connfd);
            pthread_exit(NULL); // Exit thread instead of whole program
        }
    }
    return NULL;
}

int main() {
    read_key(key);
    AES_init_ctx(&ctx, key);

    int sockfd, len;
    struct sockaddr_in servaddr, cli;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // bind the socket
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } else
        printf("Socket successfully binded..\n");

    // start listening
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        printf("Server listening..\n");
    len = sizeof(cli);

    // accept a client connection
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(0);
    } else
        printf("Server accepted the client...\n");

    // create threads for sending and receiving messages
    pthread_create(&sendThread, NULL, sendMessages, NULL);
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    // join threads
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    // close the server socket
    close(sockfd);
    return 0;
}

