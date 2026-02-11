#include <stdio.h>
#include <stdbool.h>
#include <utils/liba.h>

bool A(void) { printf("  [libA] Executing A\n"); return true; }
bool B(void) { printf("  [libA] Executing B\n"); return true; }

