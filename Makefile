CC=gcc
CFLAGS=-fPIC -Wall

PATCHELF=patchelf
ALL= libtracer_linked.so libtracer_dlopened.so

all: $(ALL)

# VERSION 1: Patched
libtracer_linked.so: libtracer.c
	$(CC) $(CFLAGS) -shared -lpthread -ldl -o libtracer_linked.so libtracer.c
	@echo "Adding liba.so dependency to libtracer.so via patchelf..."
	$(PATCHELF) --add-needed liba.so libtracer_linked.so

# VERSION 2: No Patch (Forces manual dlopen)
libtracer_dlopened.so: libtracer.c
	$(CC) $(CFLAGS) -shared -lpthread -ldl -o libtracer_dlopened.so libtracer.c

clean:
	rm -f $(ALL)
	@for dir in utils/ test_*/; do [ -d "$$dir" ] && $(MAKE) -C "$$dir" clean || true; done

