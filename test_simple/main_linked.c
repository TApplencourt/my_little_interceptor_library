#include <stdio.h>

extern void A(void);

int main(int argc, char *argv[]) {
  printf("  [Main] Calling A\n");
  A();
  return 0;
}
