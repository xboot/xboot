#
# machine makefile.
#

NAMESPACES	+= -D__ARM_ARCH__=7 -D__CORTEX_A8__

ASFLAGS	:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS	:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS	:= -rcs
OCFLAGS	:= -v -O binary
ODFLAGS	:=
MCFLAGS	:=

LIBDIRS	:=
LIBS 		:=

INCDIRS	:=
SRCDIRS	:=

#
# add fixup rule
#
ifeq ($(strip $(HOSTOS)), linux)
MKHEADER	:= arch/$(ARCH)/$(MACH)/tools/linux/mkheader
endif
ifeq ($(strip $(HOSTOS)), windows)
MKHEADER	:= arch/$(ARCH)/$(MACH)/tools/windows/mkheader
endif

fixup:
	@echo make header information for irom booting
	@$(MKHEADER) $(X_NAME).bin
