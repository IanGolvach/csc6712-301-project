CC=gcc
CFLAGS=Wall -g -c

all: btreebenchmarkwrites

btreebenchmarkwrites: btreebenchmarkwrites.o btreeproj.o
	$(CC) -o btreebenchmarkwrites btreeproj.o btreebenchmarkwrites.o

btreebenchmarkreads: btreebenchmarkreads.o btreeproj.o
	$(CC) -o btreebenchmarkreads btreeproj.o btreebenchmarkreads.o

btreeproj: btreeproj.o
	$(CC) -o btreeproj btreeproj.o

btreetests: btreetests.o btreeproj.o
	$(CC) -o btreetests btreeproj.o btreetests.o

btreetests.o: btreetests.c
	$(CC) $(CFLAGS) -o btreetests.o btreetests.c

btreebenchmarkwrites.o: btreebenchmarkwrites.c
	$(CC) $(CFLAGS) -o btreebenchmarkwrites.o btreebenchmarkwrites.c

btreebenchmarkreads.o: btreebenchmarkreads.c
	$(CC) $(CFLAGS) -o btreebenchmarkreads.o btreebenchmarkreads.c

btreeproj.o: btreeproj.c
	$(CC) $(CFLAGS) -o btreeproj.o btreeproj.c

clean: rm -f btreeproj *.o