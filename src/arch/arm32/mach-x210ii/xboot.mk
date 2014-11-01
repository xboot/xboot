#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A8__ -D__ARM_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:= -d
MCFLAGS		:= -mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a -mfpu=neon -ftree-vectorize -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

#
# Add end rule
#
ifeq ($(strip $(HOSTOS)), linux)
MKV210		:= arch/$(ARCH)/$(MACH)/tools/linux/mkv210
endif
ifeq ($(strip $(HOSTOS)), windows)
MKV210		:= arch/$(ARCH)/$(MACH)/tools/windows/mkv210
endif

end:
	@echo make header information for irom booting
	$(MKV210) $(X_NAME).bin
