#
# Makefile for external module.
#

_TARGET		?= arm-realview
_CROSS		?= /home/jjj/software/eclipse-juno-for-arm-linux/compiler/arm-none-eabi/bin/arm-none-eabi-

CDIR	:= $(shell pwd)
KDIR	:= /home/jjj/xboot/xboot/

MODULE	:= hello.mod
MOBJS	:= module-hello.o

.PHONY: all clean

all:
	@$(MAKE) -C $(KDIR) M=$(CDIR) TARGET=$(_TARGET) CROSS=$(_CROSS) module

clean:
	echo $(ARCH)
