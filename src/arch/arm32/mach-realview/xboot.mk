#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A8__ -D__ARM_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
