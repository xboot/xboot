#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A17__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a17 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKROCK		:= arch/$(ARCH)/$(MACH)/tools/linux/mkrock
endif
ifeq ($(strip $(HOSTOS)), windows)
MKROCK		:= arch/$(ARCH)/$(MACH)/tools/windows/mkrock
endif
ifeq ($(strip $(HOSTOS)), macos)
MKROCK		:= arch/$(ARCH)/$(MACH)/tools/macos/mkrock
endif
INIFILE		:= arch/$(ARCH)/$(MACH)/tools/images/rk3288.ini

xend:
	@echo Packing rockchip binrary for irom booting
	@$(MKROCK) $(INIFILE)
