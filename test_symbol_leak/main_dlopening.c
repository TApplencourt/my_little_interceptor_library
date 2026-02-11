#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <library_path>\n", argv[0]);
    return 1;
  }

  // 1. Load the shim as a local plugin
  printf("Loading Local: %s\n", argv[1]);
  void *handle = dlopen(argv[1], RTLD_LAZY | RTLD_LOCAL);
  if (!handle) {
    fprintf(stderr, "Error loading libb: %s\n", dlerror());
    return 1;
  }
  // 2. Trigger the shim so it loads the real lib
  call_symbol(handle, "A");
  // 3. Check private symbol where not exported
  void *leak = dlsym(RTLD_DEFAULT, "_secret_A");
  if (leak) {
    printf("FAIL: Symbol '_secret_A' leaked into Global Scope!\n");
    return 1;
  } else {
    printf("SUCCESS: Global Scope is clean.\n");
    return 0;
  }
}
