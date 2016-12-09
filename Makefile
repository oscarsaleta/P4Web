CC=g++
CFLAGS=-std=gnu++14 -Wall -g
LFLAGS=-lwt -lboost_system
HTTPFLAGS=-lwthttp
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

SRCDIR=src
BINDIR=bin
DEPDIR=.d
EXECUTABLE=$(BINDIR)/main

SOURCES=$(wildcard $(SRCDIR)/*.cc)
OBJECTS=$(patsubst $(SRCDIR)/%.cc,$(BINDIR)/%.o,$(SOURCES))
DEPENDENCIES=$(patsubst $(SRCDIR)/%.cc,$(DEPDIR)/%.d,$(SOURCES))


COMPILE_EXE=$(CC) $(DEPFLAGS) $(CFLAGS) $(OBJECTS) -o $@ $(HTTPFLAGS) $(LFLAGS)
COMPILE_OBJ=$(CC) $(DEPFLAGS) $(CFLAGS) -c
POSTCOMPILE=mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d


.PHONY: all build clean run debug test

target: build all

# Build (mkdir bin .d)
build:
	@mkdir -p bin $(DEPDIR)
	
all: $(EXECUTABLE)

# Main executable
$(EXECUTABLE): $(OBJECTS)
	$(COMPILE_EXE)

# Objects compilation
$(OBJECTS): bin/%.o : src/%.cc
	$(COMPILE_OBJ) $< $(LFLAGS) -o $@
	$(POSTCOMPILE)


# Cleaning directives
clean:
	-rm $(OBJECTS) $(EXECUTABLE) $(DEPENDENCIES)


# Running directives
run: all
	$(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080
	
debug: all
	gdb --args $(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080


# Handling dependencies
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(DEPENDENCIES)