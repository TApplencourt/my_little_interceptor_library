CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: main_dlopening

main_dlopening: main_dlopening.c ../utils/utils.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl

clean:
	rm -f main_dlopening
