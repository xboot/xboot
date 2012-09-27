#
# Makefile for module.
#

XBOOT_SRC_DIR				:= $(CURDIR)/../../../

export MODULE_NAME			:= gears
export MODULE_INCLUDE_DIRS		:= . \
					libtinygl \
					libtinygl/include
export MODULE_SOURCE_DIRS		:= . \
					libtinygl		

module:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) module

clean:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) mclean
