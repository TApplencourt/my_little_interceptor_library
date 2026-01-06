#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*simple_func)(void);

void call_symbol(void *handle, const char *name) {
  simple_func func = (simple_func)dlsym(handle, name);
  if (func != NULL) {
    printf("  [ProgramC] Calling %s\n", name);
    func();
  } else {
    char *error = dlerror();
    printf("  [ProgramC] Error: %s\n", error);
  }
}

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

#ifdef _OPENMP
    #pragma omp parallel
    {
        call_symbol(handle, "A");
    }
#else
  call_symbol(handle, "A1");
  call_symbol(handle, "A2");
  call_symbol(handle, "B");
#endif

  dlclose(handle);
  return 0;
}
