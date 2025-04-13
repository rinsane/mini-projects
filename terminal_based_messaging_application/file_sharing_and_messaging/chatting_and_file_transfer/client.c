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

int msg_sockfd, file_sockfd;
char last_received_file[MAX] = "recv_client.txt";

// function to receive messages and files
void *receiveMessages(void *arg) {
    char buff[MAX];
    while (1) {
        bzero(buff, MAX);
        int bytes_read = read(msg_sockfd, buff, sizeof(buff));

        if (bytes_read <= 0) {
            printf("\nServer disconnected unexpectedly. Exiting...\n");
            close(msg_sockfd);
            close(file_sockfd);
            exit(0);
        }

        if (strncmp(buff, "!filesend", 9) == 0) {
            printf("Server is sharing a file...\n");

            FILE *fp = fopen(last_received_file, "w");
            if (!fp) {
                perror("Error opening file");
                continue;
            }

            char file_data[CHUNK_SIZE];
            int recv_bytes;
            while ((recv_bytes = recv(file_sockfd, file_data, CHUNK_SIZE, 0)) > 0) {
                fwrite(file_data, 1, recv_bytes, fp);
                printf("Recieved bytes: %d\n", recv_bytes);
                if (recv_bytes < CHUNK_SIZE) break;
            }

            fclose(fp);
            printf("File transfer successful.\n");
            continue;
        }

        printf("\nServer: %s", buff);
        if (strncmp(buff, "exit", 4) == 0) {
            printf("Server disconnected. Exiting...\n");
            close(msg_sockfd);
            close(file_sockfd);
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
        printf("Client: ");
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

            write(msg_sockfd, buff, strlen(buff));

            char file_data[CHUNK_SIZE];
            int read_bytes;
            while ((read_bytes = fread(file_data, 1, CHUNK_SIZE, fp)) > 0) {
                send(file_sockfd, file_data, read_bytes, 0);
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

        if (write(msg_sockfd, buff, strlen(buff)) <= 0) {
            printf("\nServer disconnected unexpectedly. Exiting...\n");
            exit(0);
        }

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            close(msg_sockfd);
            close(file_sockfd);
            exit(0);
        }
    }
    return NULL;
}


int main() {
    struct sockaddr_in servaddr;
    pthread_t sendThread, receiveThread;

    msg_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    file_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    connect(msg_sockfd, (SA *)&servaddr, sizeof(servaddr));
    servaddr.sin_port = htons(FILE_PORT);
    connect(file_sockfd, (SA *)&servaddr, sizeof(servaddr));

    printf("Connected to the server..\n");

    pthread_create(&sendThread, NULL, sendMessages, NULL);
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    close(msg_sockfd);
    close(file_sockfd);
    return 0;
}
