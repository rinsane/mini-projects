#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
int current_thread = -1;
int global_time = 0;

// Color definitions
const char* RED = "\033[31m";    // CPU IDLE
const char* BLUE = "\033[34m";   // Thread 0
const char* GREEN = "\033[32m";  // Thread 1
const char* CYAN = "\033[36m";   // Thread 2
const char* MAGENTA = "\033[35m"; // Thread 3
const char* YELLOW = "\033[33m";  // Thread 4
const char* RESET = "\033[0m";    // Reset to default

typedef struct {
    int id;
    int arrival_time;
    int cpu_time;
    const char* color; // Color for the thread
} ThreadInfo;

int compare_by_arrival_time(const void* a, const void* b) {
    ThreadInfo* threadA = (ThreadInfo*)a;
    ThreadInfo* threadB = (ThreadInfo*)b;
    return threadA->arrival_time - threadB->arrival_time;
}

void* thread_function(void* arg) {
    ThreadInfo* t = (ThreadInfo*)arg;

    // Lock the mutex before waiting for the signal
    pthread_mutex_lock(&mutex);
    while (t->id != current_thread) {
        printf("Thread %d\n", t->id);
        // Wait until it's this thread's turn
        pthread_cond_wait(&cond_var, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    // Simulate execution (CPU burst)
    printf("Global Time: %02d -> %sThread %d running (Arrival Time: %d, CPU Time: %d)%s\n", global_time, t->color, t->id, t->arrival_time, t->cpu_time, RESET);
    for (int i = 0; i < t->cpu_time; i++) {
        sleep(1);  // Simulate 1 second of execution
        global_time++;
        if (i == t->cpu_time - 1) {
            printf("Global Time: %02d -> %sThread %d finished.%s\n", global_time, t->color, t->id, RESET);
        } else {
            printf("Global Time: %02d -> %sThread %d running.%s\n", global_time, t->color, t->id, RESET);
        }
    }

    pthread_exit(NULL);
}

void fcfs_scheduler(ThreadInfo threads[], pthread_t thread_ids[], int n) {
    // Sort the threads by their arrival time before scheduling
    qsort(threads, n, sizeof(ThreadInfo), compare_by_arrival_time);

    for (int i = 0; i < n; i++) {
        // Simulate CPU idle time if global_time is less than thread's arrival time
        while (global_time < threads[i].arrival_time) {
            printf("Global Time: %02d -> %sCPU IDLE.%s\n", global_time, RED, RESET);
            sleep(1);
            global_time++;
        }

        // Lock the mutex, set current thread, and signal the waiting thread to run
        // pthread_mutex_lock(&mutex);
        current_thread = threads[i].id;  // Allow the current thread to run
        printf("Current Thread: %d\n", current_thread);
        pthread_cond_broadcast(&cond_var);  // Wake up the waiting thread
        // pthread_mutex_unlock(&mutex);

        // Wait for the current thread to finish execution
        pthread_join(thread_ids[i], NULL);
    }
}

int main() {
    int n = 5;  // Number of threads
    // ThreadInfo threads[5] = {
    //     {0, 2, 3, BLUE},     // Thread 0: Arrival Time 2, CPU Time 3 seconds
    //     {1, 2, 4, GREEN},    // Thread 1: Arrival Time 2, CPU Time 4 seconds
    //     {2, 10, 3, CYAN},    // Thread 2: Arrival Time 10, CPU Time 3 seconds
    //     {3, 12, 2, MAGENTA}, // Thread 3: Arrival Time 12, CPU Time 2 seconds
    //     {4, 11, 4, YELLOW}   // Thread 4: Arrival Time 11, CPU Time 4 seconds
    // };
    ThreadInfo threads[5] = {
        {0, 2, 3, BLUE},     // Thread 0: Arrival Time 2, CPU Time 3 seconds
        {1, 2, 5, GREEN},    // Thread 1: Arrival Time 2, CPU Time 4 seconds
        {2, 2, 2, CYAN},    // Thread 2: Arrival Time 10, CPU Time 3 seconds
        {3, 13, 2, MAGENTA}, // Thread 3: Arrival Time 12, CPU Time 2 seconds
        {4, 13, 4, YELLOW}   // Thread 4: Arrival Time 11, CPU Time 4 seconds
    };

    pthread_t thread_ids[n];

    // Create a thread for each task
    for (int i = 0; i < n; i++) {
        pthread_create(&thread_ids[i], NULL, thread_function, (void*)&threads[i]);
    }

    // Run the FCFS scheduler
    fcfs_scheduler(threads, thread_ids, n);

    printf("All threads have completed execution.\n");

    return 0;
}
