#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=5 -D__ARM926EJS__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv5te -mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/linux/mksunxi
MKZ			:= arch/$(ARCH)/$(MACH)/tools/linux/mkz
endif
ifeq ($(strip $(HOSTOS)), windows)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/windows/mksunxi
MKZ			:= arch/$(ARCH)/$(MACH)/tools/windows/mkz
endif
ifeq ($(strip $(HOSTOS)), macos)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/macos/mksunxi
MKZ			:= arch/$(ARCH)/$(MACH)/tools/macos/mkz
endif

xend:
	@echo Make header information for brom booting
	@$(MKSUNXI) $(X_NAME).bin
	@$(MKZ) -m 3 -n 0 -p 0 -z -s 16384 -i "" -k "" $(X_NAME).bin $(X_NAME).bin.z
