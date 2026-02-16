#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <utils/utils.h>

// Used to check for symbol leak
uint64_t A(void) {
  printf("  [libA] Executing A\n");
  return 0;
}

// Vicious check for dlopen re-entrance
__attribute__((constructor)) static void ctor(void) {
  printf("  [libA] ctor\n");
#ifdef DO_CHECK
  CHECK_RESULT(A());
#else
  A();
#endif
}

__attribute__((destructor)) static void dtor(void) {
  printf("  [libA] dtor\n");
#ifdef DO_CHECK
  CHECK_RESULT(A());
#else
  A();
#endif
}
