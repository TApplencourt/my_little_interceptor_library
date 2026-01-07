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

  // 2. Fallback: Check the manual handle
  if (!next_func) {
    // If the handle is NULL, we might be inside the recursion gap (inside the
    // dlopen constructor). We try to grab the handle using RTLD_NOLOAD.
    if (!manual_liba_handle) {
      const char *libname = get_library_name();
      // RTLD_NOLOAD: Get handle if it is already resident, but don't load it if
      // not. This works even if dlopen hasn't returned to the caller yet!
      void *temp_handle = dlopen(libname, RTLD_LAZY | RTLD_NOLOAD);
      if (temp_handle) {
        next_func = (func_t)dlsym(temp_handle, func_name);
        // Note: We don't save temp_handle to manual_liba_handle here to avoid
        // race conditions with the main thread logic, we just use it for this
        // resolution.
        dlclose(temp_handle); // decrement refcount incremented by NOLOAD
      }
    } else {
      next_func = (func_t)dlsym(manual_liba_handle, func_name);
    }
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
