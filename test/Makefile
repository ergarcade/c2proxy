CC=clang

INCDIR=-I../include -I../csafe
CFLAGS=-Wall -pedantic -g -DCSAFE_DEBUG -D_THREAD_SAFE -pthread -DDEBUG
LDFLAGS=-lhidapi

SOURCES=../csafe/csafe.cpp

OBJS=$(SOURCES:.cpp=.o)

.cpp.o:
	$(CC) $(INCDIR) $(CFLAGS) -c $< -o $@

csafe-test: csafe-test.o ../csafe/csafe.o utils.o
	$(CC) -o $@ $^ $(LDFLAGS)

all: csafe-test cksum queries

test: csafe-test
	./csafe-test

cksum: cksum.o
	$(CC) -o $@ $^ $(LDFLAGS)

queries: queries.o ../csafe/csafe.o utils.o
	$(CC) -o $@ $^ $(LDFLAGS)

dep:
	$(CC) -M $(INCDIR) $(SOURCES) > .depend
	ctags $(SOURCES)

clean:
	/bin/rm -rf *.dSYM
	/bin/rm -f csafe-test cksum queries
	/bin/rm -f ut_*
	/bin/rm -f $(OBJS)
	/bin/rm -f *.o
	/bin/rm -f Makefile.bak
	/bin/rm -f .depend tags

-include .depend
