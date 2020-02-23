BIN = bvis
CFLAGS += -std=c99 -Wall -Wextra -Wpedantic

SOURCES = $(wildcard *.c)
bvis: $(SOURCES:.c=.o)

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	$(RM) -- $(BIN) *.o *.d

.PHONY: distclean
distclean: clean
	$(RM) -- tags cscope.out

tags: $(SOURCES) $(wildcard $(SOURCES:.c=.d))
	ctags -R .

ifeq ($(filter-out clean distclean,$(MAKECMDGOALS)),$(MAKECMDGOALS))
    # create .d files
    CPPFLAGS += -MD
    # include .d files
    include $(wildcard $(SOURCES:.c=.d))
endif

# prune some implicit rules to limit rule search
Makefile: ;
%.c %.d %.h: ;

.SUFFIXES:
.SUFFIXES: .o .c
.DEFAULT_GOAL := all
