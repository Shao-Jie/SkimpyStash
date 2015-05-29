UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	DEBUG = -g -rdynamic -ggdb -DDEBUG
else
	DEBUG -g -ggdb -DDEBUG
endif
CC = g++
CFLAGS = -C -WALL $(DEBUG)

LIB_OBJS =   \
	./database.o \
	./hashtable.o \
	./log.o      \
	./hashfuns.o \
	./bloomfilter.o

TEST_OBJS =  \
	./test.o  \
	./testbloom.o

TESTS = \
	./test \
	./testbloom
LIBRARY = libskipmpystash.a

all: $(LIBRARY)

clean:
	-rm -f $(LIBRARY)
	-rm -f $(LIB_OBJS)
	-rm -f $(TEST_OBJS)
	-rm -f $(TESTS)
	-rm -f *.db
$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)

test: ./test.o $(TEST_OBJS) $(LIB_OBJS)
	$(CC) ./test.o $(SVR_OBJS) $(LIB_OBJS) -o $@
testbloom: ./testbloom.o $(TEST_OBJS) $(LIB_OBJS)
	$(CC) ./testbloom.o $(SVR_OBJS) $(LIB_OBJS) -o $@
