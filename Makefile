WORKDIR=/tmp

test:
	docker-compose run 9cc bash -c "cd $(WORKDIR); make -f docker.mk test"

clean:
	rm -f 9cc *.o *~ tmp*
