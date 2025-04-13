#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 8
#define THREADS 8

void random_init(int* arr, int n) {
    for (int i = 0; i < SIZE; i++) {
        *(arr + i) = rand() % 10;
        printf("%d ", *(arr + i));
    }
    printf("\nSequential Prefix Sum:\n");
    int sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum += arr[i];
        printf("%d ", sum);
    }
    puts("");
    puts("");
}

void printer(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", *(arr + i));
    }
    puts("\n");
}

void prefix_sum_framework(int* A, int* M, int* L, int* R, int* B, int (*f1)(int), int (*f2)(int, int), int max_threads) {
    // Bottom-Up Phase - Step S1
    #pragma omp parallel for num_threads(max_threads)
    for (int i = 0; i < SIZE; i++) {
        M[i + SIZE] = f1(A[i]);
    }

    // Bottom-Up Phase - Step S2
    #pragma omp parallel num_threads(max_threads)
    {
        for (int r = 1; r <= log2(SIZE); r++) {
            #pragma omp single
            int offset = SIZE / (1 << r);
            #pragma omp for
            for (int i = 0; i < offset; i++) {
                int j = i + offset;
                M[j] = f2(M[2 * j], M[2 * j + 1]);
            }
        }
    }

    // Another Step in Bottom-Up Phase - Step S3
    #pragma omp parallel for num_threads(max_threads)
    for (int i = 1; i < SIZE; i++) {
        L[i] = M[2 * i];
    }
    R[1] = 0;

    // Top-Down Phase - Step S4
    #pragma omp parallel num_threads(max_threads)
    {
        for (int r = 1; r < log2(SIZE) + 1; r++) {
            int offset = 1 << r;
            #pragma omp for
            for (int i = 0; i < offset; i++) {
                int j = i + offset;
                if (j % 2 == 0)
                    R[j] = R[j / 2];
                else
                    R[j] = f2(R[j / 2], L[j / 2]);
            }
        }
    }

    // Step S5 - Compute final result
    #pragma omp parallel for num_threads(max_threads)
    for (int i = 0; i < SIZE; i++) {
        B[i] = f2(M[i + SIZE], R[i + SIZE]);
    }
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
    int* L = (int*)calloc(SIZE, sizeof(int));
    int* R = (int*)calloc(SIZE * 2, sizeof(int));
    int* B = (int*)calloc(SIZE, sizeof(int));

    random_init(A, SIZE);

    // pass function pointers
    prefix_sum_framework(A, M, L, R, B, copy, add, THREADS);

    printf("\nParallel Prefix Sum:\n");
    printer(B, SIZE);

    free(A);
    free(M);
    free(L);
    free(R);
    free(B);

    return 0;
}
