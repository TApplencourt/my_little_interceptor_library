#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <utils/utils.h>
#include <utils/liba.h>

void call_symbol(void *handle, const char *name) {

  union {
     void *obj;
     liba_func_t func;
   } alias;

  alias.obj = dlsym(handle, name);
  liba_func_t func = alias.func;
  if (func != NULL) {
    printf("  [Main] Calling %s\n", name);
    CHECK_RESULT(func());
  } else {
    char *error = dlerror();
    printf("  [Main] Error: %s\n", error);
    exit(1);
  }
}

void call_symbol_check_not_traced(void *handle, const char *name){ 

  union {
     void *obj;
     liba_func_t func;
   } alias;

  alias.obj = dlsym(handle, name);
  liba_func_t func = alias.func;
  if (func != NULL) {
    printf("  [Main] Calling %s\n", name);
    CHECK_RESULT_NOT_TRACED(func());
  } else {
    char *error = dlerror();
    printf("  [Main] Error: %s\n", error);
    exit(1);
  }
}

