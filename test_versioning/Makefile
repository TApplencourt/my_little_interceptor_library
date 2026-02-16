CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: main_v1 main_v2 main_default

../utils/liba.so:
	$(MAKE) -C ../utils liba.so

main_v1: main.c ../utils/liba.so
	$(CC) -DVERSYM=1.0 $(CFLAGS) -o $@ $^

main_v2: main.c ../utils/liba.so
	$(CC) -DVERSYM=2.0 $(CFLAGS) -o $@ $^

main_default: main.c ../utils/liba.so
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main_v1 main_v2 main_default
