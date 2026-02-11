#!/usr/bin/env bats

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=../utils/
   make -C ../utils liba.so
   make -C ../ libtracer_dlopened.so libtracer_linked.so
}

@test "Main dlopening | Tracer dlopening" {
  make main_dlopening
  TRACING_ON=1 ./main_dlopening ../utils/liba.so ../libtracer_dlopened.so
}

@test "Main dlopening | Tracer linked" {
  make main_dlopening  
  TRACING_ON=1 ./main_dlopening ../utils/liba.so ../libtracer_linked.so
}
