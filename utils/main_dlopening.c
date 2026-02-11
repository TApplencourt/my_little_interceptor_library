#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <library_path>\n", argv[0]);
    return 1;
  }

  printf("Loading: %s\n", argv[1]);
  void *handle = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL);

  if (!handle) {
    fprintf(stderr, "Error loading libb: %s\n", dlerror());
    return 1;
  }

  call_symbol(handle, "A");
  dlclose(handle);
  return 0;
}
