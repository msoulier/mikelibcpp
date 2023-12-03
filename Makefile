CC=g++
CFLAGS=-Wall --std=c++17 -I. -I../mikelibc -D_GNU_SOURCE
OBJS=mlogger.o mnetwork.o mlogger.o mcrypto.o mstring.o
LIBS=
OS := $(shell uname -s)
MDEBUG=0
ASAN=0
MTHREADS=1

ifeq ($(MDEBUG),1)
	CFLAGS += -ggdb -DMDEBUG=1
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
endif

ifeq ($(MTHREADS),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: libmikecpp.a

libmike.a:
	cd ../mikelibc && make MDEBUG=$(MDEBUG)
	cp ../mikelibc/libmike.a .
	ar x libmike.a

libmikecpp.a: libmike.a $(OBJS) mqueue.hpp
	ar rc libmikecpp.a *.o

mlogger.o: mlogger.cpp mlogger.hpp type_traits.hpp to_string.hpp
	$(CC) $(CFLAGS) -c mlogger.cpp

mcrypto.o: mcrypto.cpp mcrypto.hpp
	$(CC) $(CFLAGS) -c mcrypto.cpp

mstring.o: mstring.cpp mstring.hpp
	$(CC) $(CFLAGS) -c mstring.cpp

mnetwork.o:: mnetwork.hpp mlogger.hpp
	$(CC) $(CFLAGS) -c mnetwork.cpp

tags:
	ctags *.cpp *.hpp

test: all
	cd t && make run MDEBUG=$(MDEBUG)

clean:
	rm -f *.a *.o
	rm -rf doc
	cd t && make clean
	cd ../mikelibc && make clean

doc:
	doxygen Doxyfile
