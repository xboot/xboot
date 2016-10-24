#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=8 -D__CORTEX_A53__

ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv8-a -mcpu=cortex-a53 -mtune=cortex-a53

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MK3399		:= arch/$(ARCH)/$(MACH)/tools/linux/mk3399
endif
ifeq ($(strip $(HOSTOS)), windows)
MK3399		:= arch/$(ARCH)/$(MACH)/tools/windows/mk3399
endif
LOADER		:= arch/$(ARCH)/$(MACH)/tools/images/loader

xend:
	@echo Make header information for irom booting
	@$(MK3399) $(X_NAME)pak.bin $(LOADER) $(X_NAME).bin
