#include <stdio.h>

// Declared extern - forces PLT lookup at runtime
//extern void internal_secret(void);

void internal_secret(void) {
    printf("  [liba] internal_secret called (CORRECT)\n");
}

void A(void) {
    printf("  [liba] A called. invoking internal_secret...\n");
    internal_secret();  // Goes through PLT -> dynamic resolution
}

void B(void) {
    printf("  [liba] B called\n");
}
