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

xend:
	@echo Make kernel8.img for raspberry-pi v3
	@$(CP) $(X_NAME).bin $(X_OUT)/kernel8.img