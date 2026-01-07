#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*simple_func)(void);

void call_symbol(void *handle, const char *name) {
  simple_func func = (simple_func)dlsym(handle, name);
  if (func != NULL) {
    printf("  [Main] Calling %s\n", name);
    func();
  } else {
    char *error = dlerror();
    printf("  [Main] Error: %s\n", error);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <library_path>\n", argv[0]);
    return 1;
  }

  // 1. App explicitly loads the target library first
  void *h_target = dlopen("./liba.so", RTLD_NOW | RTLD_GLOBAL);
  if (!h_target) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  // 2. App loads the tracer plugin
  printf("Loading: %s\n", argv[1]);
  void *h_tracer = dlopen(argv[1], RTLD_NOW | RTLD_GLOBAL);
  if (!h_tracer) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  // 3. App looks up 'A' (Getting the intercepted version from tracer)
  call_symbol(h_tracer, "A");
  return 0;
}
