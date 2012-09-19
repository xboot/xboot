#
# Makefile for external module.
#

TARGET	?= arm-realview
CROSS	?= /home/jjj/Software/eclipse-juno-for-arm-linux/compiler/arm-none-eabi/bin/arm-none-eabi-

CDIR	:= $(CURDIR)
KDIR	:= $(CDIR)/../../

IDIRS	:= ./
SDIRS	:= ./

MODULE	:= module-hello

.PHONY:	all module clean

all:	module

module:
	@$(MAKE) -C $(KDIR) M=$(CDIR) IDIRS="$(IDIRS)" SDIRS="$(SDIRS)" MODULE=$(MODULE) TARGET=$(TARGET) CROSS=$(CROSS) module

clean:
	@$(RM) *~ *.d *.o *.map *.so
	@echo Clean module complete.
