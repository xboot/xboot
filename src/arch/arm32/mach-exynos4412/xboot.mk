#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A9__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=hard

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MK4412		:= arch/$(ARCH)/$(MACH)/tools/linux/mk4412
endif
ifeq ($(strip $(HOSTOS)), windows)
MK4412		:= arch/$(ARCH)/$(MACH)/tools/windows/mk4412
endif

xend:
	@echo Make header information for irom booting
	@$(MK4412) $(X_NAME).bin
