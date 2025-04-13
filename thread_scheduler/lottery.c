#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 5
#define MAX_TICKETS 10

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
    int id;               // Process ID
    int num_tickets;     // Number of tickets assigned to the process
    const char* name;    // Process name
    const char* color;   // Color for the process output
} Process;

void generate_tickets(Process processes[]) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].num_tickets = rand() % MAX_TICKETS + 1; // Random tickets between 1 and MAX_TICKETS
    }
}

void display_processes(Process processes[]) {
    printf("Processes and their assigned tickets:\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("%sProcess %d (%s) -> Tickets: %d%s\n", 
               processes[i].color, processes[i].id, processes[i].name, processes[i].num_tickets, RESET);
    }
}

int lottery_scheduling(Process processes[]) {
    int total_tickets = 0;

    // Calculate total tickets
    for (int i = 0; i < NUM_PROCESSES; i++) {
        total_tickets += processes[i].num_tickets;
    }

    // Generate a random number based on total tickets
    int winning_ticket = rand() % total_tickets;

    // Select the winner based on the winning ticket
    int ticket_count = 0;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        ticket_count += processes[i].num_tickets;
        if (ticket_count > winning_ticket) {
            return processes[i].id; // Return the winning process ID
        }
    }
    return -1; // Should never reach here
}

int main() {
    srand(time(NULL)); // Seed for random number generation

    Process processes[NUM_PROCESSES] = {
        {0, 0, "Process A", COLORS[0]},
        {1, 0, "Process B", COLORS[1]},
        {2, 0, "Process C", COLORS[2]},
        {3, 0, "Process D", COLORS[3]},
        {4, 0, "Process E", COLORS[4]}
    };

    generate_tickets(processes);
    display_processes(processes);

    int winner = lottery_scheduling(processes);
    printf("The winning process is: %s%s (Process ID: %d)%s\n", 
           processes[winner].color, processes[winner].name, winner, RESET);

    return 0;
}
