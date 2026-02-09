// main_deepbind_test.c
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*func_t)(void);

// This symbol intentionally conflicts with liba.so's internal one
void internal_secret(void) {
    printf("  [MAIN] WRONG internal_secret called! Shadowing happened!\n");
    abort();  // Make it obvious
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <liba.so> <libtracer.so>\n", argv[0]);
        return 1;
    }

    // IMPORTANT: Load libtracer.so FIRST → its symbols (including main's) enter global scope first
    void *tracer = dlopen(argv[2], RTLD_LAZY | RTLD_GLOBAL);
    if (!tracer) {
        fprintf(stderr, "Failed to load tracer: %s\n", dlerror());
        return 1;
    }

    void *liba = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL);
    if (!liba) {
        fprintf(stderr, "Failed to load liba.so: %s\n", dlerror());
        return 1;
    }

    func_t A = dlsym(liba, "A");
    printf("Calling A() from liba.so...\n");
    A();

    dlclose(liba);
    dlclose(tracer);
    return 0;
}
