CC=gcc
CFLAGS=Wall -g -c

all: btreebenchmarkwrites

btreebenchmarkwrites: btreebenchmarkwrites.o btreeproj.o
	$(CC) -o btreebenchmarkwrites btreeproj.o btreebenchmarkwrites.o

btreebenchmarkwritesMEMORY: btreebenchmarkwritesMEM.o btreereduxMEM.o
	$(CC) -o btreebenchmarkwritesMEMORY btreereduxMEM.o btreebenchmarkwritesMEM.o

btreebenchmarkwritesMEM.o: btreebenchmarkwritesMEM.c
	$(CC) $(CFLAGS) -o btreebenchmarkwritesMEM.o btreebenchmarkwritesMEM.c

btreebenchmarkreads: btreebenchmarkreads.o btreeproj.o
	$(CC) -o btreebenchmarkreads btreeproj.o btreebenchmarkreads.o

btreebenchmarkreadsMEMORY: btreebenchmarkreadsMEM.o btreereduxMEM.o
	$(CC) -o btreebenchmarkreadsMEMORY btreereduxMEM.o btreebenchmarkreadsMEM.o

btreebenchmarkreadsMEM.o: btreebenchmarkreadsMEM.c
	$(CC) $(CFLAGS) -o btreebenchmarkreadsMEM.o btreebenchmarkreadsMEM.c

btreeproj: btreeproj.o
	$(CC) -o btreeproj btreeproj.o

btreetestsredux: btreetests.o btreeredux.o
	$(CC) -o btreetestsredux btreeredux.o btreetests.o

btreetestsmemonly: btreetestsMEM.o btreereduxMEM.o
	$(CC) -o btreetestsmemonly btreereduxMEM.o btreetestsMEM.o

btreetestsMEM.o: btreetestsMEM.c
	$(CC) $(CFLAGS) -o btreetestsMEM.o btreetestsMEM.c

btreereduxMEM.o: btreereduxMEM.c btreeprojMEM.h
	$(CC) $(CFLAGS) -o btreereduxMEM.o btreereduxMEM.c

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

btreeredux.o: btreeredux.c btreeproj.h
	$(CC) $(CFLAGS) -o btreeredux.o btreeredux.c

clean: rm -f btreeproj *.o