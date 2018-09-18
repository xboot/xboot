#
# Machine makefile
#

DEFINES		+= -D__RISCV32__ -D__RISCV32_I__ -D__RISCV32_M__ -D__RISCV32_A__ -D__RISCV32_F__ -D__RISCV32_D__ -D__RISCV32_C__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=rv32imafdc -mabi=ilp32d -mcmodel=medany

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
