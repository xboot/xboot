#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A17__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a17 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm -mno-thumb-interwork -mno-unaligned-access

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKIDB		:= arch/$(ARCH)/$(MACH)/tools/linux/mkidb
endif
ifeq ($(strip $(HOSTOS)), windows)
MKIDB		:= arch/$(ARCH)/$(MACH)/tools/windows/mkidb
endif

xend:
	@echo Packing rockchip binrary for irom booting
	@$(MKIDB) arch/$(ARCH)/$(MACH)/tools/images/rk3288_ddr_400MHz_v1.09.bin $(X_NAME).bin $(X_NAME)pak.bin
