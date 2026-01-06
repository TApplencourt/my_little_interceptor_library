#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*simple_func)(void);

extern void A(void);
extern void A1(void);
extern void A2(void);

int main(int argc, char *argv[]) {
  printf("  [ProgramC] Calling A1\n");
  A1();
  printf("  [ProgramC] Calling A2\n");
  A2();
  return 0;
}
