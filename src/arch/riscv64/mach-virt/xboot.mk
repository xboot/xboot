#
# Machine makefile
#

DEFINES		+= -D__RISCV64__ -D__RISCV64_I__ -D__RISCV64_M__ -D__RISCV64_A__ -D__RISCV64_F__ -D__RISCV64_D__ -D__RISCV64_C__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv64imafdc -mabi=lp64d -mcmodel=medany

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
