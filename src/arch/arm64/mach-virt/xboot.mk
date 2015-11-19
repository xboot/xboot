#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=8 -D__CORTEX_A57__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
CXXFLAGS	:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv8-a -mcpu=cortex-a57 -mtune=cortex-a57

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
