#define _GNU_SOURCE
#include <assert.h>
#include <dlfcn.h>
#include <link.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL 0
#endif

typedef void (*func_t)(void);
static void *_Atomic real_handle = NULL;

// Resolve and cache the real implementation of a wrapped function
// - func_name: symbol name to resolve
// - wrapper_addr: address of our wrapper (used for recursion detection)
// - cache_ptr: where to store the resolved function pointer
// - from_ctor: true if called from constructor (happy path)
static void resolve(const char *func_name, const void *wrapper_addr,
                    func_t *cache_ptr, bool from_ctor) {

  printf("  [libTracer] Resolving symbol '%s'\n", func_name);

  // Strategy 1: Try RTLD_NEXT (works if LD_PRELOAD or patchelf succeeded)
  func_t real_func = (func_t)dlsym(RTLD_NEXT, func_name);
  if (real_func) {
    printf("  [libTracer] Symbol '%s' found via RTLD_NEXT\n", func_name);
    *cache_ptr = real_func;
    return;
  }

  // Strategy 2: Manual dlopen fallback

  // 2.1 Obtain handle to the real library
  if (!real_handle) {
    char *libname = getenv("REAL_LIB");
    if (!libname)
      libname = "liba.so";

    if (from_ctor) {
      // We're in our constructor: must actually load the library
      real_handle = dlopen(libname, RTLD_LAZY | RTLD_LOCAL);

      printf("  [libTracer] dlopen(RTLD_LOCAL) of '%s' succeeded\n", libname);
    } else {
      // We're in a wrapper: library should already be loaded.
      // If we aren't (like we are in the real constructor),
      // RTLD_NOLOAD ensures we will get am handle
      real_handle = dlopen(libname, RTLD_LAZY | RTLD_NOLOAD);
      printf("  [libTracer] dlopen(RTLD_NOLOAD) of '%s' succeeded\n", libname);
    }
  }

  assert(real_handle && "Failed to obtain handle to real library");

  // 2.2 Look up symbol in the real library
  real_func = (func_t)dlsym(real_handle, func_name);
  if (!real_func) {
    if (from_ctor) {
#if VERBOSE_LEVEL > 0
      printf("  [libTracer] Warning: Symbol '%s' not found\n", func_name);
      printf("  [libTracer] Will crash if called\n");
#endif
      return;
    }
    fprintf(stderr, "  [libTracer] FATAL: Symbol '%s' not found\n", func_name);
    exit(1);
  }
  printf("  [libTracer] Symbol '%s' found via dlsym\n", func_name);

  // 3. Avoid Infinit recursion when called
  Dl_info my_info, real_info;
  if (dladdr((void *)resolve, &my_info) &&
      dladdr((void *)real_func, &real_info)) {
    if (my_info.dli_fbase == real_info.dli_fbase) {
      printf("  [libTracer] Fatal: Symbol '%s' resolved inside the Tracer\n",
             func_name);
      exit(1);
    }
  }
  // 4. Cache the resolved function
  *cache_ptr = real_func;
}

// To be faster, no thread safety.
// In the happy case, our constructor will set the cache.
// In the pathological case, when the real lib contructor call some symbol AND
// are multithreaded, they will jut do extra work
static func_t real_A = NULL;
static func_t real_B = NULL;

// Forward declarations of our wrappers
void A(void);
void B(void);

__attribute__((constructor)) static void init_tracer(void) {
  printf("  [libTracer] Initializing tracer (via ctor)\n");

  // Pre-resolve symbols during initialization
  // to reduce runtime overhead and "solve" thread-safety
  if (__builtin_expect(!real_A, 0))
    resolve("A", (void *)A, &real_A, true);

  if (__builtin_expect(!real_B, 0))
    resolve("B", (void *)B, &real_B, true);
}

__attribute__((destructor)) static void cleanup_tracer(void) {
  if (real_handle) {
    dlclose(real_handle);
    real_handle = NULL;
  }
}

#define DEFINE_WRAPPER(NAME)                                                   \
  void NAME(void) {                                                            \
    printf("  [libTracer] Intercepted " #NAME "\n");                           \
    /* Lazy resolve if constructor didn't run or failed */                     \
    if (__builtin_expect(!real_##NAME, 1))                                     \
      resolve(#NAME, (void *)NAME, &real_##NAME, false);                       \
    real_##NAME();                                                             \
  }

DEFINE_WRAPPER(A)
DEFINE_WRAPPER(B)
