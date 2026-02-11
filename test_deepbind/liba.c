#include <stdio.h>
#include <stdbool.h>

void internal_secret(void) {
    printf("  [liba] internal_secret called (CORRECT)\n");
}

int A(void) {
    printf("  [liba] A called. invoking internal_secret...\n");
    internal_secret();
    return true;
}
