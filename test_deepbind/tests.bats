#!/usr/bin/env bats

assert_output() {
  diff <(echo "$output") <(echo "$1")
}

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=.
   make liba.so
   make -C ../ libtracer_dlopened.so
}

@test "Main dlopening | Tracer dlopening" {
  make main_deepbind
  run ./main_deepbind ../libtracer_dlopened.so

  assert_output "Loading: ../libtracer_dlopened.so
  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'A'
  [libTracer] dlopen(RTLD_LOCAL) of 'liba.so' succeeded
  [libTracer] Symbol 'A' found via dlsym
  [libTracer] Resolving symbol 'B'
  [libTracer] Symbol 'B' found via dlsym
  [Main] Calling A
  [libTracer] Intercepted A
  [liba] A called. invoking internal_secret...
  [liba] internal_secret called (CORRECT)"
}
