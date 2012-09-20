#
# machine makefile.
#

ifeq ($(TARGET), arm-realview)

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:= -mcpu=arm926ej-s -mtune=arm926ej-s -march=armv5te

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

endif