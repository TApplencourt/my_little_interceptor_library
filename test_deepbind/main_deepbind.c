#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*simple_func)(void);

// This symbol would shadow liba's internal_secret WITHOUT RTLD_DEEPBIND
void internal_secret(void) {
    fprintf(stderr, "  [main] FATAL: internal_secret called! Shadowing occurred.\n");
    exit(1);
}

void call_symbol(void *handle, const char *name) {
    simple_func func = (simple_func)dlsym(handle, name);
    if (func != NULL) {
        printf("  [Main] Calling %s\n", name);
        func();
    } else {
        printf("  [Main] Error: %s\n", dlerror());
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <library_path>\n", argv[0]);
        return 1;
    }

    printf("Loading: %s\n", argv[1]);
    // Use RTLD_DEEPBIND to protect loaded library's internal symbols
    void *handle = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND);

    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    call_symbol(handle, "A");
    dlclose(handle);
    return 0;
}
