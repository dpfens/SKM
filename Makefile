CC=gcc
CCFLAGS = -Wall -fopenmp -g
SRC=./src/
BIN=./bin/
PYTHON27CFLAGS = $(shell python-config --cflags)
PYTHON27INCLUDES = $(shell python-config --includes)
PYTHON27LDFLAGS = $(shell python-config --ldflags)
PYTHON3CFLAGS = $(shell python3.7-config --cflags)
PYTHON3INCLUDES = $(shell python3.7-config --includes)
PYTHON3LDFLAGS = $(shell python3.7-config --ldflags)

build:
	mkdir -p $(BIN)
	python3 $(SRC)python/setup.py build_ext --inplace
	$(CC) $(CCFLAGS) -c $(SRC)py_skm.c $(PYTHON27CFLAGS) -o $(BIN)py_skm27.o
	$(CC) $(CCFLAGS) -shared -o $(SRC)py_skm27.o $(PYTHON27LDFLAGS) -o $(BIN)py_skm27.so

	$(CC) $(CCFLAGS) -c $(SRC)py_skm.c $(PYTHON3CFLAGS) -o $(BIN)py_skm3.o
	$(CC) $(CCFLAGS) -shared $(PYTHON3LDFLAGS) -o $(BIN)py_skm3.so

test:
	mkdir -p $(BIN)
	$(CC) $(CCFLAGS) -o $(BIN)test $(SRC)test.c -lm

python:
	python3 $(SRC)python/setup.py install
