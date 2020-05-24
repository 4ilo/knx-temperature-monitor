DOCKER_IMAGE := ubuntu_arm_none_gcc
SUBDIR := module

$(MAKECMDGOALS): $(DOCKER_IMAGE)
	docker run --rm -it -v $(PWD):/project --workdir /project --privileged --user 1000:1000 $(DOCKER_IMAGE) $(MAKE) -C $(SUBDIR) -f module.mk $@

$(DOCKER_IMAGE):
	docker build -t $@ .
	@touch $@
