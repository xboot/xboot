#
# Machine makefile
#

DEFINES		+=

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv64imafd -mabi=lp64d -mcmodel=medany

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
