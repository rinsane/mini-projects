#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

const SIZE = 1<<20;
#define THREADS 8

void random_init(int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < SIZE; i++) {
        *(arr + i) = rand() % 100;
        // printf("%d ", *(arr + i));
        sum += arr[i];
    }
    printf("\nSequential Sum: %d", sum);
    puts("");
    puts("");
}

void printer(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", *(arr + i));
    }
    puts("");
    puts("");
}

void tree_sum_framework(int* A, int* M, int (*f1)(int), int (*f2)(int, int), int max_threads) {
    // Bottom-Up Phase - Step S1
    #pragma omp parallel for num_threads(max_threads)
    for (int i = 0; i < SIZE; i++) {
        M[i + SIZE] = f1(A[i]);
    }

    // Bottom-Up Phase - Step S2
    int procs = SIZE >> 1;
    #pragma omp parallel num_threads(max_threads) shared(procs)
    {
        do {
            #pragma omp for
            for (int i = 0; i < procs; i++) {
                int j = i + procs;
                M[j] = f2(M[j << 1], M[(j << 1) + 1]);
            }
            #pragma omp single
            procs = procs >> 1;
        } while (procs > 0);
    }
    // Bottom-Up Phase - Step S2
    /*
    for (int r = 1; r <= log2(SIZE); r++) {
        int offset = SIZE / (1 << r);
        #pragma omp parallel for num_threads(max_threads)
        for (int i = 0; i < offset; i++) {
            int j = i + offset;
            M[j] = f2(M[2 * j], M[2 * j + 1]);
        }
    }
    */
}

int add(int n1, int n2) {
    return n1 + n2;
}

int copy(int n1) {
    return n1;
}

int main() {
    srand(time(NULL));

    int* A = (int*)malloc(sizeof(int) * SIZE);
    int* M = (int*)calloc(SIZE * 2, sizeof(int));

    random_init(A, SIZE);

    // pass function pointers
    tree_sum_framework(A, M, copy, add, THREADS);  

    printf("Parallel Sum: %d\n", M[1]);

    free(A);
    free(M);

    return 0;
}
