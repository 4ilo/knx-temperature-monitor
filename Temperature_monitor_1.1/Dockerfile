FROM ubuntu:20.04

RUN apt-get update && apt-get install -y build-essential gcc-arm-none-eabi openocd cppcheck

#OpenOCD talks to the chip through USB, so we need to grant our account access to the FTDI.
#RUN cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/60-openocd.rules
