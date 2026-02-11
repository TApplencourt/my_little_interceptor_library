CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: liba.so main_deepbind

liba.so: liba.c 
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

main_deepbind: main_deepbind.c ../utils/utils.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl

clean:
	rm -f liba.so main_deepbind
