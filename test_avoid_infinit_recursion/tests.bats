#!/usr/bin/env bats
bats_require_minimum_version 1.5.0

assert_output() {
  diff <(echo "$output") <(echo "$1")
}

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=.
   make liba.so
   make -C ../ libtracer_dlopened.so
}

@test "Main dlopening | Tracer tracing himself" {
  make main_dlopening
  REAL_LIB=../libtracer_dlopened.so run -1 ./main_dlopening ../libtracer_dlopened.so
  
  assert_output "Loading: ../libtracer_dlopened.so
  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'A'
  [libTracer] dlopen(RTLD_LOCAL) of '../libtracer_dlopened.so' succeeded
  [libTracer] Symbol 'A' found via dlsym
  [libTracer] Fatal: Symbol 'A' resolved inside the Tracer"
}

