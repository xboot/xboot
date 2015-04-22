#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A9__ -D__ARM_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -mcpu=cortex-a9 -mtune=cortex-a9 -march=armv7-a -mfpu=neon -ftree-vectorize -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
