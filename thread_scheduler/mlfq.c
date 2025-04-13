#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_PROCESSES 5
#define TIME_QUANTUM_1 1 // Time quantum for queue 1
#define TIME_QUANTUM_2 2 // Time quantum for queue 2
#define MAX_EXECUTION_TIME 10 // Max execution time for a process

// Color definitions for processes
const char* RESET = "\033[0m";
const char* COLORS[] = {
    "\033[34m", // Blue
    "\033[32m", // Green
    "\033[36m", // Cyan
    "\033[35m", // Magenta
    "\033[33m"  // Yellow
};

typedef struct {
    int id;                  // Process ID
    int burst_time;         // Total burst time required
    int remaining_time;     // Remaining burst time
    int queue_level;        // Current queue level (0 or 1)
    const char* color;      // Color for the process output
} Process;

void initialize_processes(Process processes[]) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].id = i;
        processes[i].burst_time = rand() % MAX_EXECUTION_TIME + 1; // Random burst time
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].queue_level = 0; // Start in the highest priority queue
        processes[i].color = COLORS[i]; // Assign a unique color to each process
    }
}

void display_processes(Process processes[]) {
    printf("Processes:\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("%sProcess %d -> Burst Time: %d, Remaining Time: %d, Queue Level: %d%s\n",
               processes[i].color, processes[i].id, processes[i].burst_time, processes[i].remaining_time, processes[i].queue_level, RESET);
    }
}

void mlfq_scheduling(Process processes[]) {
    int all_done;

    do {
        all_done = 1; // Assume all processes are done
        for (int i = 0; i < NUM_PROCESSES; i++) {
            if (processes[i].remaining_time > 0) {
                all_done = 0; // At least one process is still running

                // Determine time quantum based on queue level
                int time_quantum = (processes[i].queue_level == 0) ? TIME_QUANTUM_1 : TIME_QUANTUM_2;
                int executed_time = (processes[i].remaining_time < time_quantum) ? processes[i].remaining_time : time_quantum;

                // Simulate execution
                sleep(executed_time); // Simulate time passing
                processes[i].remaining_time -= executed_time;

                printf("%sProcess %d executed for %d seconds (Remaining Time: %d)%s\n",
                       processes[i].color, processes[i].id, executed_time, processes[i].remaining_time, RESET);

                // If the process has remaining time, increase queue level
                if (processes[i].remaining_time > 0) {
                    processes[i].queue_level = 1; // Move to lower priority queue
                } else {
                    printf("%sProcess %d finished execution.%s\n", processes[i].color, processes[i].id, RESET);
                }
            }
        }
    } while (!all_done);
}

int main() {
    srand(time(NULL)); // Seed for random number generation
    Process processes[NUM_PROCESSES];

    initialize_processes(processes);
    display_processes(processes);
    mlfq_scheduling(processes);

    printf("All processes have completed execution.\n");
    return 0;
}
