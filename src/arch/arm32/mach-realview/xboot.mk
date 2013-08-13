#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A8__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
CXXFLAGS	:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:= -d
MCFLAGS		:= -mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a -mfpu=neon -ftree-vectorize -ffast-math -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=
