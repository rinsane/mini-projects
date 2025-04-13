#include <stdio.h>
#include <stdlib.h>

// array printing function
void printarr(int *array, int size) {

    for (int i = 0; i < size; i++) {
        printf("%d ", *(array + i));
    }
    printf("\n");
}

int main() {
    int *arr;
    int n = 10;

////////////////////////////////////////////////////////////////////
    printf("\n1. Forgetting to allocate memory:\n");

    // allocating memory to ensure proper access
    arr = (int *)malloc(sizeof(int) * n);
    printf(" -- memory address of the array is: %p\n", (void *)arr);
    // assigning elements to the memeory locations in the array
    for (int i = 0; i < n; i++) {
        *(arr + i) = i + 1;
    }
    printarr(arr, n);

////////////////////////////////////////////////////////////////////
    printf("\n2. Not allocating enough memory:\n");

    // increasing the size of array
    n++;
    // reallocating new size of memory to the array
    arr = (int *)realloc(arr, sizeof(int) * n);
    printf(" -- memory address of the array is: %p\n", (void *)arr);
    // assigning element to the new location
    *(arr + (n - 1)) = 68;
    printarr(arr, n);

////////////////////////////////////////////////////////////////////
    printf("\n3. Forgetting to free memory:\n");

    printf(" -- freeing location %p\n", (void *)arr);
    // freeing the memory at that address
    free(arr);
    // setting array to NULL after freeing to ensure no later access
    arr = NULL;
    printf("freed location   %p\n", (void *)arr);

////////////////////////////////////////////////////////////////////
    printf("\n4. Free memory before done with it:\n");

    // access handling
    if (arr != NULL) {
        *(arr + 0) = 999;
        // printing the array after freeing it
        printarr(arr, n);
        printf(" -- garbage values printed!\n");
    } else {
        printf(" -- attempting to access freed memory!\n");
    }

////////////////////////////////////////////////////////////////////
    printf("\n5. Call free redundantly:\n");

    // calling free again
    if (arr == NULL) {
        printf(" -- array already freed!\n");
    } else {
        free(arr);
    }

////////////////////////////////////////////////////////////////////
    printf("\n6. Calling free incorrectly:\n");

    // freeing a non dynamically allocated pointer
    int value = 100;
    int *val_ptr = &value;
    printf(" -- attempting to free a non dynamically allocated pointer:\n");
    // checking for validity
    if (val_ptr != NULL) {
        printf(" -- Error: Cannot free a non dynamically allocated pointer.\n");
    } else {
        free(val_ptr);
        printf(" -- Success!\n");
    }

    // freeing part of an already freed array
    int *arr_ptr = arr + 1;
    printf(" -- attempting to free part of an already freed array:\n");
    // checking for validity
    if (arr == NULL) {
        printf(" -- Error: Cannot free part of an already freed array.\n");
    } else {
        free(arr_ptr);
        printf(" -- Success!\n");
    }
    
    printf("\n");
    return 0;
}
