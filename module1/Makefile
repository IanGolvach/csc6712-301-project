CC=gcc
CFLAGS=Wall -g -c

all: btreeproj

btreeproj: btreeproj.o
	$(CC) -o btreeproj btreeproj.o

btreeproj.o: btreeproj.c
	$(CC) $(CFLAGS) -o btreeproj.o btreeproj.c

clean: -f ttsh *.o