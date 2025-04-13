#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define PORT 9000
#define MAX 4096
#define SA struct sockaddr

// Load public key from file (fix: use PEM_read_RSA_PUBKEY)
RSA *load_public_key(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open public key file");
        return NULL;
    }

    RSA *rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);

    if (!rsa) {
        fprintf(stderr, "Error loading public key from %s\n", filename);
    }
    return rsa;
}

void handle_client(int connfd) {
    char buffer[MAX] = {0};
    int bytes_read = read(connfd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        close(connfd);
        return;
    }
    buffer[bytes_read] = '\0';

    printf("Request received for: %s\n", buffer);

    RSA *rsa = NULL;
    if (strcmp(buffer, "banana") == 0) {
        rsa = load_public_key("banana_public.pem");
    } else if (strcmp(buffer, "apple") == 0) {
        rsa = load_public_key("apple_public.pem");
    }

    if (rsa) {
        BIO *bio = BIO_new(BIO_s_mem());
        PEM_write_bio_RSA_PUBKEY(bio, rsa);
        int keylen = BIO_read(bio, buffer, MAX);
        write(connfd, buffer, keylen);
        BIO_free(bio);
        RSA_free(rsa);
    } else {
        write(connfd, "Error: Public key not found", 27);
    }

    close(connfd);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (SA*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);

    printf("Key server running on port %d...\n", PORT);

    while (1) {
        socklen_t len = sizeof(cli);
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd >= 0) {
            handle_client(connfd);
        }
    }

    close(sockfd);
    return 0;
}
