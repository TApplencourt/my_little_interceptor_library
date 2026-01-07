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
  [libTracer] dlopen of liba.so succeeded
  [Main] Calling A
  [libTracer] Intercepted A
  [libTracer] Finding Symbol A
  [libA] Executing A"
}

@test "Main dlopening | Tracer linked" {
  make main_dlopening
  TRACED_LIB=x run ./main_dlopening ../libtracer_linked.so

  assert_output "Loading: ../libtracer_linked.so
  [libTracer] dlopen failed: x: cannot open shared object file: No such file or directory
  [Main] Calling A
  [libTracer] Intercepted A
  [libTracer] Finding Symbol A
  [libA] Executing A"
}

@test "Main linked | Tracer dlopening" {
  make main_linked
  TRACED_LIB=x LD_PRELOAD=../libtracer_dlopened.so run ./main_linked

  assert_output "  [libTracer] dlopen failed: x: cannot open shared object file: No such file or directory
  [Main] Calling A
  [libTracer] Intercepted A
  [libTracer] Finding Symbol A
  [libA] Executing A"
}

@test "Main linked | Tracer linked" {
  make main_linked
  TRACED_LIB=x LD_PRELOAD=../libtracer_linked.so run ./main_linked

  assert_output "  [libTracer] dlopen failed: x: cannot open shared object file: No such file or directory
  [Main] Calling A
  [libTracer] Intercepted A
  [libTracer] Finding Symbol A
  [libA] Executing A"
}

