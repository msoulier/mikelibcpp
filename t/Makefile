CC=g++
CFLAGS=-Wall --std=c++17 -I. -I.. -I../../mikelibc
LIBS=-L.. -lmikecpp -lcppunit -lpthread -lssl -lcrypto
OBJS=safequeuetest.o test.o loggertest.o cryptotest.o mstringtest.o
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

mstringtest.o: mstringtest.cpp mstringtest.hpp
	$(CC) $(CFLAGS) -c mstringtest.cpp

loggertest.o: loggertest.cpp loggertest.hpp ../mlogger.hpp
	$(CC) $(CFLAGS) -c loggertest.cpp

cryptotest.o: cryptotest.cpp cryptotest.hpp ../mcrypto.hpp
	$(CC) $(CFLAGS) -c cryptotest.cpp

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
