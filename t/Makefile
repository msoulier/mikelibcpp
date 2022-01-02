CC=g++
CFLAGS=-Wall -DLINUX --std=c++11 -I. -I..
LIBS=-L.. -lmikecpp -lcppunit -lpthread
OBJS=safequeuetest.o test.o loggertest.o
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

all: test

loggertest.o: loggertest.cpp loggertest.hpp
	$(CC) $(CFLAGS) -c loggertest.cpp

safequeuetest.o: safequeuetest.cpp safequeuetest.hpp
	$(CC) $(CFLAGS) -c safequeuetest.cpp

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

test: $(OBJS)
	$(CC) $(CFLAGS) -o unittest $(OBJS) $(LIBS)

run: test
	./unittest

clean:
	rm -f *.a *.o unittest
