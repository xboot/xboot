#
# machine makefile.
#

ifeq ($(MACH), mach-smdkv210)

CROSS		?= arm-none-eabi-

ASFLAGS		:=
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles -nostdlib -nodefaultlibs -nostdinc
ARFLAGS		:=
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

#
# add final rules
#
ifeq ($(strip $(HOSTOS)), linux)
MKHEADER	:= arch/$(ARCH)/$(MACH)/tools/linux/mkheader
endif
ifeq ($(strip $(HOSTOS)), windows)
MKHEADER	:= arch/$(ARCH)/$(MACH)/tools/windows/mkheader
endif

final:
	@echo make header information for irom booting
	@$(MKHEADER) $(OUTDIR)/$(XBOOT_NAME).bin

endif
