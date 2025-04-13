#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 1024

// allocation tracker struct
typedef struct {
    void *ptr;        // pointer to allocated memo
    size_t size;      // size of the allocation
    const char *file; // file where allocation happened
    int line;         // line number where alloc happened
} Allocation;

// array to store all allocations
static Allocation allocs[BUFFER];
static int alloc_count = 0;

// custom malloc to track allocations
void* custom_malloc(size_t size, const char *file, int line) {
    // allocate memory
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // store allocation info in the allocation array
    allocs[alloc_count].ptr = ptr;
    allocs[alloc_count].size = size;
    allocs[alloc_count].file = file;
    allocs[alloc_count].line = line;

    printf("Allocated %zu bytes (at %s:%d)\n", 
    allocs[alloc_count].size, allocs[alloc_count].file, allocs[alloc_count].line);

    alloc_count++;
    return ptr;
}

// custom calloc to track allocations
void* custom_calloc(size_t num, size_t size, const char *file, int line) {
    // callocate memory
    void *ptr = calloc(num, size);
    if (ptr == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // store allocation info in the allocation array
    allocs[alloc_count].ptr = ptr;
    allocs[alloc_count].size = num * size;
    allocs[alloc_count].file = file;
    allocs[alloc_count].line = line;

    printf("Callocated %zu bytes (at %s:%d)\n", 
    allocs[alloc_count].size, allocs[alloc_count].file, allocs[alloc_count].line);

    alloc_count++;
    return ptr;
}

// custom realloc to track allocations
void* custom_realloc(void *old_ptr, size_t size, const char *file, int line) {
    // reallocate memory
    void *new_ptr = realloc(old_ptr, size);
    if (new_ptr == NULL) {
        printf("Error: Memory reallocation failed\n");
        return NULL;
    }

    // update allocation info in the allocation array
    for (int i = 0; i < alloc_count; i++) {
        if (allocs[i].ptr == old_ptr) {
            allocs[i].ptr = new_ptr;
            allocs[i].size = size;
            allocs[i].file = file;
            allocs[i].line = line;
            printf("Reallocated %zu bytes (at %s:%d)\n", allocs[i].size, allocs[i].file, allocs[i].line);
            break;
        }
    }
    return new_ptr;
}

// custom free to track deallocations
void custom_free(void *ptr, const char *file, int line) {
    // ignore NULL pointers
    if (ptr == NULL) return;

    // find and remove allocation info from allocation array
    for (int i = 0; i < alloc_count; i++) {
        if (allocs[i].ptr == ptr) {
            free(ptr);  // free the memory
            printf("Freed %zu bytes (allocated at %s:%d)\n", allocs[i].size, allocs[i].file, allocs[i].line);
            // gapfilling by moving the last allocation to the freed location in allocation array
            allocs[i] = allocs[alloc_count - 1];
            alloc_count--;
            return;
        }
    }

    printf("Warning: Attempt to free untracked memory at %s:%d\n", file, line);
}

// report memory leaks (unfreed allocations)
void report_leaks() {
    if (alloc_count > 0) {
        printf("\nMemory Leak Report:\n");
        for (int i = 0; i < alloc_count; i++) {
            printf("Leaked %zu bytes allocated at %s:%d\n", allocs[i].size, allocs[i].file, allocs[i].line);
        }
    } else {
        printf("No memory leaks detected!\n");
    }
}

// wrappers for standard memory management functions
#define malloc(size) custom_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) custom_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) custom_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) custom_free(ptr, __FILE__, __LINE__)

int main() {
    int *arr1 = malloc(100 * sizeof(int));   // allocating memory
    int *arr2 = calloc(50, sizeof(int));     // allocating zero-initialized memory
    arr1 = realloc(arr1, 200 * sizeof(int)); // reallocating memory
    int *arr3 = malloc(900 * sizeof(int));   // allocating memory

    puts("");
    free(arr1); // freeing arr1
    // NOTE: arr2 not freed to test!
    free(arr3); // freeing arr3

    // report memory leaks at the end of the program
    report_leaks();

    return 0;
}
