#
# machine makefile.
#

ifeq ($(TARGET), arm-smdkv310)

CROSS		?= arm-none-eabi-

ASFLAGS		:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

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
#	@$(MKHEADER) $(OUTDIR)/$(XBOOT).bin

endif
