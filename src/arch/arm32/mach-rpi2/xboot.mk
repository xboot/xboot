#
# Machine makefile
#

DEFINES		+= -D__ARM_ARCH__=7 -D__CORTEX_A7__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
CXXFLAGS	:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=softfp

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

xend:
	@echo Make kernel7.img for raspberry pi
	@$(CP) $(X_NAME).bin $(X_OUT)/kernel7.img