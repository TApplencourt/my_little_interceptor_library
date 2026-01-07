#include <stdio.h>

// Used to check for symbol leak
void A(void) { printf("  [libA] Executing A\n"); }

// Vicious check for dlopen re-entrance
__attribute__((constructor)) static void ctor(void) {
  printf("  [libA] ctor\n");
  A();
}

__attribute__((destructor)) static void dtor(void) {
  printf("  [libA] dtor\n");
  A();
}
