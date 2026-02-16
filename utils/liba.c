#include <stdio.h>
#include <stdbool.h>
#include <utils/liba.h>

bool A(void) { printf("  [libA] Executing A\n"); return true; }
bool B_v1(void) { puts("[liba] Executing Bv1\n"); return true; }
bool B_v2(void) { puts("[liba] Executing Bv2 (DEFAULT)\n"); return true; }

__asm__(".symver B_v1, B@LIBA_1.0");
__asm__(".symver B_v2, B@@LIBA_2.0");
