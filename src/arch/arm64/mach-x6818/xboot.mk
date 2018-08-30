#
# Machine makefile
#

DEFINES		+= -D__ARM64_ARCH__=8 -D__CORTEX_A53__ -D__ARM64_NEON__

ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv8-a -mcpu=cortex-a53 -mtune=cortex-a53

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
NSIH		:= arch/$(ARCH)/$(MACH)/tools/images/nsih.txt
SECBOOT		:= arch/$(ARCH)/$(MACH)/tools/images/2ndboot

xend:
	@echo Make header information for irom booting
	@$(MK6818) $(X_NAME)pak.bin $(NSIH) $(SECBOOT) $(X_NAME).bin
