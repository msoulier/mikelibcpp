CC=g++
CFLAGS=-Wall -DLINUX --std=c++11 -I. -I..
LIBS=-L.. -lmikecpp
OS := $(shell uname -s)
DEBUG=0

ifeq ($(DEBUG),1)
	CFLAGS += -ggdb
endif

ifeq ($(OS),Darwin)
	CFLAGS+=-DDARWIN -I/usr/local/include
endif

ifeq ($(OS),Linux)
	CFLAGS+=-DLINUX
endif

all: testbuild

testbuild:
	$(CC) $(CFLAGS) -o test test.cpp $(LIBS)

test: testbuild
	./test

clean:
	rm -f *.a *.o test
