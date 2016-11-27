include Makefile.inc
HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
CFLAGS=-Wall -pedantic -std=c11 -D_POSIX_C_SOURCE=200112L
LDFLAGS=-lrt -lpthread
EXE=bubble_sort

.PHONY: clean

all: CFLAGS += -g
all: $(EXE)

release: CFLAGS += -O3
release: $(EXE)
$(EXE): src/main.c
	@echo "linking $@..."
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@echo "cleaning..."
	@rm -rf $(EXE)

doc: doc/index.html

doc/index.html: Doxyfile $(HEADERS) $(SOURCES)
	@echo "generating doc with doxygen..."
	@doxygen