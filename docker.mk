CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)

$(OBJS): 9cc.h

test: 9cc
	./9cc -test
	./test.sh

test-verbose: 9cc
	./9cc -test
	./test.sh -v

clean:
	rm -f 9cc *.o *~ tmp*
