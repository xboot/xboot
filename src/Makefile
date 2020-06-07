#
# Makefile for xboot
#

#
# You can pass CROSS_COMPILE, PLATFORM and WBOXTEST variable.
#
CROSS_COMPILE	?=
PLATFORM		?=
WBOXTEST		?=

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
# Modify default variables.
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
				lib/libc/* \
				lib/libm/$(ARCH) \
				lib/libm \
				lib/libx \
				init \
				kernel/* \
				kernel/vfs/* \
				driver/* \
				driver/block/partition \
				driver/input/rc \
				framework \
				framework/*

#
# Add external library
#
INCDIRS		+= 	external/libfdt-1.4.2
SRCDIRS		+=	external/libfdt-1.4.2

INCDIRS		+= 	external/lz4-1.8.2
SRCDIRS		+=	external/lz4-1.8.2

INCDIRS		+= 	external/zlib-1.2.11
SRCDIRS		+=	external/zlib-1.2.11

INCDIRS		+= 	external/libpng-1.6.37
SRCDIRS		+=	external/libpng-1.6.37

INCDIRS		+= 	external/jpeg-9c
SRCDIRS		+=	external/jpeg-9c

INCDIRS		+= 	external/lua-5.3.5
SRCDIRS		+=	external/lua-5.3.5

INCDIRS		+= 	external/lua-cjson-2.1.0
SRCDIRS		+=	external/lua-cjson-2.1.0

INCDIRS		+= 	external/freetype-2.10.1/include \
				external/freetype-2.10.1/src/*
SRCDIRS		+=	external/freetype-2.10.1/src/*

INCDIRS		+= 	external/pixman-0.38.4 \
				external/pixman-0.38.4/$(ARCH)
SRCDIRS		+=	external/pixman-0.38.4 \
				external/pixman-0.38.4/$(ARCH)

INCDIRS		+= 	external/cairo-1.17.2
SRCDIRS		+=	external/cairo-1.17.2

INCDIRS		+=	external/chipmunk-7.0.3/include
SRCDIRS		+=	external/chipmunk-7.0.3/src

#
# Add whitebox test
#
ifneq ($(strip $(WBOXTEST)),)
INCDIRS		+=	wboxtest
SRCDIRS		+=	wboxtest \
				wboxtest/*
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

EXTEND_DIR	=	$(patsubst %/, %, $(foreach dir, $(1),$(filter %/,$(wildcard $(dir)/)))) 

X_INCDIRS	:=	$(patsubst %, -I %, $(call EXTEND_DIR,$(INCDIRS)))
X_LIBDIRS	:=	$(patsubst %, -L %, $(call EXTEND_DIR,$(LIBDIRS)))
X_SRCDIRS	:=	$(patsubst %, %, $(sort $(call EXTEND_DIR,$(SRCDIRS))))
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
	@$(MKDIR) $(X_OBJDIRS) $(X_OUT) \
		&& $(RM) .obj/init/version.o \
		&& $(RM) .obj/driver/block/romdisk.o \
		&& $(RM) .obj/romdisk \
		&& $(RM) .obj/romdisk.cpio \
		&& $(CP) romdisk .obj \
		&& $(CP) arch/$(ARCH)/$(MACH)/romdisk .obj \
		&& $(CD) .obj/romdisk \
		&& $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio \
		&& $(CD) ../..

clean : xclean
	@$(RM) .obj $(X_OUT)
