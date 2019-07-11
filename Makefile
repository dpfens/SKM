CC=gcc
CCFLAGS = -Wall -fopenmp -g
SRC=./src/
BIN=./bin/
PYTHON27CFLAGS = $(shell python-config --cflags)
PYTHON27INCLUDES = $(shell python-config --includes)
PYTHON27LDFLAGS = $(shell python-config --ldflags)
PYTHON3CFLAGS = $(shell python3-config --cflags)
PYTHON3INCLUDES = $(shell python3-config --includes)
PYTHON3LDFLAGS = $(shell python3-config --ldflags)

build:
	mkdir -p $(BIN)
	python3 $(SRC)python/setup.py build_ext --inplace

test:
	mkdir -p $(BIN)
	$(CC) $(CCFLAGS) -o $(BIN)test $(SRC)test.c -lm

python:
	python3 $(SRC)python/setup.py install
