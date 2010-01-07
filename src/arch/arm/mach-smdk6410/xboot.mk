#
# machine makefile.
#

ifeq ($(MACH), mach-smdk6410)

CROSS		?= arm-none-eabi-

ASFLAGS		:=
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