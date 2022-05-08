CC=g++
CFLAGS=-Wall --std=c++17 -I. -I.. -ggdb -fsanitize=address
LIBS=-L.. -lmikecpp -lcppunit -lpthread
OBJS=safequeuetest.o test.o loggertest.o
OS := $(shell uname -s)

all: test

loggertest.o: loggertest.cpp loggertest.hpp
	$(CC) $(CFLAGS) -c loggertest.cpp

safequeuetest.o: safequeuetest.cpp safequeuetest.hpp
	$(CC) $(CFLAGS) -c safequeuetest.cpp

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

test: $(OBJS)
	$(CC) $(CFLAGS) -o unittest $(OBJS) $(LIBS)

run: clean test
	rm -f /tmp/logfile*
	./unittest

clean:
	rm -f *.a *.o unittest
