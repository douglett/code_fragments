CC=g++ -std=c++11 -Wall
SDLDIR=C:\mingw64\devlib\SDL2
CCFLAGS=-I. -I$(SDLDIR)\include
LDFLAGS=-L$(SDLDIR)\lib -lmingw32 -lSDL2main -lSDL2

# xdlib lib location
# XDLIB=../xd/xdlib

all: main.exe

# main exe
main.exe: main.o
	$(CC) $(LDFLAGS) -o main.exe main.o

# main file
main.o: main.cpp
	$(CC) $(CCFLAGS) -c -o main.o main.cpp

run: main.exe
	./main.exe
clean:
	rm -rf *.o *.out *.exe
