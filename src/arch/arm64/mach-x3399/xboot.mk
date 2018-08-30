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
MK3399		:= arch/$(ARCH)/$(MACH)/tools/linux/mk3399
PAK3399		:= arch/$(ARCH)/$(MACH)/tools/linux/pak3399
endif
ifeq ($(strip $(HOSTOS)), windows)
MK3399		:= arch/$(ARCH)/$(MACH)/tools/windows/mk3399
PAK3399		:= arch/$(ARCH)/$(MACH)/tools/windows/pak3399
endif
TEMPLATE	:= arch/$(ARCH)/$(MACH)/tools/images/template

xend:
	@$(MK3399) $(X_NAME).bin $(X_NAME).img
	@$(PAK3399) $(X_NAME)pak.bin $(X_NAME).img $(TEMPLATE)
