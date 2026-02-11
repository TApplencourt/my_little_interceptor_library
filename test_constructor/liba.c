#include <stdio.h>
#include <stdbool.h>
#include <utils/utils.h>

// Used to check for symbol leak
int A(void) { printf("  [libA] Executing A\n"); return true; }

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
