CC ?= gcc
CFLAGS ?= -g -Wall

.PHONY: all clean

all: liba.so main_deepbind

liba.so: liba.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

main_deepbind: main_deepbind.c
	$(CC) $(CFLAGS) -rdynamic -o $@ $< -ldl

clean:
	rm -f liba.so main_deepbind
