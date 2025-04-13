#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_REQUESTS 200
#define MEMORY 10
#define FILE_PATH "request.txt"
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"

int requests[MAX_REQUESTS][3];
int requests_ptr = 0;
int memory_buffer[MEMORY] = {0};
void file_parser(char *file_path);
bool possible_allocation(int size, int process_id);
void deallocate_memory(int process_id);
void print_memory_buffer();

bool possible_allocation(int size, int process_id) {
    int current_space = 0;
    for (int i = 0; i < MEMORY; i++) {
        if (memory_buffer[i] == 0) current_space++;
        else current_space = 0;
        
        if (current_space == size) {
            int start = i - size + 1;
            for (int j = i - size + 1; j <= i; j++) {
                memory_buffer[j] = process_id;
            }
            return true;
        }
    }

    return false;
}

void deallocate_memory(int process_id) {
    for (int i = 0; i < MEMORY; i++) {
        if (memory_buffer[i] == process_id) {
            memory_buffer[i] = 0;
        }
    }
}

void print_memory_buffer() {
    for (int i = 0; i < MEMORY; i++) {
        printf(COLOR_RED "%d ", memory_buffer[i]);
    }
    printf(COLOR_RESET "\n");
}

void file_parser(char *file_path) {
    FILE *file_ptr;

    file_ptr = fopen(file_path, "r");

    if (file_ptr == NULL) {
        printf("File can't be opened!\n");
        return;
    }

    char ch;
    int checker = 1;

    while ((ch = fgetc(file_ptr)) != EOF) {
        int numarr[3];
        int numarr_ptr = 0;
        if (ch == '<') {
            checker = 1;
            while (checker) {
                    char num[10];
                    int ptr = 0;
                    ch = fgetc(file_ptr);
                    while ((ch != ',') && (ch != '>')) {
                        num[ptr++] = ch;
                        ch = fgetc(file_ptr);
                    }
                    num[ptr] = '\0';
                    numarr[numarr_ptr++] = atoi(num);

                    if (ch == '>') {
                        checker = 0;
                    }
                }
                requests[requests_ptr][0] = numarr[0];
                requests[requests_ptr][1] = numarr[1];
                requests[requests_ptr][2] = numarr[2];
                requests_ptr++;
        }
    }

    fclose(file_ptr);
}
