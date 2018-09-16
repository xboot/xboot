#
# Machine makefile
#

DEFINES		+= -D__RISCV64__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv64imafdc -mabi=lp64d -mcmodel=medany

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
