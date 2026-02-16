#include <stdint.h>
#include <stdio.h>

void _secret_A(void) {};
uint64_t A(void) {
  _secret_A();
  printf("  [libA] Executing A\n");
  return 0;
}
