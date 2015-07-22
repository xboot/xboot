#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A9__ -D__ARM_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -mcpu=cortex-a53 -mtune=cortex-a53 -march=armv8-a -ftree-vectorize

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MK6818		:= arch/$(ARCH)/$(MACH)/tools/linux/mk6818
endif
ifeq ($(strip $(HOSTOS)), windows)
MK6818		:= arch/$(ARCH)/$(MACH)/tools/windows/mk6818
endif
NSIH		:= arch/$(ARCH)/$(MACH)/tools/image/nsih.txt
SECBOOT		:= arch/$(ARCH)/$(MACH)/tools/image/2ndboot

xend:
	@echo Make header information for irom booting
	@$(MK6818) $(X_NAME)pak.bin $(NSIH) $(SECBOOT) $(X_NAME).bin
