.PHONY: c docker-image
c:
	make -C c clean run
clean:
	make -C c clean
	@# don't need to clean $(DOCKER_IMAGE_TOUCH) here

DOCKER_TAG ?= lox
DOCKER_IMAGE_TOUCH = tmp/docker-image-$(DOCKER_TAG)
docker-image: $(DOCKER_IMAGE_TOUCH)

$(DOCKER_IMAGE_TOUCH): Dockerfile
	docker build -t $(DOCKER_TAG) -f Dockerfile .
	mkdir -p $$(dirname $(DOCKER_IMAGE_TOUCH)) && touch $(DOCKER_IMAGE_TOUCH)

docker: docker-image
	docker run --rm -it -v $(PWD):/src -w /src -e RUN_FILE $(DOCKER_TAG) make c
