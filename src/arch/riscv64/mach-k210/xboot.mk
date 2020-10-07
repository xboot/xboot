#
# Machine makefile
#

DEFINES		+=

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv64imafc -mabi=lp64f -mcmodel=medany

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
