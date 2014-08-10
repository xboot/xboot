#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A9__ -D__ARM_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:= -d
MCFLAGS		:= -mcpu=cortex-a9 -mtune=cortex-a9 -march=armv7-a -mfpu=neon -ftree-vectorize -ffast-math -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

#
# Add fixup rule
#
ifeq ($(strip $(HOSTOS)), linux)
MK4412		:= arch/$(ARCH)/$(MACH)/tools/linux/mk4412
endif
ifeq ($(strip $(HOSTOS)), windows)
MK4412		:= arch/$(ARCH)/$(MACH)/tools/windows/mk4412
endif

fixup:
	@echo make header information for irom booting
	@$(MK4412) $(X_NAME).bin
