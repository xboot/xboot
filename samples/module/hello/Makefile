#
# Makefile for module.
#

XBOOT_SRC_DIR					:= $(CURDIR)/../../../

export TARGET					?= arm32-realview
export CROSS					?= arm-none-eabi-

export MODULE_NAME				:= libhello
export MODULE_INCLUDE_DIRS		:= .
export MODULE_SOURCE_DIRS		:= .

export MODULE_ASFLAGS			:=
export MODULE_CFLAGS			:=
export MODULE_CXXFLAGS			:=
export MODULE_LDFLAGS			:=

export MODULE_LIB_SEARCH_DIRS	:= $(CURDIR)
export MODULE_LIB_LINKS			:= -lgcc

module:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) module

clean:
	@$(MAKE) -C $(XBOOT_SRC_DIR) M=$(CURDIR) mclean
