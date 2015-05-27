#UNAME := $(shell uname)
#ifeq ($(UNAME),Linux)
#	DEBUG = -g -rdynamic -ggdb -DDEBUG
#else
#	DEBUG -g -ggdb -DDEBUG
#endif
CC = g++
CFLAGS = -C -wALL $(DEBUG)

LIB_OBJS =   \
	./database.o \
	./hashtable.o \
	./log.o

TEST_OBJS =  \
	./test.o

TESTS = \
	./test
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
