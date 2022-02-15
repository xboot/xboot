#
# Makefile for xboot
#

#
# You can pass CROSS_COMPILE and PLATFORM variable.
#
CROSS_COMPILE	?=
PLATFORM		?=

#
# Configurable advanced custom components
#
CFG_FRAMEWORK	?= y
CFG_CAIRO		?= y
CFG_WBOXTEST 	?= n

#
# Xboot version
#
XBOOT_MAJOY		:= 3
XBOOT_MINIOR	:= 0
XBOOT_PATCH		:= 0
DEFINES			:= -DXBOOT_MAJOY=$(XBOOT_MAJOY) -DXBOOT_MINIOR=$(XBOOT_MINIOR) -DXBOOT_PATCH=$(XBOOT_PATCH)

#
# Get platform information about ARCH and MACH from PLATFORM variable.
#
ifeq ($(words $(subst -, , $(PLATFORM))), 2)
ARCH			:= $(word 1, $(subst -, , $(PLATFORM)))
MACH			:= mach-$(word 2, $(subst -, , $(PLATFORM)))
else
ARCH			:= x64
MACH			:= mach-sandbox
endif

#
# System environment variable.
#
ifeq ($(OS), Windows_NT)
	HOSTOS		:= windows
else
	ifneq (,$(findstring Linux, $(shell uname -a)))
		HOSTOS	:= linux
	endif
endif

#
# Load default variables.
#
ASFLAGS			:= -g -ggdb -Wall -O0
CFLAGS			:= -g -ggdb -Wall -O0
LDFLAGS			:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS			:=

LIBDIRS			:=
LIBS 			:=
INCDIRS			:=
SRCDIRS			:=

#
# Override default variables.
#
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# Define some variables.
#
ifeq ($(strip $(ARCH)), arm32)
DEFINES		+=	-D__ARM32__
endif
ifeq ($(strip $(ARCH)), arm64)
DEFINES		+=	-D__ARM64__
endif
ifeq ($(strip $(ARCH)), riscv64)
DEFINES		+=	-D__RISCV64__
endif
ifeq ($(strip $(ARCH)), x64)
DEFINES		+=	-D__X64__
endif

ASFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
CFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
LDFLAGS		+=	-Wl,-gc-sections

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
INCDIRS		+=	include \
				arch/$(ARCH)/include \
				arch/$(ARCH)/$(MACH)/include

SRCDIRS		+=	arch/$(ARCH)/lib/cpu \
				arch/$(ARCH)/lib \
				arch/$(ARCH)/$(MACH) \
				arch/$(ARCH)/$(MACH)/command \
				arch/$(ARCH)/$(MACH)/driver \
				lib/libc/charset \
				lib/libc/crypto \
				lib/libc/ctype \
				lib/libc/environ \
				lib/libc/errno \
				lib/libc/exit \
				lib/libc/filter \
				lib/libc/locale \
				lib/libc/malloc \
				lib/libc/path \
				lib/libc/stdio \
				lib/libc/stdlib \
				lib/libc/string \
				lib/libc/time \
				lib/libm/$(ARCH) \
				lib/libm \
				lib/libx \
				init \
				kernel/command \
				kernel/core \
				kernel/graphic \
				kernel/shell \
				kernel/sound \
				kernel/time \
				kernel/vfs \
				kernel/vfs/cpio \
				kernel/vfs/ext4 \
				kernel/vfs/fat \
				kernel/vfs/ram \
				kernel/vfs/sys \
				kernel/vfs/tar \
				kernel/vision \
				kernel/xfs \
				kernel/xui \
				driver/adc \
				driver/audio \
				driver/battery \
				driver/block \
				driver/block/partition \
				driver/buzzer \
				driver/camera \
				driver/clk \
				driver/clockevent \
				driver/clocksource \
				driver/compass \
				driver/console \
				driver/dac \
				driver/dma \
				driver/framebuffer \
				driver/gmeter \
				driver/gnss \
				driver/gpio \
				driver/gyroscope \
				driver/hygrometer \
				driver/i2c \
				driver/input \
				driver/input/rc \
				driver/interrupt \
				driver/led \
				driver/light \
				driver/motor \
				driver/net \
				driver/nvmem \
				driver/pressure \
				driver/proximity \
				driver/pwm \
				driver/regulator \
				driver/reset \
				driver/rng \
				driver/rtc \
				driver/sd \
				driver/servo \
				driver/spi \
				driver/stepper \
				driver/thermometer \
				driver/uart \
				driver/vibrator \
				driver/watchdog \
				driver/wifi

#
# Add necessary external library
#
INCDIRS		+= 	external/libfdt-1.4.2
SRCDIRS		+=	external/libfdt-1.4.2

INCDIRS		+= 	external/lz4-1.9.3
SRCDIRS		+=	external/lz4-1.9.3

INCDIRS		+= 	external/zlib-1.2.11
SRCDIRS		+=	external/zlib-1.2.11

INCDIRS		+= 	external/libpng-1.6.37
SRCDIRS		+=	external/libpng-1.6.37

INCDIRS		+= 	external/jpeg-9d
SRCDIRS		+=	external/jpeg-9d

INCDIRS		+=	external/stb_vorbis-1.21
SRCDIRS		+=	external/stb_vorbis-1.21

INCDIRS		+=	external/quirc-1.1.0
SRCDIRS		+=	external/quirc-1.1.0

INCDIRS		+=	external/libcg-1.0.3
SRCDIRS		+=	external/libcg-1.0.3

INCDIRS		+= 	external/freetype-2.10.4/include \
				external/freetype-2.10.4/src/autofit \
				external/freetype-2.10.4/src/base \
				external/freetype-2.10.4/src/cache \
				external/freetype-2.10.4/src/cff \
				external/freetype-2.10.4/src/cid \
				external/freetype-2.10.4/src/gxvalid \
				external/freetype-2.10.4/src/otvalid \
				external/freetype-2.10.4/src/pfr \
				external/freetype-2.10.4/src/psaux \
				external/freetype-2.10.4/src/pshinter \
				external/freetype-2.10.4/src/psnames \
				external/freetype-2.10.4/src/raster \
				external/freetype-2.10.4/src/sfnt \
				external/freetype-2.10.4/src/smooth \
				external/freetype-2.10.4/src/truetype \
				external/freetype-2.10.4/src/type1 \
				external/freetype-2.10.4/src/type42 \
				external/freetype-2.10.4/src/winfonts
SRCDIRS		+=	external/freetype-2.10.4/src/autofit \
				external/freetype-2.10.4/src/base \
				external/freetype-2.10.4/src/cache \
				external/freetype-2.10.4/src/cff \
				external/freetype-2.10.4/src/cid \
				external/freetype-2.10.4/src/gxvalid \
				external/freetype-2.10.4/src/otvalid \
				external/freetype-2.10.4/src/pfr \
				external/freetype-2.10.4/src/psaux \
				external/freetype-2.10.4/src/pshinter \
				external/freetype-2.10.4/src/psnames \
				external/freetype-2.10.4/src/raster \
				external/freetype-2.10.4/src/sfnt \
				external/freetype-2.10.4/src/smooth \
				external/freetype-2.10.4/src/truetype \
				external/freetype-2.10.4/src/type1 \
				external/freetype-2.10.4/src/type42 \
				external/freetype-2.10.4/src/winfonts

#
# Advanced custom components
#
ifeq ($(strip $(CFG_FRAMEWORK)), y)
INCDIRS		+= 	external/lua-5.4.4
SRCDIRS		+=	external/lua-5.4.4
INCDIRS		+= 	external/lua-cjson-2.1.0
SRCDIRS		+=	external/lua-cjson-2.1.0
INCDIRS		+= 	framework
SRCDIRS		+=	framework \
				framework/codec \
				framework/core \
				framework/hardware
endif

ifeq ($(strip $(CFG_CAIRO)), y)
INCDIRS		+= 	external/pixman-0.40.0 \
				external/pixman-0.40.0/$(ARCH)
SRCDIRS		+=	external/pixman-0.40.0 \
				external/pixman-0.40.0/$(ARCH)
INCDIRS		+= 	external/cairo-1.17.4
SRCDIRS		+=	external/cairo-1.17.4
endif

ifeq ($(strip $(CFG_WBOXTEST)), y)
INCDIRS		+=	wboxtest
SRCDIRS		+=	wboxtest \
				wboxtest/benchmark-graphic \
				wboxtest/benchmark-memory \
				wboxtest/block \
				wboxtest/camera \
				wboxtest/crypto \
				wboxtest/dma \
				wboxtest/graphic \
				wboxtest/path \
				wboxtest/stdio \
				wboxtest/task
endif

#
# You shouldn't need to change anything below this point.
#
AS			:=	$(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:=	$(CROSS_COMPILE)gcc
CXX			:=	$(CROSS_COMPILE)g++
LD			:=	$(CROSS_COMPILE)ld
AR			:=	$(CROSS_COMPILE)ar
SZ			:=	$(CROSS_COMPILE)size
OC			:=	$(CROSS_COMPILE)objcopy
OD			:=	$(CROSS_COMPILE)objdump
MKDIR		:=	mkdir -p
CP			:=	cp -af
RM			:=	rm -fr
CD			:=	cd
FIND		:=	find
CPIO		:=	cpio -o -H newc --quiet

#
# Xboot variables
#
X_ASFLAGS	:=	$(MCFLAGS) $(ASFLAGS)
X_CFLAGS	:=	$(MCFLAGS) $(CFLAGS)
X_LDFLAGS	:=	$(LDFLAGS)
X_LIBS		:=	$(LIBS) -lgcc

X_OUT		:=	../output
X_NAME		:=	$(patsubst %, $(X_OUT)/%, xboot)
X_INCDIRS	:=	$(patsubst %, -I %, $(INCDIRS))
X_LIBDIRS	:=	$(patsubst %, -L %, $(LIBDIRS))
X_SRCDIRS	:=	$(patsubst %, %, $(SRCDIRS))
X_OBJDIRS	:=	$(patsubst %, .obj/%, $(X_SRCDIRS))

X_ARCHS		:=	$(filter arch/%, $(X_SRCDIRS))
X_OTHERS	:=	$(filter-out arch/%, $(X_SRCDIRS))

X_SFILES	:=	$(foreach dir, $(X_ARCHS), $(wildcard $(dir)/*.S))
X_CFILES	:=	$(foreach dir, $(X_ARCHS), $(wildcard $(dir)/*.c))

X_SOBJS		:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o))
X_COBJS		:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o))
X_OBJS		:=	$(X_SOBJS) $(X_COBJS) $(foreach dir, $(X_OTHERS), .obj/$(dir)/built-in.o)

.PHONY: all clean romdisk xbegin xend xclean $(X_NAME) $(X_ARCHS) $(X_OTHERS)
export AS CC LD X_ASFLAGS X_CFLAGS X_INCDIRS

#
# Xboot rules
#
all : xend

xend : $(X_NAME)

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@ $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) -v -O binary $@ $@.bin

$(X_OBJS) : $(X_OTHERS)

$(X_OTHERS) : $(X_ARCHS)
	@$(MAKE) -s -f rules.mk SRCDIR=$@ NAME=.obj/$@/built-in.o

$(X_ARCHS) : xbegin
	@$(MAKE) -s -f rules.mk SRCDIR=$@

xbegin : romdisk

romdisk :
	@echo [ROMDISK] Packing romdisk
	@$(MKDIR) $(X_OBJDIRS) $(X_OUT)
	@$(RM) .obj/init/version.o
	@$(RM) .obj/driver/block/romdisk.o
	@$(RM) .obj/romdisk
	@$(RM) .obj/romdisk.cpio
	@$(CP) romdisk .obj
ifeq ($(strip $(CFG_FRAMEWORK)), y)
	@$(CP) framework/romdisk .obj
endif
	@$(CP) arch/$(ARCH)/$(MACH)/romdisk .obj
	@$(CD) .obj/romdisk && $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio

clean : xclean
	@$(RM) .obj $(X_OUT)
