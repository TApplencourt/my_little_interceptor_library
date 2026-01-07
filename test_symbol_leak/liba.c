#include <stdio.h>

void _secret_A(void) {};
void A(void) {
  _secret_A();
  printf("  [libA] Executing A\n");
}
