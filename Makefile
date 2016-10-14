FLAGS= -std=gnu++11 -Wall -g
TAIL= -lwt

target: all
	
all: main

# Main executable
main: main.o MyApplication.o MainUI.o HomeLeft.o HomeRight.o
	g++ $(FLAGS) main.o MyApplication.o MainUI.o HomeLeft.o HomeRight.o -o main -lwthttp $(TAIL)

# Objects compilation
main.o: main.cc
	g++ $(FLAGS) -c main.cc -lwhttp $(TAIL)

MyApplication.o: MyApplication.cc MyApplication.h
	g++ $(FLAGS) -c MyApplication.cc $(TAIL)

MainUI.o: MainUI.cc MainUI.h
	g++ $(FLAGS) -c MainUI.cc $(TAIL)

HomeLeft.o: HomeLeft.cc HomeLeft.h
	g++ $(FLAGS) -c HomeLeft.cc $(TAIL)

HomeRight.o: HomeRight.cc HomeRight.h
	g++ $(FLAGS) -c HomeRight.cc $(TAIL)




# Cleaning directives
clean:
	rm *o main

# Running directives
run: all
	./main --docroot . --http-address 0.0.0.0 --http-port 8080
	
debug: all
	gdb --args ./main --docroot . --http-address 0.0.0.0 --http-port 8080