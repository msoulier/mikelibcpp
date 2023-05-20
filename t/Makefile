CC=g++
CFLAGS=-Wall --std=c++17 -I. -I..
LIBS=-L.. -lmikecpp -lcppunit -lpthread
OBJS=safequeuetest.o test.o loggertest.o
OS := $(shell uname -s)
MDEBUG=0
ASAN=0

ifeq ($(MDEBUG),1)
	CFLAGS += -ggdb
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
endif

all: test

loggertest.o: loggertest.cpp loggertest.hpp ../mlogger.hpp
	$(CC) $(CFLAGS) -c loggertest.cpp

safequeuetest.o: safequeuetest.cpp safequeuetest.hpp ../mqueue.hpp
	$(CC) $(CFLAGS) -c safequeuetest.cpp

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

test: $(OBJS) ../libmikecpp.a
	$(CC) $(CFLAGS) -o unittest $(OBJS) $(LIBS)

run: clean test
	rm -f /tmp/logfile*
	./unittest

clean:
	rm -f *.a *.o unittest
