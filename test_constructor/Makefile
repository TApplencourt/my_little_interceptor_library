CC ?= gcc
CFLAGS ?= -g -Wall -Wpedantic  -I ../

.PHONY: all clean

all: check/liba.so no_check/liba.so 

check/liba.so: liba.c
	mkdir -p check
	$(CC) $(CFLAGS) -fPIC -shared -DO_CHECK -o $@ $<

no_check/liba.so: liba.c
	mkdir -p no_check
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

clean:
	rm -rf check/ no_check/
