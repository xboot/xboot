#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#

#
# You must pass TARGET variable, for choosing platform.
# default TARGET is arm-realview.
#
TARGET		?= arm-realview

#
# Get platform information about ARCH and MACH from TARGET variable.
#
ifeq ($(words $(subst -, , $(TARGET))), 2)
ARCH		:= $(word 1, $(subst -, , $(TARGET)))
MACH		:= mach-$(word 2, $(subst -, , $(TARGET)))
else
ARCH		:= arm
MACH		:= mach-realview
endif

#
# System environment variable.
#
ifneq (,$(findstring Linux, $(shell uname -s)))
HOSTOS		:= linux
endif
ifneq (,$(findstring windows, $(shell uname -s)))
HOSTOS		:= windows
endif
ifeq ($(strip $(HOSTOS)),)
$(error unable to determine host operation system from uname)
endif

#
# Define default make variables.
#
ASFLAGS		:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

#
# Override default make variables.
#
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# Add pre-define macros.
#
ASFLAGS		+= 	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"
CFLAGS		+= 	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"
CXXFLAGS	+=	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
INCDIRS		+= 	-I include						\
				-I arch/$(ARCH)/include			\
				-I arch/$(ARCH)/$(MACH)/include

SRCDIRS		+=	arch/$(ARCH)/lib/gcc			\
				arch/$(ARCH)/lib/cpu			\
				arch/$(ARCH)/lib				\
				arch/$(ARCH)/$(MACH)			\
				arch/$(ARCH)/$(MACH)/driver		\
				arch/$(ARCH)/$(MACH)/resource	\
				lib								\
				lib/libc						\
				lib/libc/ctype					\
				lib/libc/environ				\
				lib/libc/errno					\
				lib/libc/stdio					\
				lib/libc/stdlib					\
				lib/libc/string					\
				lib/libc/time					\
				lib/libc/readline				\
				lib/libc/locale					\
				lib/libc/exit					\
				lib/libc/malloc					\
				lib/libm						\
				lib/xfs							\
				init							\
				init/mode						\
				kernel							\
				kernel/core						\
				kernel/shell					\
				kernel/command					\
				kernel/time						\
				kernel/graphic					\
				kernel/graphic/image			\
				kernel/graphic/maps				\
				kernel/graphic/maps/software	\
				kernel/fs						\
				kernel/fs/vfs					\
				kernel/fs/ramfs					\
				kernel/fs/procfs				\
				kernel/fs/devfs					\
				kernel/fs/arfs					\
				kernel/fs/tarfs					\
				kernel/fs/cpiofs				\
				kernel/fs/fatfs					\
				drivers							\
				drivers/console					\
				drivers/disk					\
				drivers/disk/partition			\
				drivers/serial					\
				drivers/fb						\
				drivers/led						\
				drivers/loop					\
				drivers/romdisk					\
				drivers/rtc						\
				drivers/input					\
				drivers/input/keyboard			\
				drivers/input/mouse				\
				drivers/mmc						\
				drivers/mtd						\
				drivers/mtd/nand				\
				drivers/mtd/nor

#
# Add external library
#
INCDIRS		+= 	-I external/lua-5.2.0
SRCDIRS		+=	external/lua-5.2.0

INCDIRS		+= 	-I external/zlib-1.2.6
SRCDIRS		+=	external/zlib-1.2.6

#
# You shouldn't need to change anything below this point.
#
CROSS		?=
AS			:=	$(CROSS)gcc -x assembler-with-cpp
CC			:=	$(CROSS)gcc
CXX			:=	$(CROSS)g++
LD			:=	$(CROSS)ld
AR			:=	$(CROSS)ar
OC			:=	$(CROSS)objcopy
OD			:=	$(CROSS)objdump
MKDIR		:=	mkdir -p
CP			:=	cp -af
RM			:=	rm -fr
CD			:=	cd
FIND		:=	find
CPIO		:=	cpio -o -H newc --quiet

OUTDIR		:=	../output
OBJDIRS		:=	.obj $(patsubst %, .obj/%, $(SRCDIRS))
XBOOT		:=	$(patsubst %, $(OUTDIR)/%, xboot)

SFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
OBJS		:=	$(patsubst %, .obj/%, $(SFILES:.S=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o))
VPATH		:=	$(OBJDIRS)

#
# Module variables
#
M_ASFLAGS	:=	$(MCFLAGS) $(ASFLAGS)
M_CFLAGS	:=	$(MCFLAGS) $(CFLAGS)
M_CXXFLAGS	:=	$(MCFLAGS) $(CXXFLAGS)
M_LDFLAGS	:=	-shared -nostdlib -fPIC

M_INCDIRS	:=	$(INCDIRS) $(patsubst %, -I $(M)/%, $(IDIRS))
M_SRCDIRS	:=	$(patsubst %, $(M)/%, $(SDIRS))

M_SFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.S))
M_CFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.c))
M_CPPFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.cpp))
M_OBJS		:=	$(patsubst %, %, $(M_SFILES:.S=.o) $(M_CFILES:.c=.o) $(M_CPPFILES:.cpp=.o))

M_NAME		:= 	$(patsubst %, $(M)/%, $(MODULE))


.PHONY:		all module clean fixup

all:		$(XBOOT) fixup
module:		$(M_NAME)

$(XBOOT):	$(OBJS)
	@echo [LD] Linking $@.elf
	@$(CC) $(LDFLAGS) $(LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@.elf $(LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) $(OCFLAGS) $@.elf $@.bin

$(M_NAME):	$(M_OBJS)
	@echo [M][LD] Linking $@.so
	@$(CC) $(M_LDFLAGS) -Wl,--cref,-Map=$@.map $^ -o $@.so

.obj/%.o:	%.S
	@echo [AS] $<
	@$(AS) $(MCFLAGS) $(ASFLAGS) $(INCDIRS) -c $< -o $@
	@$(AS) $(MCFLAGS) $(ASFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

.obj/%.o:	%.c
	@echo [CC] $<
	@$(CC) $(MCFLAGS) $(CFLAGS) $(INCDIRS) -c $< -o $@
	@$(CC) $(MCFLAGS) $(CFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

.obj/%.o:	%.cpp
	@echo [CXX] $<
	@$(CXX) $(MCFLAGS) $(CXXFLAGS) $(INCDIRS) -c $< -o $@	
	@$(CXX) $(MCFLAGS) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

%.o:		%.S
	@echo [M][AS] $<
	@$(AS) $(M_ASFLAGS) $(M_INCDIRS) -c $< -o $@
	@$(AS) $(M_ASFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

%.o:		%.c
	@echo [M][CC] $<
	@$(CC) $(M_CFLAGS) $(M_INCDIRS) -c $< -o $@
	@$(CC) $(M_CFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

%.o:		%.cpp
	@echo [M][CXX] $<
	@$(CXX) $(M_CXXFLAGS) $(M_INCDIRS) -c $< -o $@	
	@$(CXX) $(M_CXXFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

clean:
	@$(RM) $(OUTDIR) $(OBJDIRS)
	@echo Clean complete.

# 
# Include the dependency files, should be place the last of makefile
#
sinclude $(shell $(MKDIR) $(OBJDIRS) $(OUTDIR)							\
			&& $(RM) .obj/romdisk										\
			&& $(RM) .obj/romdisk.cpio									\
			&& $(RM) .obj/init/version.o								\
			&& $(RM) .obj/drivers/romdisk/data.o						\
			&& $(CP) romdisk .obj										\
			&& $(CP) arch/$(ARCH)/$(MACH)/romdisk .obj					\
			&& $(CD) .obj/romdisk										\
			&& $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio		\
			&& $(CD) ../..)												\
			$(foreach dir, $(OBJDIRS), $(wildcard $(dir)/*.d))
