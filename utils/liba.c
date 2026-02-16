#include <stdio.h>
#include <stdbool.h>
#include <utils/liba.h>
#include <stdint.h>

uint64_t A(void) { printf("  [libA] Executing A\n"); return 0; }
uint64_t B_v1(void) { printf("[liba] Executing Bv1\n"); return 1; }
uint64_t B_v2(void) { printf("[liba] Executing Bv2 (DEFAULT)\n"); return 2; }

__asm__(".symver B_v1, B@LIBA_1.0");
__asm__(".symver B_v2, B@@LIBA_2.0");
