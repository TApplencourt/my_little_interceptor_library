#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint64_t A(void) {
  printf("  [libA] Executing A\n");
  return 0;
}
uint64_t AA(void) {
  printf("  [libA] Executing AA\n");
  return 0;
}
