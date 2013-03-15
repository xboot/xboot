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
MK4412		:= arch/$(ARCH)/$(MACH)/tools/linux/mk4412
endif
ifeq ($(strip $(HOSTOS)), windows)
MK4412		:= arch/$(ARCH)/$(MACH)/tools/windows/mk4412
endif

fixup:
	@echo make checksum information for irom booting
	@$(MK4412) $(X_NAME).bin
