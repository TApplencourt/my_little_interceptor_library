CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: main_linked main_dlopening

../utils/liba.so:
	$(MAKE) -C ../utils liba.so

main_linked: main_linked.c ../utils/liba.so
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main_linked main_dlopening
