CC = g++
CFLAGS = -Wall

all: oss worker

oss: src/oss.cpp
	$(CC) $(CFLAGS) -o bin/oss src/oss.cpp

worker: src/worker.cpp
	$(CC) $(CFLAGS) -o bin/worker src/worker.cpp

clean:
	rm -f bin/oss bin/worker
