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
# override default make variables.
#
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# add pre-define macros.
#
ASFLAGS		+= 	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"
CFLAGS		+= 	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"
CXXFLAGS	+=	-D __ARCH__=\"$(ARCH)\" -D __MACH__=\"$(MACH)\"

#
# add necessary directory for INCDIRS and SRCDIRS.
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
				lib/libc/errno					\
				lib/libc/stdio					\
				lib/libc/stdlib					\
				lib/libc/string					\
				lib/libc/time					\
				lib/libc/readline				\
				lib/libm						\
				lib/lua							\
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
# you shouldn't need to change anything below this point.
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

XBOOT		:=	xboot
OUTDIR		:=	../output
OBJDIRS		:=	.obj $(patsubst %, .obj/%, $(SRCDIRS))

SFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
OBJS		:=	$(patsubst %, .obj/%, $(SFILES:.S=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o))
VPATH		:=	$(OBJDIRS)


.PHONY:		all doc clean fixup

all:		$(OUTDIR)/$(XBOOT).bin fixup

$(OUTDIR)/$(XBOOT).bin:			$(OUTDIR)/$(XBOOT).elf
	@echo [OC] objcopying $@
	@$(OC) $(OCFLAGS) $< $@

$(OUTDIR)/$(XBOOT).elf:			$(OBJS)
	@echo [LD] linking $@
	@$(CC) $(LDFLAGS) $(LIBDIRS) -Wl,--cref,-Map=$(OUTDIR)/$(XBOOT).map $^ -o $@ $(LIBS)

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

doc:
	@echo no documents

clean:
	@$(RM) $(OUTDIR) $(OBJDIRS)
	@echo clean complete.

# 
# include the dependency files, should be the last of the makefile
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
