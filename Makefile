CC=g++
CFLAGS=-std=gnu++14 -Wall -g
LFLAGS=-lwt
HTTPFLAGS=-lwthttp

SOURCES=$(wildcard src/*.cc)
OBJECTS=$(patsubst src/%.cc,bin/%.o,$(SOURCES))
DEPENDENCIES_PATH=bin/dependencies/
DEPENDENCIES=$(patsubst src/%.cc,$(DEPENDENCIES_PATH)%.d,$(SOURCES))

EXECUTABLE=bin/main

.PHONY: all build clean run debug test

target: build all
	
all: $(EXECUTABLE)

# Main executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(HTTPFLAGS) $(LFLAGS)

# pull dependencies
-include $(DEPENDENCIES)

# Objects compilation
$(OBJECTS): bin/%.o : src/%.cc
	$(CC) $(CFLAGS) -c $< $(LFLAGS) -o $@
	$(CC) -MM $(CFLAGS) -c $< $(LFLAGS) > $(DEPENDENCIES_PATH)$*.d


# Build (mkdir bin)
build:
	@mkdir -p bin $(DEPENDENCIES_PATH)

# Cleaning directives
clean:
	-rm $(OBJECTS) $(EXECUTABLE) $(DEPENDENCIES)

# Running directives
run: all
	$(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080
	
debug: all
	gdb --args $(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080