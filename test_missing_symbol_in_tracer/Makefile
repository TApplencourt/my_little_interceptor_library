CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: liba.so main_dlopening main_linked

liba.so: liba.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

main_linked: main_linked.c liba.so
	$(CC) $(CFLAGS) -o $@ $^

main_dlopening: main_dlopening.c ../utils/utils.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl

clean:
	rm -f liba.so main_linked main_dlopening
