CC=g++
CFLAGS=-Wall -DLINUX --std=c++11 -I. -I..
LIBS=-L.. -lmikecpp
OS := $(shell uname -s)
DEBUG=0

ifeq ($(DEBUG),1)
	CFLAGS += -ggdb
endif

#ifeq ($(threads),1)
#    CFLAGS += -DMIKELIBC++_THREADS
#endif

ifeq ($(OS),Darwin)
	LDFLAGS+=-L/usr/local/lib
	CFLAGS+=-DDARWIN -I/usr/local/include
	LIBS+=-lpthread
endif

ifeq ($(OS),Linux)
	LDFLAGS+=-L/usr/lib/x86_64-linux-gnu
	CFLAGS+=-DLINUX -static
	LIBS+=-lpthread
endif

all: testbuild

testbuild:
	$(CC) $(CFLAGS) -o test test.cpp $(LIBS)

test: testbuild
	./test

clean:
	rm -f *.a *.o test
