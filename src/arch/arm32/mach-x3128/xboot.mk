#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A7__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MK3128		:= arch/$(ARCH)/$(MACH)/tools/linux/mk3128
endif
ifeq ($(strip $(HOSTOS)), windows)
MK3128		:= arch/$(ARCH)/$(MACH)/tools/windows/mk3128
endif
LOADER		:= arch/$(ARCH)/$(MACH)/tools/images/loader

xend:
	@echo Make header information for irom booting
	@$(MK3128) $(X_NAME)pak.bin $(LOADER) $(X_NAME).bin
