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
  [libA] ctor
  [libA] Executing A
  [libTracer] dlopen(RTLD_LOCAL) of 'liba.so' succeeded
  [libTracer] Symbol 'A' found via dlsym
  [libTracer] Resolving symbol 'B'
  [libTracer] Symbol 'B' found via dlsym
  [Main] Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] dtor
  [libA] Executing A"
}

@test "Main dlopening | Tracer dlopening | LD_PRELOAD" {
  make main_dlopening
  LD_PRELOAD=../libtracer_dlopened.so run ./main_dlopening ../libtracer_dlopened.so

  assert_output "  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'A'
  [libA] ctor
  [libA] Executing A
  [libTracer] dlopen(RTLD_LOCAL) of 'liba.so' succeeded
  [libTracer] Symbol 'A' found via dlsym
  [libTracer] Resolving symbol 'B'
  [libTracer] Symbol 'B' found via dlsym
Loading: ../libtracer_dlopened.so
  [Main] Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] dtor
  [libA] Executing A"
}


@test "Main dlopening | Tracer linked" {
  make main_dlopening
  run ./main_dlopening ../libtracer_linked.so

  assert_output "Loading: ../libtracer_linked.so
  [libA] ctor
  [libTracer] Intercepted A
  [libTracer] Resolving symbol 'A'
  [libTracer] Symbol 'A' found via RTLD_NEXT
  [libA] Executing A
  [libTracer] Initializing tracer (via ctor)
  [libTracer] Resolving symbol 'B'
  [libTracer] Symbol 'B' found via RTLD_NEXT
  [Main] Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] dtor
  [libTracer] Intercepted A
  [libA] Executing A"
}
