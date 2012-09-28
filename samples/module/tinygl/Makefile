#
# Makefile for module.
#

XBOOT_SRC_DIR					:= $(CURDIR)/../../../

export MODULE_NAME				:= gears
export MODULE_INCLUDE_DIRS		:= . libtinygl libtinygl/include
export MODULE_SOURCE_DIRS		:= . libtinygl

export MODULE_ASFLAGS			:= -g -ggdb -Wall -O3
export MODULE_CFLAGS			:= -g -ggdb -Wall -O3
export MODULE_CXXFLAGS			:= -g -ggdb -Wall -O3
export MODULE_LDFLAGS			:=

export MODULE_LIB_SEARCH_DIRS	:= -L $(CURDIR)
export MODULE_LIB_LINKS			:= -lgcc

module:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) module

clean:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) mclean
