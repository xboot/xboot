#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A8__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a8 -mfpu=vfpv3 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKV210		:= arch/$(ARCH)/$(MACH)/tools/linux/mkv210
endif
ifeq ($(strip $(HOSTOS)), windows)
MKV210		:= arch/$(ARCH)/$(MACH)/tools/windows/mkv210
endif

xend:
	@echo Make header information for irom booting
	$(MKV210) $(X_NAME).bin
