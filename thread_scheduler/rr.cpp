#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unistd.h>

using namespace std;

const int TIME_QUANTUM = 1; // Time quantum for round robin

struct Job {
    int identifier;    // Job ID
    int start_time;    // Start time of the job
    int duration;      // Total duration of the job
    const char* color; // Color for job output
};

// Comparator for sorting jobs by start time
bool FIFO(const Job &a, const Job &b) {
    return a.start_time < b.start_time;
}

// Comparator for the priority queue (min-heap based on start time)
struct CompareJob {
    bool operator()(const Job &a, const Job &b) {
        return a.start_time > b.start_time; // Min-heap
    }
};

void roundRobinScheduler(vector<Job> &task_set) {
    int current_time = 0;
    sort(task_set.begin(), task_set.end(), FIFO);
    priority_queue<Job, vector<Job>, CompareJob> task_queue;
    int idx = task_set.size() - 1;

    while (!task_set.empty() || !task_queue.empty()) {
        // Add new jobs to the queue that have arrived by current_time
        while (idx >= 0 && task_set[idx].start_time <= current_time) {
            task_queue.push(task_set[idx]);
            task_set.pop_back();
            idx--;
        }

        if (task_queue.empty()) {
            // If no jobs are available, move time forward to the next job's start time
            if (idx >= 0) {
                current_time = task_set[idx].start_time;
            }
            continue;
        } else {
            Job current_task = task_queue.top();
            task_queue.pop();
            int executed_time = min(current_task.duration, TIME_QUANTUM);
            current_task.duration -= executed_time;

            // Print the job execution with color
            printf("Global Time: %02d -> %sJob %d scheduled (Remaining Duration: %d)%s\n",
                   current_time, current_task.color, current_task.identifier, current_task.duration, "\033[0m");

            // Simulate execution time
            sleep(executed_time);

            // If the job is not finished, re-add it to the queue with updated duration
            if (current_task.duration > 0) {
                current_task.start_time = current_time; // Update start time for the next round
                task_queue.push(current_task);
            }
            current_time += executed_time; // Increment current time
        }
    }
}

int main() {
    vector<Job> task_set = {
        {1, 0, 3, "\033[34m"},  // Job 1: start at 0, duration 3, color blue
        {2, 1, 5, "\033[32m"},  // Job 2: start at 2, duration 5, color green
        {3, 2, 2, "\033[36m"},  // Job 3: start at 5, duration 2, color cyan
        {4, 1, 1, "\033[35m"},  // Job 4: start at 8, duration 1, color magenta
        {5, 2, 4, "\033[33m"}  // Job 5: start at 10, duration 4, color yellow
    };

    roundRobinScheduler(task_set);

    cout << "All jobs have completed execution." << endl;
    return 0;
}