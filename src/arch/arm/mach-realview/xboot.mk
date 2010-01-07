#
# machine makefile.
#

ifeq ($(MACH), mach-realview)

CROSS		?= arm-none-eabi-

ASFLAGS		:= --gstabs --warn
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles -nostdlib -nodefaultlibs -nostdinc
ARFLAGS		:=
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

endif