#
# Machine makefile
#

DEFINES		+=

ASFLAGS		:= -g -ggdb -Wall -D __ASSEMBLY__
CFLAGS		:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -mlittle-endian -mcpu=ck610 -Wa,-mcpu=ck610 -fno-tree-vectorize -msoft-float -mdiv

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:= arch/$(ARCH)/$(MACH)/driver/video

export sys-ccu-flags 		:= -fPIC
export sys-copyself-flags	:= -fPIC
export sys-dram-flags		:= -fPIC
export sys-spinor-flags		:= -fPIC
export sys-uart-flags		:= -fPIC
