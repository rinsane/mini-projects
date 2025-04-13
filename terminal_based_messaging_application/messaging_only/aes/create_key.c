#include <stdio.h>
#include <stdint.h>

int main() {
    uint8_t key[] = {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    FILE *key_file = fopen("aes.key", "wb");

    fwrite(key, 1, sizeof(key), key_file);
    fclose(key_file);

    return 0;
}
