CC=g++
CFLAGS=-Wall --std=c++17 -I.
OBJS=mlogger.o mnetwork.o
LIBS=
OS := $(shell uname -s)
MDEBUG=0
ASAN=0

ifeq ($(MDEBUG),1)
	CFLAGS += -ggdb
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
endif

all: libmikecpp.a

libmikecpp.a: $(OBJS) mqueue.hpp
	ar rc libmikecpp.a $(OBJS)

mlogger.o: mlogger.cpp mlogger.hpp type_traits.hpp to_string.hpp
	$(CC) $(CFLAGS) -c mlogger.cpp

mnetwork.o:: mnetwork.hpp mlogger.hpp
	$(CC) $(CFLAGS) -c mnetwork.cpp

tags:
	ctags *.cpp *.hpp

test: all
	(cd t && make clean && make run MDEBUG=$(MDEBUG))

clean:
	rm -f *.a *.o
	(cd t && make clean)
