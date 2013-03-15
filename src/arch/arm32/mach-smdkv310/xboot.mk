#
# machine makefile.
#

NAMESPACES	+= -D__ARM_ARCH__=7 -D__CORTEX_A9__

ASFLAGS	:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
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
MKV310		:= arch/$(ARCH)/$(MACH)/tools/linux/mkv310
endif
ifeq ($(strip $(HOSTOS)), windows)
MKV310		:= arch/$(ARCH)/$(MACH)/tools/windows/mkv310
endif

fixup:
	@echo make checksum information for irom booting
	@$(MKV310) $(X_NAME).bin
