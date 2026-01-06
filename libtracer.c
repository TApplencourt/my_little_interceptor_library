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
  const char *value = getenv("LIB_TO_TRACE");
  if (value != NULL) {
    return value;
  } else {
    return "./liba.so";
  }
}

static void load_manual_liba(void) {
  manual_liba_handle = dlopen(get_library_name(), RTLD_LAZY | RTLD_GLOBAL);
  if (!manual_liba_handle) {
    fprintf(stderr, "  [libTracer] dlopen failed: %s\n", dlerror());
  } else {
    printf("  [libTracer] dlopen succeeded\n");
  }
}

static void resolve_and_run(const char *func_name, void *current_wrapper_addr) {
  printf("  [libTracer] Intercepted %s\n", func_name);

  // 1. Try RTLD_NEXT (Works if patchelf worked)
  func_t next_func = (func_t)dlsym(RTLD_NEXT, func_name);

  // 2. Fallback: If RTLD_NEXT failed and we have a manual handle, look there
  if (!next_func) {
    pthread_once(&manual_liba_once, load_manual_liba);
    if (manual_liba_handle) {
      next_func = (func_t)dlsym(manual_liba_handle, func_name);
    }
  }

  if ((void *)next_func == current_wrapper_addr) {
    printf("  [libTracer] FATAL: Infinite recursion detected on symbol '%s'. "
           "Don't trace the tracer.\n",
           func_name);
    exit(1); // Abort safely instead of stack overflowing
  }

  // 3. Call
  if (next_func) {
    next_func();
  } else {
    printf("  [libTracer] Symbol '%s' not found in intercepted lib\n",
           func_name);
  }
}

__attribute__((destructor)) static void cleanup(void) {
  if (manual_liba_handle) {
    dlclose(manual_liba_handle);
    manual_liba_handle = NULL;
  }
}

void A(void) { resolve_and_run("A", (void *)A); }
// No A1, A1 is not exported
void A2(void) { resolve_and_run("A2", (void *)A2); }

void B(void) { resolve_and_run("B", (void *)B); }
