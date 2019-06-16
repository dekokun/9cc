CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)

$(OBJS): 9cc.h

.PHONY: test
test: 9cc
	./9cc -test
	./test.sh

.PHONY: test-verbose
test-verbose: 9cc
	./9cc -test
	./test.sh -v

.PHONY: clean
clean:
	rm -f 9cc *.o *~ tmp*
