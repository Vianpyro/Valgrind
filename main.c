#include <stdio.h>
#include <stdlib.h>

int main() {
    int *leak = (int *)malloc(10 * sizeof(int));  // Allocate memory

    // Simulate usage of allocated memory
    for (int i = 0; i < 10; i++) {
        leak[i] = i;
        printf("%d ", leak[i]);
    }

    printf("\n");

    // Intentionally forget to free the allocated memory to create a memory leak
    // free(leak);  // This line is missing, causing the memory leak

    return 0;
}
