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
    int priority; // New priority field
    const char* color; // Color for the thread
} ThreadInfo;

int compare_by_arrival_and_priority(const void* a, const void* b) {
    ThreadInfo* threadA = (ThreadInfo*)a;
    ThreadInfo* threadB = (ThreadInfo*)b;
    if (threadA->arrival_time == threadB->arrival_time) {
        return threadA->priority - threadB->priority; // Higher priority first (lower number)
    }
    return threadA->arrival_time - threadB->arrival_time;
}

void* thread_function(void* arg) {
    ThreadInfo* t = (ThreadInfo*)arg;

    pthread_mutex_lock(&mutex);
    while (t->id != current_thread) {
        pthread_cond_wait(&cond_var, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Global Time: %02d -> %sThread %d running (Arrival Time: %d, CPU Time: %d, Priority: %d)%s\n", 
           global_time, t->color, t->id, t->arrival_time, t->cpu_time, t->priority, RESET);
    for (int i = 0; i < t->cpu_time; i++) {
        sleep(1);
        global_time++;
        if (i == t->cpu_time - 1) {
            printf("Global Time: %02d -> %sThread %d finished.%s\n", global_time, t->color, t->id, RESET);
        } else {
            printf("Global Time: %02d -> %sThread %d running.%s\n", global_time, t->color, t->id, RESET);
        }
    }

    pthread_exit(NULL);
}

void priority_scheduler(ThreadInfo threads[], pthread_t thread_ids[], int n) {
    // Sort the threads by their arrival time and priority before scheduling
    qsort(threads, n, sizeof(ThreadInfo), compare_by_arrival_and_priority);

    for (int i = 0; i < n; i++) {
        // Simulate CPU idle time if global_time is less than thread's arrival time
        while (global_time < threads[i].arrival_time) {
            printf("Global Time: %02d -> %sCPU IDLE.%s\n", global_time, RED, RESET);
            sleep(1);
            global_time++;
        }

        // Lock the mutex, set current thread, and signal the waiting thread to run
        pthread_mutex_lock(&mutex);
        current_thread = threads[i].id;  // Allow the current thread to run
        pthread_cond_broadcast(&cond_var);  // Wake up the waiting thread
        pthread_mutex_unlock(&mutex);

        // Wait for the current thread to finish execution
        pthread_join(thread_ids[i], NULL);
    }
}

int main() {
    int n = 5;  // Number of threads
    ThreadInfo threads[5] = {
        {0, 2, 3, 1, BLUE},     // Thread 0: Arrival Time 2, CPU Time 3 seconds, Priority 1
        {1, 2, 5, 2, GREEN},    // Thread 1: Arrival Time 2, CPU Time 5 seconds, Priority 2
        {2, 2, 2, 0, CYAN},     // Thread 2: Arrival Time 2, CPU Time 2 seconds, Priority 0 (highest)
        {3, 13, 2, 3, MAGENTA}, // Thread 3: Arrival Time 13, CPU Time 2 seconds, Priority 3
        {4, 13, 4, 4, YELLOW}   // Thread 4: Arrival Time 13, CPU Time 4 seconds, Priority 4
    };

    pthread_t thread_ids[n];

    // Create a thread for each task
    for (int i = 0; i < n; i++) {
        pthread_create(&thread_ids[i], NULL, thread_function, (void*)&threads[i]);
    }

    // Run the priority scheduler
    priority_scheduler(threads, thread_ids, n);

    printf("All threads have completed execution.\n");

    return 0;
}
