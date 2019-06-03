install:
	docker build -t 9cc .

build:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk 9cc"

test:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk test"

test-verbose:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk test-verbose"

clean:
	rm -f 9cc *.o *~ tmp*
