#
# machine makefile.
#

ifeq ($(TARGET), arm-realview)

CROSS		?= arm-none-eabi-

ASFLAGS		:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
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