#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=8 -D__CORTEX_A53__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
CXXFLAGS	:= -g -ggdb -Wall -O0
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
NSIH		:= arch/$(ARCH)/$(MACH)/tools/image/nsih.txt
SECBOOT		:= arch/$(ARCH)/$(MACH)/tools/image/2ndboot

xend:
	@echo Make header information for irom booting
	@$(MK6818) $(X_NAME)pak.bin $(NSIH) $(SECBOOT) $(X_NAME).bin
