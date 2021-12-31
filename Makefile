CC=g++
CFLAGS=-Wall -DLINUX --std=c++11 -I.
OBJS=mlogger.o
LIBS=
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

all: libmikecpp.a

libmikecpp.a: $(OBJS)
	ar rc libmikecpp.a $(OBJS)

mlogger.o: mlogger.cpp mlogger.hpp
	$(CC) $(CFLAGS) -c mlogger.cpp

tags:
	ctags *.cpp *.hpp

test: all
	(cd t && make clean && make run)

clean:
	rm -f *.a *.o
	(cd t && make clean)
