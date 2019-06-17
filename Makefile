.PHONY: install
install:
	docker build -t 9cc .

.PHONY: build
build:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk 9cc"

.PHONY: test
test:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk test"

.PHONY: test-verbose
test-verbose:
	docker run -v $(shell pwd):/tmp/ 9cc bash -c "cd /tmp/; make -f docker.mk test-verbose"

.PHONY: clean
clean:
	rm -f 9cc *.o *~ tmp*
