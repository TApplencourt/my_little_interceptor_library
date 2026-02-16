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

static void resolve_versioned(const char *func_name, const char *version,
                              const void *wrapper_addr, void **cache_ptr,
                              bool from_ctor) {
    printf("  [libTracer] Resolving symbol versioned '%s@%s'\n", func_name, version);

    // Try dlvsym with RTLD_NEXT
    void *real_func = dlvsym(RTLD_NEXT, func_name, version);
    if (real_func) {
        printf("  [libTracer] Symbol '%s@%s' found via RTLD_NEXT\n",
               func_name, version);
        *cache_ptr = real_func;
        return;
    }

    // Fallback to manual dlopen with dlvsym
    // ... rest of your logic using dlvsym instead of dlsym
}

static void resolve(const char *func_name, const void *wrapper_addr,
                    void **cache_ptr, bool from_ctor) {

  printf("  [libTracer] Resolving symbol '%s'\n", func_name);

  // Strategy 1: Try RTLD_NEXT (works if we are LD_PRELOADed or we are linked we the tracee lib)
  void *real_func = dlsym(RTLD_NEXT, func_name);
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
      printf("  [libTracer] dlopen(RTLD_LOCAL) of '%s'...\n", libname);
      real_handle = dlopen(libname, RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);
    } else {
      // We're in a wrapper: library should already be loaded.
      // If we aren't (like we are in the real constructor),
      // RTLD_NOLOAD ensures we will get an handle
      printf("  [libTracer] dlopen(RTLD_NOLOAD) '%s'...\n", libname);
      real_handle = dlopen(libname, RTLD_LAZY | RTLD_NOLOAD);
    }
  }

  assert(real_handle && "Failed to obtain handle to real library");
  printf("  [libTracer] dlopen(RTLD_LOCAL)  succeeded\n");

  // 2.2 Look up symbol in the real library
  real_func = dlsym(real_handle, func_name);
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
      exit(2);
    }
  }
  // 4. Cache the resolved function
  *cache_ptr = real_func;
}

// To be faster, no thread safety.
// In the happy case, our constructor will set the cache.
// In the pathological case, when the real lib contructor call some symbol AND
// are multithreaded, they will jut do extra work
static void* real_casted_A = NULL;
static void* real_casted_B_v1 = NULL;
static void* real_casted_B_v2 = NULL;

// Forward declarations of our wrappers
uint64_t A(void);
uint64_t B_v1(void);
uint64_t B_v2(void);

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

__attribute__((constructor)) static void init_tracer(void) {
  printf("  [libTracer] Initializing tracer (via ctor)\n");

  // Pre-resolve symbols during initialization
  // to reduce runtime overhead and "solve" thread-safety
  if (likely(real_casted_A == NULL))
    resolve("A", (void *)A, &real_casted_A, true);

  if (likely(real_casted_B_v1 == NULL))
    resolve_versioned("B", "LIBA_1.0", (void *)B_v1, &real_casted_B_v1, true);

  if (likely(real_casted_B_v2 == NULL))
    resolve_versioned("B","LIBA_2.0", (void *)B_v2, &real_casted_B_v2, true);
}

__attribute__((destructor)) static void cleanup_tracer(void) {
  if (real_handle) {
    printf("  [libTracer] dlclose (via dtor)\n");
    dlclose(real_handle);
    real_handle = NULL;
  }
}

#define DEFINE_WRAPPER(NAME)                                                   \
   uint64_t NAME(void) {                                                       \
    printf("  [libTracer] Intercepted " #NAME "\n");                           \
    /* Lazy resolve if constructor didn't run or failed */                     \
    /* We will not take the branch most of the time*/                          \
    if (unlikely(real_casted_##NAME == NULL))                                  \
      resolve(#NAME, (void *)NAME, &real_casted_##NAME, false);                \
    /* Set the last bit to 1, */                                               \
    /* this allow us to check externaly if we are intercepted or not */        \
    return ((typeof(&NAME))real_casted_##NAME)() | (1ULL << 63);               \
}

DEFINE_WRAPPER(A)
__asm__(".symver B_v1, B@LIBA_1.0");
DEFINE_WRAPPER(B_v1)
__asm__(".symver B_v2, B@@LIBA_2.0");
DEFINE_WRAPPER(B_v2)
