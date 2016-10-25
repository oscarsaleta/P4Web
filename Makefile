CFLAGS= -std=gnu++14 -Wall -g
LFLAGS= -lwt
HTTPFLAGS= -lwthttp

target: all
	
all: main

# Main executable
main: main.o MyApplication.o MainUI.o HomeLeft.o HomeRight.o
	g++ $(CFLAGS) main.o MyApplication.o MainUI.o HomeLeft.o HomeRight.o -o main $(HTTPFLAGS) $(LFLAGS)

# Objects compilation
main.o: main.cc
	g++ $(CFLAGS) -c main.cc -lwhttp $(LFLAGS)

MyApplication.o: MyApplication.cc MyApplication.h
	g++ $(CFLAGS) -c MyApplication.cc $(LFLAGS)

MainUI.o: MainUI.cc MainUI.h
	g++ $(CFLAGS) -c MainUI.cc $(LFLAGS)

HomeLeft.o: HomeLeft.cc HomeLeft.h
	g++ $(CFLAGS) -c HomeLeft.cc $(LFLAGS)

HomeRight.o: HomeRight.cc HomeRight.h
	g++ $(CFLAGS) -c HomeRight.cc $(LFLAGS)




# Cleaning directives
clean:
	rm *o main

# Running directives
run: all
	./main --docroot . --http-address 0.0.0.0 --http-port 8080
	
debug: all
	gdb --args ./main --docroot . --http-address 0.0.0.0 --http-port 8080