#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

void internal_secret(void) {
    printf("  [liba] internal_secret called (CORRECT)\n");
}

uint64_t A(void) {
    printf("  [liba] A called. invoking internal_secret...\n");
    internal_secret();
    return 0;
}
