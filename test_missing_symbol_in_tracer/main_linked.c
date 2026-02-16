#include <stdint.h>
#include <stdio.h>
#include <utils/utils.h>

// Yeah we don't have any liba header
extern uint64_t A(void);
extern uint64_t AA(void);

int main(int argc, char *argv[]) {
  printf("  [Main] Calling A\n");
  CHECK_RESULT(A());
  CHECK_RESULT_NOT_TRACED(AA());
  return 0;
}
