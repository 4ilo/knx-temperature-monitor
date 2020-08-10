include Makefile

flash: all
	openocd -f stm32f072.cfg -c "program build/Temperature_monitor_1.1.elf verify reset exit"

test:
	$(MAKE) -C tests test

cppcheck:
	cppcheck Src Inc
