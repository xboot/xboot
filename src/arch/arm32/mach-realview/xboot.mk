#
# Machine makefile
#

NAMESPACES	+= -D__ARM_ARCH__=5 -D__ARM926EJS__

ASFLAGS	:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS	:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS	:= -rcs
OCFLAGS	:= -v -O binary
ODFLAGS	:=
MCFLAGS	:= -mcpu=arm926ej-s -mtune=arm926ej-s -march=armv5te

LIBDIRS	:=
LIBS 		:=

INCDIRS	:=
SRCDIRS	:=
