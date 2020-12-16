#
# Machine makefile
#

DEFINES		+= -D__ARM64_ARCH__=8 -D__CORTEX_A53__ -D__ARM64_NEON__

ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv8-a -mcpu=cortex-a53 -mtune=cortex-a53 -mstrict-align

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
INIFILE		:= arch/$(ARCH)/$(MACH)/tools/images/rk3399.ini

xend:
	@echo Packing rockchip binrary for irom booting
	@$(MKROCK) $(INIFILE)
