#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>

// This symbol would shadow liba's internal_secret WITHOUT RTLD_DEEPBIND
void internal_secret(void) {
    fprintf(stderr, "  [main] FATAL: internal_secret called! Shadowing occurred.\n");
    exit(1);
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
