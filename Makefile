CC=g++
CFLAGS=-Wall -DLINUX --std=c++11 -I.
OBJS=mlogger.o
LIBS=
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

all: libmikecpp.a

libmikecpp.a: $(OBJS)
	ar rc libmikecpp.a $(OBJS)

mlogger.o: mlogger.cpp mlogger.hpp
	$(CC) $(CFLAGS) -c mlogger.cpp

tags:
	ctags *.cpp *.hpp

test: all
	(cd t && make clean && make test)

clean:
	rm -f *.a *.o
	(cd t && make clean)
