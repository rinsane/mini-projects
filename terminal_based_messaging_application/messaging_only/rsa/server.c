#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define MAX 4096
#define PORT 8000
#define KEY_SERVER_PORT 9000
#define SA struct sockaddr
#define RECEIVER "apple"

int connfd;
RSA *rsa;
RSA *private_rsa;

RSA *get_public_key() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr = {AF_INET, htons(KEY_SERVER_PORT), inet_addr("127.0.0.1")};
    connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
    write(sockfd, RECEIVER, strlen(RECEIVER));

    char buffer[MAX] = {0};
    int bytes = read(sockfd, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    close(sockfd);

    BIO *bio = BIO_new_mem_buf(buffer, bytes);
    RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);

    if (!rsa) {
        fprintf(stderr, "Failed to load public key for %s\n", RECEIVER);
    }
    return rsa;
}

RSA *get_private_key() {
    FILE *fp = fopen("banana_private.pem", "r");
    if (!fp) return NULL;
    RSA *rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    return rsa;
}

void *sendMessages(void *arg) {
    char message[MAX];
    unsigned char encrypted[MAX];
    while (1) {
        printf("Banana: ");
        fgets(message, MAX, stdin);
        int enc_len = RSA_public_encrypt(strlen(message) + 1, (unsigned char*)message, encrypted, rsa, RSA_PKCS1_OAEP_PADDING);
        write(connfd, encrypted, enc_len);
        if (strncmp(message, "exit", 4) == 0) break;
    }
    pthread_exit(NULL);
}

void *receiveMessages(void *arg) {
    unsigned char buffer[MAX];
    char decrypted[MAX];
    while (1) {
        int bytes = read(connfd, buffer, sizeof(buffer));
        if (bytes <= 0) {
            printf("\nConnection closed. Exiting...\n");
            close(connfd);
            exit(0);
        }

        int dec_len = RSA_private_decrypt(bytes, buffer, (unsigned char*)decrypted, private_rsa, RSA_PKCS1_OAEP_PADDING);
        if (dec_len == -1) {
            fprintf(stderr, "Decryption failed!\n");
            continue;
        }

        decrypted[dec_len] = '\0';
        printf("\nApple: %s", decrypted);
    }
    pthread_exit(NULL);
}

int main() {
    rsa = get_public_key();
    private_rsa = get_private_key();

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr = {AF_INET, htons(PORT), INADDR_ANY};
    bind(sockfd, (SA*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);

    connfd = accept(sockfd, NULL, NULL);

    pthread_t sendThread, receiveThread;
    pthread_create(&sendThread, NULL, sendMessages, NULL);
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    return 0;
}
