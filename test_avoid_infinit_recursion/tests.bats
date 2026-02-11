#!/usr/bin/env bats
bats_require_minimum_version 1.5.0

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   make -C ../ libtracer_dlopened.so
}

@test "Main dlopening | Tracer tracing himself" {
  make -C ../utils main_dlopening
  REAL_LIB=../libtracer_dlopened.so run -2 ../utils/main_dlopening ../libtracer_dlopened.so
}

