CC ?= gcc
CFLAGS ?= -g -Wall

.PHONY: all clean

all: liba.so main_dlopening

liba.so: liba.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

main_dlopening: main_dlopening.c
	$(CC) $(CFLAGS) -o $@ $< -ldl

clean:
	rm -f liba.so main_linked 
