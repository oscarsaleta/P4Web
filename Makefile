CC=g++
CFLAGS=-std=gnu++14 -Wall -g
LFLAGS=-lwt
HTTPFLAGS=-lwthttp
SOURCES=$(wildcard src/*.cc)
OBJECTS=$(patsubst src/%.cc,bin/%.o,$(SOURCES))
EXECUTABLE=bin/main

.PHONY: all build clean run debug test

target: build all
	
all: $(EXECUTABLE)

# Main executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(HTTPFLAGS) $(LFLAGS)

# Objects compilation
$(OBJECTS): bin/%.o : src/%.cc
	$(CC) $(CFLAGS) -c $< $(LFLAGS) -o $@


# Build (mkdir bin)
build:
	@mkdir -p bin

# Cleaning directives
clean:
	-rm $(OBJECTS) $(EXECUTABLE)

# Running directives
run: all
	$(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080
	
debug: all
	gdb --args $(EXECUTABLE) --docroot . --http-address 0.0.0.0 --http-port 8080