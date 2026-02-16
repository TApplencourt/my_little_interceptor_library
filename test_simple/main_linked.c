#include <stdio.h>
#include <utils/liba.h>
#include <utils/utils.h>

int main(int argc, char *argv[]) {
  printf("  [Main] Calling A\n");
  CHECK_RESULT(A());
  printf("  [Main] Calling B\n");
  CHECK_RESULT(B());
  return 0;
}
