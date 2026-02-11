#!/usr/bin/env bats

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   make liba.so
   make -C ../ libtracer_dlopened.so
}

@test "Main dlopening | Tracer dlopening" {
  make main_deepbind
  TRACING_ON=1 LD_LIBRARY_PATH=. ./main_deepbind ../libtracer_dlopened.so
}
