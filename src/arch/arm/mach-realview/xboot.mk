#
# machine makefile.
#

ifeq ($(TARGET), arm-realview)

CROSS		?= arm-none-eabi-

ASFLAGS		:= --gstabs --warn
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles -nostdlib -nodefaultlibs -nostdinc
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

endif