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
MK4418		:= arch/$(ARCH)/$(MACH)/tools/linux/mk4418
endif
ifeq ($(strip $(HOSTOS)), windows)
MK4418		:= arch/$(ARCH)/$(MACH)/tools/windows/mk4418
endif
NSIH		:= arch/$(ARCH)/$(MACH)/tools/images/nsih.txt
SECBOOT		:= arch/$(ARCH)/$(MACH)/tools/images/2ndboot

xend:
	@echo Make header information for irom booting
	@$(MK4418) $(X_NAME)pak.bin $(NSIH) $(SECBOOT) $(X_NAME).bin
