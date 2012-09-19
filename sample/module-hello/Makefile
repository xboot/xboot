#
# Makefile for external module.
#

TARGET				?= arm-realview
CROSS				?= /home/jjj/Software/eclipse-juno-for-arm-linux/compiler/arm-none-eabi/bin/arm-none-eabi-

XBOOT_DIR			:= $(CURDIR)/../../
MODULE_TOP_DIR		:= $(CURDIR)

MODULE_INCLUDE_DIRS	:= ./
MODULE_SOURCE_DIRS	:= ./

MODULE_NAME			:= module-hello

ifeq ($(XBOOT),)
module:
	@$(MAKE) -C $(XBOOT_DIR) M=$(MODULE_TOP_DIR) TARGET=$(TARGET) CROSS=$(CROSS) module
	
clean:
	@$(MAKE) -C $(XBOOT_DIR) M=$(MODULE_TOP_DIR) TARGET=$(TARGET) CROSS=$(CROSS) module-clean
endif
