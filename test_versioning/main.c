#include <stdio.h>
#include <assert.h>
#include <utils/liba.h>
#include <utils/utils.h>
#include <inttypes.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// TOSTRING(VERSYM) → TOSTRING(1.0) → "1.0"
#ifdef VERSYM
__asm__(".symver B, B@LIBA_" TOSTRING(VERSYM));
#endif

int main(void) {
#ifdef VERSYM
    puts("[main] calling B@LIBA_" TOSTRING(VERSYM));
#else
    puts("[main] calling B (default version)");
#endif

    uint64_t b = B();
    CHECK_RESULT(b);

#ifdef VERSYM
    if (strcmp(TOSTRING(VERSYM), "1.0") == 0) {
        assert((b & 1) && "Expected bit 0 set for version 1.0");
    } else {
	    assert((b & 2) && "Expected bit 1 set for version 2.0");
    }
#else
    assert((b & 2) && "Expected bit 1 set when default");
#endif

    return 0;
}
