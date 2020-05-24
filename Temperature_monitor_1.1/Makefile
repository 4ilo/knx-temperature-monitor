DOCKER_IMAGE := ubuntu_arm_none_gcc
SUBDIR := module

DOCKER_CMD = docker run --rm -v $(PWD):/project --workdir /project --privileged
ifneq ($(CI),1)
    DOCKER_CMD += --user 1000:1000
endif
DOCKER_CMD += $(DOCKER_IMAGE)

$(MAKECMDGOALS): $(DOCKER_IMAGE)
	$(DOCKER_CMD) $(MAKE) -C $(SUBDIR) -f module.mk $@

$(DOCKER_IMAGE):
	docker build -t $@ .
	@touch $@
