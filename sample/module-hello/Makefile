#
# Makefile for module.
#

XBOOT_SRC_DIR				:= $(CURDIR)/../../

export MODULE_NAME			:= module-hello
export MODULE_INCLUDE_DIRS	:= .
export MODULE_SOURCE_DIRS	:= .

module:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) module

clean:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) mclean
