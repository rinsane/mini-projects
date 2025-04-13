#include "headers.h"

int main() {

    int running_array[requests_ptr][3];
    int running_ptr = 0;
    int admitted = 0;
    int finished = 0;
    int current_time = 0;

    file_parser(FILE_PATH);
    for (int i = 0; i < requests_ptr; i++) {
        printf("<%d, %d, %d>\n", requests[i][0], requests[i][1], requests[i][2]);
    }

    while (finished < requests_ptr) {
        int check = 0;

        while (admitted != requests_ptr && possible_allocation(requests[admitted][1], requests[admitted][0])) {
            check = 1;

            printf(COLOR_YELLOW "[TIME %2d] " COLOR_BLUE "Admitting Process: %d" COLOR_RESET "\n", 
            current_time, requests[admitted][0]);

            admitted++;
            print_memory_buffer();
        }

        for (int i = 0; i < admitted; i++) {
            requests[i][2]--;

            if (requests[i][2] == 0) {
                check = 1;
                deallocate_memory(requests[i][0]);

                printf(COLOR_YELLOW "[TIME %2d] " COLOR_GREEN "Process Finished: %d" COLOR_RESET "\n", 
                current_time, requests[i][0]);

                print_memory_buffer();

                finished++;
            }
        }

        if (!check) printf(COLOR_YELLOW "[TIME %2d] " COLOR_RESET "---\n", current_time);
        current_time++;

        sleep(1);
    }

    puts("");
    return 0;
}
