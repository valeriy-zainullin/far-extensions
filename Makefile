CFLAGS = -Wall -pedantic -Wextra -Wshadow -Werror -std=c11 -Wno-unused-function -Iinclude
LDFLAGS = -static -shared -lole32
RELEASE_CFLAGS = -O2 -DNDEBUG
RELEASE_LDFLAGS = -s
DEBUG_CFLAGS = -g #-fsanitize=address,pointer-compare,pointer-subtract,leak,undefined

SOURCES = $(notdir $(wildcard src/*.c))
TARGETS_X86 = $(addprefix build_x86/, $(SOURCES:.c=.dll))
TARGETS_X86-64 = $(addprefix build_x86-64/, $(SOURCES:.c=.dll))
TARGETS_X86_DEBUG = $(addprefix build_x86_debug/, $(SOURCES:.c=.dll))
TARGETS_X86-64_DEBUG = $(addprefix build_x86-64_debug/, $(SOURCES:.c=.dll))
BUILD_DIRS = build_x86 build_x86-64 build_x86_debug build_x86-64_debug

.PHONY: all clean x86 x86-64 x86_debug x86-64_debug
all: x86 x86-64 x86_debug x86-64_debug
x86: $(TARGETS_X86)
x86-64: $(TARGETS_X86-64)
x86_debug: $(TARGETS_X86_DEBUG)
x86-64_debug: $(TARGETS_X86-64_DEBUG)

clean:
	rm -rf $(BUILD_DIRS)

$(BUILD_DIRS):
	mkdir $@

$(addsuffix /rebuild, $(BUILD_DIRS)): %/rebuild : | % 
	mkdir $@
	
$(TARGETS_X86): build_x86/%.dll : src/%.c | build_x86 build_x86/rebuild
	i686-w64-mingw32-gcc $(CFLAGS) $(RELEASE_CFLAGS) -MD -MF build_x86/rebuild/$(notdir $<).mk \
		-DTARGET=X86 -o $@ $< $(LDFLAGS) $(RELEASE_LDFLAGS)

$(TARGETS_X86-64): build_x86-64/%.dll : src/%.c | build_x86-64 build_x86-64/rebuild
	x86_64-w64-mingw32-gcc $(CFLAGS) $(RELEASE_CFLAGS) -MD -MF build_x86-64/rebuild/$(notdir $<).mk \
		-DTARGET=X86-64 -o $@ $< $(LDFLAGS) $(RELEASE_LDFLAGS)

$(TARGETS_X86_DEBUG): build_x86_debug/%.dll : src/%.c | build_x86_debug build_x86_debug/rebuild
	i686-w64-mingw32-gcc $(CFLAGS) $(DEBUG_CFLAGS) -MD -MF build_x86_debug/rebuild/$(notdir $<).mk \
		-DTARGET=X86 -o $@ $< $(LDFLAGS) $(DEBUG_LDFLAGS)

$(TARGETS_X86-64_DEBUG): build_x86-64_debug/%.dll : src/%.c | build_x86-64_debug build_x86-64_debug/rebuild
	x86_64-w64-mingw32-gcc $(CFLAGS) $(DEBUG_CFLAGS) -MD -MF \
		build_x86-64_debug/rebuild/$(notdir $<).mk -DTARGET=X86-64 -o $@ $< $(LDFLAGS)

include $(wildcard build_*/rebuild/*)