#
# makefile for xboot.
# create by: jianjun jiang <jerryjianjun@gmail.com>.
#

#
# you must pass TARGET variable, for choosing platform.
# default TARGET is arm-realview.
#
TARGET		?= arm-realview

#
# get platform information about ARCH and MACH from TARGET variable.
#
ifeq ($(words $(subst -, , $(TARGET))), 2)
ARCH		:= $(word 1, $(subst -, , $(TARGET)))
MACH		:= mach-$(word 2, $(subst -, , $(TARGET)))
else
ARCH		:= arm
MACH		:= mach-realview
endif

#
# system environment variable.
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
# define default make variables.
#
ASFLAGS		:= --gstabs --warn
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles -nostdlib -nodefaultlibs -nostdinc
ARFLAGS		:=
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=

INCDIRS		:=
SRCDIRS		:=

#
# override default make variables.
#
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# add pre-define macros.
#
CFLAGS		+= 	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"
CXXFLAGS	+=	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"

#
# add necessary directory for INCDIRS and SRCDIRS.
#
INCDIRS		+= 	-I include \
				-I arch/$(ARCH)/include \
				-I arch/$(ARCH)/$(MACH)/include

SRCDIRS		+=	arch/$(ARCH)/lib/gcc \
				arch/$(ARCH)/lib/cpu \
				arch/$(ARCH)/lib/cmd \
				arch/$(ARCH)/lib \
				arch/$(ARCH)/$(MACH) \
				arch/$(ARCH)/$(MACH)/driver \
				arch/$(ARCH)/$(MACH)/resource \
				init \
				lib \
				lib/libc \
				kernel \
				kernel/core \
				kernel/shell \
				kernel/command \
				kernel/time \
				kernel/gui \
				kernel/fs \
				kernel/fs/vfs \
				kernel/fs/ramfs \
				kernel/fs/procfs \
				kernel/fs/devfs \
				kernel/fs/arfs \
				kernel/fs/fatfs \
				drivers \
				drivers/disk \
				drivers/disk/partition \
				drivers/serial \
				drivers/terminal \
				drivers/fb \
				drivers/fb/reader \
				drivers/led \
				drivers/loop \
				drivers/ramdisk \
				drivers/rtc \
				drivers/input \
				drivers/input/keyboard \
				drivers/mmc \
				drivers/mtd \
				drivers/mtd/nand \
				drivers/mtd/nor

#
# you shouldn't need to change anything below this point.
#
CROSS		?=
AS			:=	$(CROSS)as
CC			:=	$(CROSS)gcc
CXX			:=	$(CROSS)g++
LD			:=	$(CROSS)ld
AR			:=	$(CROSS)ar
OC			:=	$(CROSS)objcopy
OD			:=	$(CROSS)objdump
RM			:=	-rm -fr

XBOOT_NAME	:=	xboot
OBJDIRS		:=	.obj $(patsubst %, .obj/%, $(SRCDIRS))
OUTDIR		:=	../output

SFILES		:=	$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s))
CFILES		:=	$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
OBJS		:=	$(patsubst %, .obj/%, $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o))
VPATH		:=	$(OBJDIRS)

COMMAND		:= 	mkdir -p $(OBJDIRS) $(OUTDIR);			\
				rm -fr .obj/init/version.o;				\
				rm -fr .obj/drivers/ramdisk/wrapper.o;	\
				find ramdisk/ -name '*.*' | xargs ar -rc .obj/ramdisk.ar

.PHONY: all final docs clean

all:	$(OBJS) $(OUTDIR)/$(XBOOT_NAME).elf $(OUTDIR)/$(XBOOT_NAME).bin final

$(OUTDIR)/$(XBOOT_NAME).bin:	$(OUTDIR)/$(XBOOT_NAME).elf
	@echo [OC] objcopy $@
	@$(OC) $(OCFLAGS) $< $@
	
$(OUTDIR)/$(XBOOT_NAME).elf:	$(OBJS)
	@echo [LD] linking $@
	@$(LD) $(LDFLAGS) $(LIBDIRS) $(LIBS) -Map $(OUTDIR)/$(XBOOT_NAME).map -o $@ $^

.obj/ramdisk:	
	@echo [RAMDISK] generate ramdisk $@

.obj/%.o:	%.s
	@echo [AS] $<
	@$(AS) $(MCFLAGS) $(ASFLAGS) $(INCDIRS) $< -o $@

.obj/%.o:	%.c
	@echo [CC] $<
	@$(CC) $(MCFLAGS) $(CFLAGS) $(INCDIRS) -c $< -o $@
	@$(CC) $(MCFLAGS) $(CFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@
	
.obj/%.o:	%.cpp
	@echo [CXX] $<
	@$(CXX) $(MCFLAGS) $(CXXFLAGS) $(INCDIRS) -c $< -o $@	
	@$(CXX) $(MCFLAGS) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

docs:
	@echo no documents
 	
clean:
	@$(RM) $(OUTDIR) $(OBJDIRS)
	@echo clean complete.

# 
# include the dependency files, should be the last of the makefile
#
sinclude $(shell $(COMMAND)) $(foreach dir,$(OBJDIRS),$(wildcard $(dir)/*.d))
