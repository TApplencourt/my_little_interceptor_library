#!/usr/bin/env bats
setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   make -C ../ libtracer_dlopened.so libtracer_linked.so
}

@test "Main dlopening | Tracer dlopening" {
  make -C ../utils main_dlopening
  make no_check/liba.so
  LD_LIBRARY_PATH=./no_check TRACING_ON=1 ../utils/main_dlopening ../libtracer_dlopened.so
}

@test "Main dlopening | Tracer dlopening | LD_PRELOAD" {
  make -C ../utils main_dlopening
  make no_check/liba.so
  LD_LIBRARY_PATH=./no_check TRACING_ON=1 LD_PRELOAD=../libtracer_dlopened.so ../utils/main_dlopening ../libtracer_dlopened.so
}

@test "Main dlopening | Tracer linked" {
  make -C ../utils main_dlopening
  make check/liba.so
  LD_LIBRARY_PATH=./check TRACING_ON=1 ../utils/main_dlopening ../libtracer_linked.so
}
