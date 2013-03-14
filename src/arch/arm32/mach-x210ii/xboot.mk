#
# machine makefile.
#

ASFLAGS	:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS	:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS	:= -rcs
OCFLAGS	:= -v -O binary
ODFLAGS	:=
MCFLAGS	:= -mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a

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
