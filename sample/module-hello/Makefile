#
# Makefile for module.
#

TARGET						?= arm-realview
CROSS						?= /home/jjj/Software/eclipse-juno-for-arm-linux/compiler/arm-none-eabi/bin/arm-none-eabi-

XBOOT_DIR					:= $(CURDIR)/../../
MODULE_DIR					:= $(CURDIR)

export MODULE_NAME			:= module-hello
export MODULE_INCLUDE_DIRS	:= ./
export MODULE_SOURCE_DIRS	:= ./

module:
	@$(MAKE) -C $(XBOOT_DIR) M=$(MODULE_DIR) TARGET=$(TARGET) CROSS=$(CROSS) module

clean:
	@$(MAKE) -C $(XBOOT_DIR) M=$(MODULE_DIR) TARGET=$(TARGET) CROSS=$(CROSS) mclean
