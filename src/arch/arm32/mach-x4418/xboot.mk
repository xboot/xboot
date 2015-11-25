#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A9__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -mcpu=cortex-a9 -mtune=cortex-a9 -march=armv7-a -mfpu=neon -ftree-vectorize -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MK4418		:= arch/$(ARCH)/$(MACH)/tools/linux/mk4418
endif
ifeq ($(strip $(HOSTOS)), windows)
MK4418		:= arch/$(ARCH)/$(MACH)/tools/windows/mk4418
endif
NSIH		:= arch/$(ARCH)/$(MACH)/tools/image/nsih.txt
SECBOOT		:= arch/$(ARCH)/$(MACH)/tools/image/2ndboot

xend:
	@echo Make header information for irom booting
	@$(MK4418) $(X_NAME)pak.bin $(NSIH) $(SECBOOT) $(X_NAME).bin
