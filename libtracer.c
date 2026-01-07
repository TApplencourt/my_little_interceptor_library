#define _GNU_SOURCE
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Define a function pointer type for readability
typedef void (*func_t)(void);

static void *manual_liba_handle = NULL;
static pthread_once_t manual_liba_once = PTHREAD_ONCE_INIT;

static const char *get_library_name() {
  const char *value = getenv("TRACED_LIB");
  if (value != NULL) {
    return value;
  } else {
    return "liba.so";
  }
}

static void load_manual_liba(void) {

  const char *libname = get_library_name();

  manual_liba_handle = dlopen(libname, RTLD_LAZY | RTLD_LOCAL);
  if (!manual_liba_handle) {
    printf("  [libTracer] dlopen failed: %s\n", dlerror());
  } else {
    printf("  [libTracer] dlopen of %s succeeded\n", libname);
  }
}

static void resolve(const char *func_name, void *current_wrapper_addr,
                    func_t *cache_ptr) {

  printf("  [libTracer] Finding Symbol %s\n", func_name);

  // 1. Try RTLD_NEXT (Works if patchelf worked)
  func_t next_func = (func_t)dlsym(RTLD_NEXT, func_name);

  // 2. Fallback: If RTLD_NEXT failed and we have a manual handle, look there
  if (!next_func && manual_liba_handle) {
    next_func = (func_t)dlsym(manual_liba_handle, func_name);
  }

  // 3.  Abort safely instead of stack overflowing
  Dl_info my_info, next_info;
  if (dladdr((void *)resolve, &my_info) &&
      dladdr((void *)next_func, &next_info)) {
    if (my_info.dli_fbase == next_info.dli_fbase) {
      printf("  [libTracer] FATAL: Symbol '%s' resolved inside the Tracer\n",
             func_name);
      exit(1);
    }
  }

  // 4. Store
  if (next_func) {
    *cache_ptr = next_func;
  } else {
    printf("  [libTracer] FATAL: Symbol '%s' not found in traced lib\n",
           func_name);
    exit(1);
  }
}

__attribute__((constructor)) static void init_tracer(void) {
  pthread_once(&manual_liba_once, load_manual_liba);
}

__attribute__((destructor)) static void cleanup(void) {
  if (manual_liba_handle) {
    dlclose(manual_liba_handle);
    manual_liba_handle = NULL;
  }
}

#define DEFINE_WRAPPER(NAME)                                                   \
  void NAME(void) {                                                            \
    printf("  [libTracer] Intercepted %s\n", #NAME);                           \
    static func_t cached = NULL;                                               \
    if (!cached)                                                               \
      resolve(#NAME, (void *)NAME, &cached);                                   \
    cached();                                                                  \
  }

DEFINE_WRAPPER(A)
DEFINE_WRAPPER(A2)
DEFINE_WRAPPER(B)
// No A1, A1 is not exported
