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
				arch/$(ARCH)/lib/syscall \
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
				kernel/fs/tarfs \
				kernel/fs/cpiofs \
				kernel/fs/fatfs \
				drivers \
				drivers/console \
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
				drivers/input/mouse \
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
MKDIR		:=	mkdir -p
CP			:=	cp -a
RM			:=	rm -fr

XBOOT		:=	xboot
OUTDIR		:=	../output
OBJDIRS		:=	$(patsubst %, .obj/%, $(SRCDIRS)) $(OUTDIR)/sdk/include $(OUTDIR)/sdk/lib

SFILES		:=	$(wildcard arch/$(ARCH)/lib/syscall/*.s)
CFILES		:=	$(wildcard arch/$(ARCH)/lib/syscall/*.c)
CPPFILES	:=	$(wildcard arch/$(ARCH)/lib/syscall/*.cpp)
SYSCALLOBJS	:=	$(patsubst %, .obj/%, $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o))

SFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.s))
CFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
OBJS		:=	$(patsubst %, .obj/%, $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o))
VPATH		:=	$(OBJDIRS)


.PHONY:		all sdk docs clean final

all:		sdk $(OUTDIR)/$(XBOOT).bin final

$(OUTDIR)/$(XBOOT).bin:			$(OUTDIR)/$(XBOOT).elf
	@echo [OC] objcopying $@
	@$(OC) $(OCFLAGS) $< $@
	
$(OUTDIR)/$(XBOOT).elf:			$(OBJS)
	@echo [CPIO] packing ramdisk.cpio
	@cd ramdisk && { find . -not -name . | cpio -o -H newc --quiet > ../.obj/ramdisk.cpio; cd ..; }
	@echo [AS] drivers/ramdisk/binary.s
	@$(AS) $(MCFLAGS) $(ASFLAGS) $(INCDIRS) drivers/ramdisk/binary.s -o .obj/drivers/ramdisk/binary.o
	@echo [LD] linking $@
	@$(CC) $(LDFLAGS) $(LIBDIRS) $(LIBS) -Wl,--cref,-Map=$(OUTDIR)/$(XBOOT).map $^ -o $@

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

.obj/drivers/ramdisk/binary.o:	.obj/ramdisk.cpio

.obj/ramdisk.cpio:
	@echo [CPIO] packing ramdisk.cpio
	@cd ramdisk && { find . -not -name . | cpio -o -H newc --quiet > ../.obj/ramdisk.cpio; cd ..; }
	
sdk:		$(OBJS) $(SYSCALLOBJS)
	@echo [CP] copying header file
	@$(CP) arch/$(ARCH)/include/syscall.h $(OUTDIR)/sdk/include/syscall.h
	@echo [AR] aring $(OUTDIR)/libsyscall.a
	@$(AR) $(ARFLAGS) $(OUTDIR)/sdk/lib/libsyscall.a $(SYSCALLOBJS)
		
docs:
	@echo no documents
 	
clean:
	@$(RM) $(OUTDIR) $(OBJDIRS)
	@echo clean complete.

# 
# include the dependency files, should be the last of the makefile
#
sinclude $(shell $(MKDIR) $(OBJDIRS) $(OUTDIR); $(RM) .obj/init/version.o;) $(foreach dir, $(OBJDIRS), $(wildcard $(dir)/*.d))
