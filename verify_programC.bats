#!/usr/bin/env bats

#libA export A and A1
#libtracer export A and B

# `program_dlopening` will try to load A,B and A1
@test "Test programC with liba.so" {
  run ./program_dlopening ./liba.so
  [ "$status" -eq 0 ]

  expected_output="Loading: ./liba.so
  [ProgramC] Calling A1
  [libA] Executing A1
  [ProgramC] Calling A2
  [libA] Executing A2: Calling A
  [libA] Executing A
  [libA] Executing A2: Exiting
  [ProgramC] Error: ./liba.so: undefined symbol: B"

  [ "${output}" = "${expected_output}" ]
}

@test "Test programC with libtracer_dlopened.so (cannot export A1)" {
  # Run programC with the library that does NOT have a patchelf dependency.
  # This forces the tracer to use its manual dlopen() fallback mechanism.
  # Hence we cannpt export D1
  run ./program_dlopening ./libtracer_dlopened.so

  [ "$status" -eq 0 ]

  expected_output="Loading: ./libtracer_dlopened.so
  [ProgramC] Error: ./libtracer_dlopened.so: undefined symbol: A1
  [ProgramC] Calling A2
  [libTracer] Intercepted A2
  [libTracer] dlopen succeeded
  [libA] Executing A2: Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] Executing A2: Exiting
  [ProgramC] Calling B
  [libTracer] Intercepted B
  [libTracer] Symbol 'B' not found in intercepted lib"

  [ "${output}" = "${expected_output}" ]
}

@test "Test programC with libtracer_linked.so (can export A1)" {
  # Run programC with the library that HAS been patched with a dependency on liba.so.
  # This allows the tracer's dlsym(RTLD_NEXT, ...) call to succeed.
  LD_LIBRARY_PATH=. run ./program_dlopening ./libtracer_linked.so
  [ "$status" -eq 0 ]

  expected_output="Loading: ./libtracer_linked.so
  [ProgramC] Calling A1
  [libA] Executing A1
  [ProgramC] Calling A2
  [libTracer] Intercepted A2
  [libA] Executing A2: Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] Executing A2: Exiting
  [ProgramC] Calling B
  [libTracer] Intercepted B
  [libTracer] dlopen succeeded
  [libTracer] Symbol 'B' not found in intercepted lib"
  
  [ "${output}" = "${expected_output}" ]
}

@test "Race condition, we dlopen once" {
  gcc -o program_dlopening_omp -fopenmp main.c -ldl
  run ./program_dlopening_omp ./libtracer_dlopened.so
  [ $(echo "$output" | grep -c "\[libTracer] dlopen succeeded") -eq 1 ]
}

# Program linked will try to load A and A1
#
@test "program_linked directly (Standard execution)" {
  run ./program_linked
  [ "$status" -eq 0 ]

  expected_output="  [ProgramC] Calling A1
  [libA] Executing A1
  [ProgramC] Calling A2
  [libA] Executing A2: Calling A
  [libA] Executing A
  [libA] Executing A2: Exiting" 

  [ "${output}" = "${expected_output}" ]
}

@test "program_linked (Intersepted)" {
  LD_PRELOAD=./libtracer_dlopened.so run ./program_linked
  [ "$status" -eq 0 ]

  expected_output="  [ProgramC] Calling A1
  [libA] Executing A1
  [ProgramC] Calling A2
  [libTracer] Intercepted A2
  [libA] Executing A2: Calling A
  [libTracer] Intercepted A
  [libA] Executing A
  [libA] Executing A2: Exiting"

  [ "${output}" = "${expected_output}" ]
}


