#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <library_path_1> <library_path_2>\n", argv[0]);
    return 1;
  }

  // 1. App explicitly loads the target library first
  //
  printf("Loading: %s\n", argv[1]);
  void *h_target = dlopen(argv[1], RTLD_NOW | RTLD_GLOBAL);
  if (!h_target) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  // 2. App loads the tracer plugin
  printf("Loading: %s\n", argv[2]);
  void *h_tracer = dlopen(argv[2], RTLD_NOW | RTLD_GLOBAL);
  if (!h_tracer) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  // 3. App looks up 'A' (Getting the intercepted version from tracer)
  printf("Loading A from  %s\n", argv[2]);
  call_symbol(h_tracer, "A");
  return 0;
}
