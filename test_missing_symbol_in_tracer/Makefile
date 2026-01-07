CC ?= gcc
CFLAGS ?= -g -Wall

.PHONY: all clean

all: liba.so main_linked main_dlopening

liba.so: liba.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

main_linked: main_linked.c liba.so
	$(CC) $(CFLAGS) -o $@ liba.so main_linked.c

main_dlopening: main_dlopening.c
	$(CC) $(CFLAGS) -o $@ $< -ldl

clean:
	rm -f liba.so main_linked main_dlopening
