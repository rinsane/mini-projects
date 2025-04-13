#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define MAX 1024
#define PORT 8000
#define FILE_PORT 8001
#define SA struct sockaddr
#define CHUNK_SIZE 10  // sending files in chunks of 100 bytes

int msg_sockfd, file_sockfd, msg_connfd, file_connfd;
char last_received_file[MAX] = "recv_server.txt";

// function to receive messages and files
void *receiveMessages(void *arg) {
    char buff[MAX];
    while (1) {
        bzero(buff, MAX);
        int bytes_read = read(msg_connfd, buff, sizeof(buff));

        if (bytes_read <= 0) {
            printf("\nClient disconnected unexpectedly. Exiting...\n");
            close(msg_connfd);
            close(file_connfd);
            exit(0);
        }

        if (strncmp(buff, "!filesend", 9) == 0) {
            printf("Client is sharing a file...\n");

            FILE *fp = fopen(last_received_file, "w");
            if (!fp) {
                perror("Error opening file");
                continue;
            }

            char file_data[CHUNK_SIZE];
            int recv_bytes;
            while ((recv_bytes = recv(file_connfd, file_data, CHUNK_SIZE, 0)) > 0) {
                fwrite(file_data, 1, recv_bytes, fp);
                printf("Recieved bytes: %d\n", recv_bytes);
                if (recv_bytes < CHUNK_SIZE) break;
            }

            fclose(fp);
            printf("File transfer successful.\n");
            continue;
        }

        printf("\nClient: %s", buff);
        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client disconnected. Exiting...\n");
            close(msg_connfd);
            close(file_connfd);
            exit(0);
        }
    }
    return NULL;
}

// function to send messages
void *sendMessages(void *arg) {
    char buff[MAX];
    while (1) {
        bzero(buff, MAX);
        printf("Server: ");
        fgets(buff, MAX, stdin);

        if (strncmp(buff, "!filesend", 9) == 0) {
            char filename[MAX];
            if (sscanf(buff, "!filesend %s", filename) != 1) {
                printf("Invalid command format. Use: !filesend <filename>\n");
                continue;
            }

            FILE *fp = fopen(filename, "r");
            if (!fp) {
                printf("File not found.\n");
                continue;
            }

            write(msg_connfd, buff, strlen(buff));

            char file_data[CHUNK_SIZE];
            int read_bytes;
            while ((read_bytes = fread(file_data, 1, CHUNK_SIZE, fp)) > 0) {
                send(file_connfd, file_data, read_bytes, 0);
                printf("Sent bytes: %d\n", read_bytes);
            }
            fclose(fp);

            continue;
        }

        if (strncmp(buff, "!fileshow", 9) == 0) {
            printf("\nDisplaying last received file:\n");
            char command[MAX] = "cat ";
            strcat(command, last_received_file);
            system(command);
            continue;
        }

        if (write(msg_connfd, buff, strlen(buff)) <= 0) {
            printf("\nClient disconnected unexpectedly. Exiting...\n");
            exit(0);
        }

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Server Exit...\n");
            close(msg_connfd);
            close(file_connfd);
            exit(0);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in servaddr, cli;
    pthread_t sendThread, receiveThread;

    msg_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    file_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    bind(msg_sockfd, (SA *)&servaddr, sizeof(servaddr));
    listen(msg_sockfd, 5);

    servaddr.sin_port = htons(FILE_PORT);
    bind(file_sockfd, (SA *)&servaddr, sizeof(servaddr));
    listen(file_sockfd, 5);

    printf("Awaiting connection...\n");

    socklen_t len = sizeof(cli);
    msg_connfd = accept(msg_sockfd, (SA *)&cli, &len);
    file_connfd = accept(file_sockfd, (SA *)&cli, &len);

    printf("Client connected.\n");

    pthread_create(&sendThread, NULL, sendMessages, NULL);
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    close(msg_sockfd);
    close(file_sockfd);
    return 0;
}
