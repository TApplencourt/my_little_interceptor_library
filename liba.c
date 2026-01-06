#include <stdio.h>

void _secret_A(void) {};
void A(void) {
  _secret_A();
  printf("  [libA] Executing A\n");
}

void A1(void) { printf("  [libA] Executing A1\n"); }

void A2(void) {
  printf("  [libA] Executing A2: Calling A\n");
  A();
  printf("  [libA] Executing A2: Exiting\n");
}
