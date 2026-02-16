#!/usr/bin/env bats

setup_file() {
   cd $(dirname "$BATS_TEST_FILENAME")
   export LD_LIBRARY_PATH=../utils/
   make -C ../utils liba.so
   make -C ../ libtracer_linked.so
}

@test "Main linked (default) | Tracer linked" {
  make main_default
  TRACING_ON=1 LD_PRELOAD=../libtracer_linked.so ./main_default
}

@test "Main linked (v1) | Tracer linked" {
  make main_v1
  TRACING_ON=1 LD_PRELOAD=../libtracer_linked.so ./main_v1
}

@test "Main linked (v2) | Tracer linked" {
  make main_v2
  TRACING_ON=1 LD_PRELOAD=../libtracer_linked.so ./main_v2
}


