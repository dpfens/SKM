CC=gcc
CCFLAGS = -Wall -fopenmp

build:
	mkdir -p bin
	$(CC) $(CCFLAGS) -o bin/test src/test.c
