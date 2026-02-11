#!/usr/bin/env bats

bats_require_minimum_version 1.5.0

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=.
   make liba.so
   make -C ../ libtracer_dlopened.so libtracer_linked.so
}

@test "Main dlopening | Tracer dlopening" {
  make main_dlopening
  TRACING_ON=1 run -1 ./main_dlopening ../libtracer_dlopened.so
}

@test "Main dlopening | Tracer linked" {
  make main_dlopening
  TRACING_ON=1 ./main_dlopening ../libtracer_linked.so
}

@test "Main linked | Tracer dlopening" {
  make main_linked
  TRACING_ON=1 LD_PRELOAD=../libtracer_dlopened.so ./main_linked
}

@test "Main linked | Tracer linked" {
  make main_linked
  TRACING_ON=1 LD_PRELOAD=../libtracer_linked.so ./main_linked
}

