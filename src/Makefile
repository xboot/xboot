#
# Makefile for xboot
#
PHONY	:=

#
# You must pass CROSS_COMPILE and PLATFORM variable.
#
CROSS_COMPILE	?=
PLATFORM		?=

#
# Get platform information about ARCH and MACH from PLATFORM variable.
#
ifeq ($(words $(subst -, , $(PLATFORM))), 2)
ARCH		:= $(word 1, $(subst -, , $(PLATFORM)))
MACH		:= mach-$(word 2, $(subst -, , $(PLATFORM)))
else
ARCH		:= x64
MACH		:= mach-sandbox
endif

#
# System environment variable.
#
ifneq (,$(findstring Linux, $(shell uname -a)))
HOSTOS		:= linux
endif
ifneq (,$(findstring windows, $(shell uname -a)))
HOSTOS		:= windows
endif
ifneq (,$(findstring Darwin, $(shell uname -a)))
HOSTOS		:= macos
endif

#
# Load default variables.
#
ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=
ARCHDIRS	:=
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
ifeq ($(strip $(ARCH)), x64)
DEFINES		+=	-D__X64__
endif

ASFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
CFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
LDFLAGS		+=	-Wl,-gc-sections

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
SRCDIRS		+=	arch lib init kernel driver framework external

#
# Add external library
#
INCDIRS		+=	include										\
				$(wildcard arch/$(ARCH)/include)			\
				$(wildcard arch/$(ARCH)/$(MACH)/include)	\
				external/libfdt-1.4.2						\
				external/lz4-1.8.2							\
				external/zlib-1.2.11						\
				external/libpng-1.6.32						\
				external/lua-5.3.4							\
				external/lua-cjson-2.1.0					\
				external/pixman-0.34.0						\
				$(wildcard external/pixman-0.34.0/$(ARCH))	\
				external/cairo-1.14.10						\
				external/freetype-2.8/include				\
				external/chipmunk-7.0.2/include

#
# You shouldn't need to change anything below this point.
#
AS			:=	$(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:=	$(CROSS_COMPILE)gcc
CXX			:=	$(CROSS_COMPILE)g++
LD			:=	$(CROSS_COMPILE)ld
AR			:=	$(CROSS_COMPILE)ar
OC			:=	$(CROSS_COMPILE)objcopy
OD			:=	$(CROSS_COMPILE)objdump
MKDIR		:=	mkdir -p
CP			:=	cp -af
RM			:=	rm -fr
CD			:=	cd
MV			:=	mv
FIND		:=	find

ifeq ($(strip $(HOSTOS)), macos)
CPIO		:=	gcpio -o -H newc --quiet
else
CPIO		:=	cpio -o -H newc --quiet
endif

#
# Xboot variables
#

TOPDIR		:=	$(CURDIR)

BUILD_SRC	:=	$(CURDIR)
BUILD_OBJ	:=	.obj

HEAD_FILES	:=
HEAD_OBJS	:=

sinclude arch/$(ARCH)/$(MACH)/Makefile.head

HEAD_FILES	+=	arch/$(ARCH)/$(MACH)/start.S arch/$(ARCH)/$(MACH)/sys-*.c arch/$(ARCH)/$(MACH)/sys-*.S
HEAD_OBJS	+=	$(patsubst %.S, %.o, $(filter %.S, $(wildcard $(HEAD_FILES))))
HEAD_OBJS	+=	$(patsubst %.c, %.o, $(filter %.c, $(wildcard $(HEAD_FILES))))

X_ASFLAGS	:=	$(MCFLAGS) $(ASFLAGS)
X_CFLAGS	:=	$(MCFLAGS) $(CFLAGS)
X_LDFLAGS	:=	$(LDFLAGS)
X_LIBS		:=	$(LIBS) -lgcc

X_OUT		:=	../output
X_NAME		:=	$(patsubst %, $(X_OUT)/%, xboot)
X_INCDIRS	:=	$(patsubst %, -I $(BUILD_SRC)/%, $(INCDIRS)) -include include/config/autoconf.h
X_LIBDIRS	:=	$(patsubst %, -L %, $(LIBDIRS))
X_OBJS		:=	$(patsubst %, $(BUILD_OBJ)/%/built-in.o, $(wildcard $(SRCDIRS)))


export TOPDIR BUILD_SRC BUILD_OBJ HEAD_OBJS HOSTOS
export ARCH MACH
export AS AR CC LD RM CP CD MV FIND CPIO MKDIR
export X_ASFLAGS X_INCDIRS X_CFLAGS

PHONY	+=	all prepare clean xbegin xend xclean prepare prepare1 prepare2 $(SRCDIRS)

#
# Xboot rules
#
all : xend

xend : $(X_NAME)

$(SRCDIRS) : xbegin

xbegin : prepare1 prepare2

prepare :
	$(if $(wildcard $(X_OUT)),:,@echo [MKDIR] $(X_OUT) && $(MKDIR) $(X_OUT))
	$(if $(wildcard $(BUILD_OBJ)),:,@echo [MKDIR] $(BUILD_OBJ) && $(MKDIR) $(BUILD_OBJ))

prepare1 : prepare
	@$(MAKE) -C $(BUILD_OBJ) -f $(BUILD_SRC)/scripts/Makefile.build obj=include conf

prepare2 : prepare
	@$(RM) $(BUILD_OBJ)/romdisk
	@$(RM) $(BUILD_OBJ)/romdisk.cpio
	@$(RM) $(BUILD_OBJ)/init/version.o
	@$(RM) $(BUILD_OBJ)/driver/block/romdisk/data.o

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $(patsubst %, $(BUILD_OBJ)/%, $(HEAD_OBJS)) $(X_OBJS) -o $@ $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) -v -O binary $@ $@.bin

$(X_OBJS) : $(SRCDIRS) ;

$(SRCDIRS) :
	@$(MAKE) -C $(BUILD_OBJ) -f $(BUILD_SRC)/scripts/Makefile.build obj=$@

clean : xclean
	@echo [RM] $(BUILD_OBJ) $(X_OUT)
	@$(RM) $(BUILD_OBJ) $(X_OUT)

PHONY += FORCE

FORCE:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
