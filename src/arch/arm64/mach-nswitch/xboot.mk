#
# Machine makefile
#

DEFINES		+= -D__ARM64_ARCH__=8 -D__CORTEX_A57__ -D__ARM64_NEON__

ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv8-a -mcpu=cortex-a57 -mtune=cortex-a57

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
