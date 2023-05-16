#
# Machine makefile
#

DEFINES		+=

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv64gcv0p7_zfh_xtheadc -mabi=lp64d -mtune=c906 -mcmodel=medlow -fno-stack-protector -mstrict-align

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
