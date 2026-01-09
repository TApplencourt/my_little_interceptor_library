#!/usr/bin/env bats

assert_output() {
  diff <(echo "$output") <(echo "$1")
}

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=.
   make liba.so
   make -C ../ libtracer_dlopened.so libtracer_linked.so
}

@test "Main dlopening | Tracer dlopening" {
  make main_dlopening
  run ./main_dlopening ../libtracer_dlopened.so

  assert_output "Loading: ../libtracer_dlopened.so
  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'A'
  [libTracer] dlopen(RTLD_LOCAL) of 'liba.so' succeeded
  [libTracer] Symbol 'A' found via dlsym
  [libTracer] Resolving symbol 'B'
  [Main] Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [Main] Error: ../libtracer_dlopened.so: undefined symbol: AA"
}

@test "Main dlopening | Tracer linked" {
  make main_dlopening
  TRACED_LIB=x run ./main_dlopening ../libtracer_linked.so

  assert_output "Loading: ../libtracer_linked.so
  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'A'
  [libTracer] Symbol 'A' found via RTLD_NEXT
  [libTracer] Resolving symbol 'B'
  [libTracer] dlopen(RTLD_LOCAL) of 'liba.so' succeeded
  [Main] Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [Main] Calling AA
  [libA] Executing AA"
}
