CC=g++
CFLAGS=-Wall --std=c++17 -I. -I../mikelibc
OBJS=mlogger.o mnetwork.o mnet.o mlogger.o mlog.o mcrypto.o
LIBS=
OS := $(shell uname -s)
MDEBUG=0
ASAN=0
MTHREADS=1

ifeq ($(MDEBUG),1)
	CFLAGS += -ggdb
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
endif

ifeq ($(MTHREADS),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: libmikecpp.a

libmikecpp.a: $(OBJS) mqueue.hpp
	ar rc libmikecpp.a *.o

mlogger.o: mlogger.cpp mlogger.hpp type_traits.hpp to_string.hpp
	$(CC) $(CFLAGS) -c mlogger.cpp

mcrypto.o: mcrypto.cpp mcrypto.hpp
	$(CC) $(CFLAGS) -c mcrypto.cpp

mlog.o: ../mikelibc/mlog.c ../mikelibc/mlog.h
	$(CC) $(CFLAGS) -c ../mikelibc/mlog.c

mnet.o: ../mikelibc/mnet.h ../mikelibc/mnet.c ../mikelibc/mlog.h
	$(CC) $(CFLAGS) -c ../mikelibc/mnet.c

mnetwork.o:: mnetwork.hpp mlogger.hpp ../mikelibc/mnet.h
	$(CC) $(CFLAGS) -c mnetwork.cpp

tags:
	ctags *.cpp *.hpp

test: all
	cd t && make clean && make run MDEBUG=$(MDEBUG)

clean:
	rm -f *.a *.o
	cd t && make clean
