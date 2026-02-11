#pragma once

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define CHECK_RESULT(result) do { \
    const char* tracing = getenv("TRACING_ON"); \
    if (tracing != NULL && strcmp(tracing, "1") == 0) { \
        /* TRACING_ON=1 → expect False */ \
        assert((result) == false && "Expected false when TRACING_ON=1"); \
    } else { \
        /* TRACING_ON not set or TRACING_ON=0 → expect True */ \
        assert((result) == true && "Expected true when TRACING_ON not set or =0"); \
    } \
} while(0)

#define CHECK_RESULT_NOT_TRACED(result) do { \
  assert((result) == true && "Expected true"); \
} while(0)

void call_symbol(void *handle, const char *name);
void call_symbol_check_not_traced(void *handle, const char *name);

