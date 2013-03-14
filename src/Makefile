#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#

#
# You must pass TARGET and CROSS variable.
#
TARGET		?= arm32-realview
CROSS		?= arm-none-eabi-

#
# Get platform information about ARCH and MACH from TARGET variable.
#
ifeq ($(words $(subst -, , $(TARGET))), 2)
ARCH		:= $(word 1, $(subst -, , $(TARGET)))
MACH		:= mach-$(word 2, $(subst -, , $(TARGET)))
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
# Load default variables.
#
ASFLAGS	:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS	:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
ARFLAGS	:= -rcs
OCFLAGS	:= -v -O binary
ODFLAGS	:=
MCFLAGS	:=

LIBDIRS	:=
LIBS 		:=

INCDIRS	:=
SRCDIRS	:=

#
# Override default variables.
#
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# Modify default variables.
#
NAMESPACES	+=	-D__ARCH__=\"$(ARCH)\" -D__MACH__=\"$(MACH)\"
ASFLAGS	+= 	$(NAMESPACES)
CFLAGS		+=	$(NAMESPACES)
CXXFLAGS	+=	$(NAMESPACES)

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
INCDIRS	+=	include									\
				arch/$(ARCH)/include						\
				arch/$(ARCH)/$(MACH)/include

SRCDIRS	+=	arch/$(ARCH)/lib/gcc						\
				arch/$(ARCH)/lib/cpu						\
				arch/$(ARCH)/lib							\
				arch/$(ARCH)/$(MACH)						\
				arch/$(ARCH)/$(MACH)/driver				\
				arch/$(ARCH)/$(MACH)/resource			\
				lib											\
				lib/elf									\
				lib/libc									\
				lib/libc/ctype							\
				lib/libc/environ							\
				lib/libc/errno							\
				lib/libc/exit								\
				lib/libc/fifo								\
				lib/libc/readline							\
				lib/libc/stdio							\
				lib/libc/stdlib							\
				lib/libc/string							\
				lib/libc/time								\
				lib/libc/locale							\
				lib/libc/malloc							\
				lib/libm									\
				lib/libx									\
				init										\
				init/mode									\
				kernel										\
				kernel/core								\
				kernel/shell								\
				kernel/command							\
				kernel/time								\
				kernel/graphic							\
				kernel/graphic/image						\
				kernel/graphic/maps						\
				kernel/graphic/maps/software			\
				kernel/fs									\
				kernel/fs/vfs								\
				kernel/fs/ramfs							\
				kernel/fs/procfs							\
				kernel/fs/devfs							\
				kernel/fs/arfs							\
				kernel/fs/tarfs							\
				kernel/fs/cpiofs							\
				kernel/fs/fatfs							\
				drivers									\
				drivers/console							\
				drivers/disk								\
				drivers/disk/partition					\
				drivers/serial							\
				drivers/fb									\
				drivers/led								\
				drivers/loop								\
				drivers/romdisk							\
				drivers/rtc								\
				drivers/input								\
				drivers/input/keyboard					\
				drivers/input/mouse						\
				drivers/mmc								\
				drivers/mtd								\
				drivers/mtd/nand							\
				drivers/mtd/nor							\
				framework									\
				framework/base64							\
				framework/cairo							\
				framework/event							\
				framework/xfs								\
				framework/framerate

#
# Add external library
#
INCDIRS	+= 	external/lua-5.2.1
SRCDIRS	+=	external/lua-5.2.1

INCDIRS	+= 	external/zlib-1.2.7
SRCDIRS	+=	external/zlib-1.2.7

INCDIRS	+= 	external/libpng-1.5.14
SRCDIRS	+=	external/libpng-1.5.14

INCDIRS	+= 	external/pixman-0.28.2
SRCDIRS	+=	external/pixman-0.28.2

INCDIRS	+= 	external/cairo-1.12.14
SRCDIRS	+=	external/cairo-1.12.14

INCDIRS	+= 	external/freetype-2.4.11/include		\
				external/freetype-2.4.11/src/base		\
				external/freetype-2.4.11/src/raster	\
				external/freetype-2.4.11/src/smooth	\
				external/freetype-2.4.11/src/cff		\
				external/freetype-2.4.11/src/cid		\
				external/freetype-2.4.11/src/pfr		\
				external/freetype-2.4.11/src/sfnt		\
				external/freetype-2.4.11/src/truetype	\
				external/freetype-2.4.11/src/type1		\
				external/freetype-2.4.11/src/type42	\
				external/freetype-2.4.11/src/winfonts	\
				external/freetype-2.4.11/src/autofit 	\
				external/freetype-2.4.11/src/cache 	\
				external/freetype-2.4.11/src/gxvalid 	\
				external/freetype-2.4.11/src/otvalid 	\
				external/freetype-2.4.11/src/psaux 	\
				external/freetype-2.4.11/src/pshinter 	\
				external/freetype-2.4.11/src/psnames
SRCDIRS	+=	external/freetype-2.4.11/src/base 		\
				external/freetype-2.4.11/src/raster	\
				external/freetype-2.4.11/src/smooth	\
				external/freetype-2.4.11/src/cff		\
				external/freetype-2.4.11/src/cid		\
				external/freetype-2.4.11/src/pfr		\
				external/freetype-2.4.11/src/sfnt		\
				external/freetype-2.4.11/src/truetype	\
				external/freetype-2.4.11/src/type1		\
				external/freetype-2.4.11/src/type42	\
				external/freetype-2.4.11/src/winfonts	\
				external/freetype-2.4.11/src/autofit 	\
				external/freetype-2.4.11/src/cache 	\
				external/freetype-2.4.11/src/gxvalid 	\
				external/freetype-2.4.11/src/otvalid 	\
				external/freetype-2.4.11/src/psaux 	\
				external/freetype-2.4.11/src/pshinter 	\
				external/freetype-2.4.11/src/psnames

#
# You shouldn't need to change anything below this point.
#
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

#
# Xboot variables
#
X_ASFLAGS	:=	$(MCFLAGS) $(ASFLAGS)
X_CFLAGS	:=	$(MCFLAGS) $(CFLAGS)
X_CXXFLAGS	:=	$(MCFLAGS) $(CXXFLAGS)
X_LDFLAGS	:=	$(LDFLAGS)
X_OCFLAGS	:=	$(OCFLAGS)
X_LIBDIRS	:=	$(LIBDIRS)
X_LIBS		:=	$(LIBS) -lgcc

X_OUT		:=	../output
X_NAME		:=	$(patsubst %, $(X_OUT)/%, xboot)
X_INCDIRS	:=	$(patsubst %, -I %, $(INCDIRS))
X_SRCDIRS	:=	$(patsubst %, %, $(SRCDIRS))
X_OBJDIRS	:=	$(patsubst %, .obj/%, $(X_SRCDIRS))

X_SFILES	:=	$(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.S))
X_CFILES	:=	$(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.c))
X_CPPFILES	:=	$(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.cpp))

X_SDEPS	:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o.d))
X_CDEPS	:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o.d))
X_CPPDEPS	:=	$(patsubst %, .obj/%, $(X_CPPFILES:.cpp=.o.d))
X_DEPS		:=	$(X_SDEPS) $(X_CDEPS) $(X_CPPDEPS)

X_SOBJS	:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o))
X_COBJS	:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o))
X_CPPOBJS	:=	$(patsubst %, .obj/%, $(X_CPPFILES:.cpp=.o)) 
X_OBJS		:=	$(X_SOBJS) $(X_COBJS) $(X_CPPOBJS)

#
# Module variables
#
M_ASFLAGS	:=	$(NAMESPACES) $(MODULE_ASFLAGS)
M_CFLAGS	:=	$(NAMESPACES) $(MODULE_CFLAGS)
M_CXXFLAGS	:=	$(NAMESPACES) $(MODULE_CXXFLAGS)
M_LDFLAGS	:=	$(MODULE_LDFLAGS) -shared -nostdlib -fPIC
M_LIBDIRS	:=	$(MODULE_LIB_SEARCH_DIRS)
M_LIBS		:=	$(MODULE_LIB_LINKS) -lgcc

M_NAME		:= 	$(patsubst %, $(M)/%, $(MODULE_NAME))
M_INCDIRS	:=	$(X_INCDIRS) $(patsubst %, -I $(M)/%, $(MODULE_INCLUDE_DIRS))
M_SRCDIRS	:=	$(patsubst %, $(M)/%, $(MODULE_SOURCE_DIRS))
M_OBJDIRS	:=	$(patsubst %, %, $(M_SRCDIRS))

M_SFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.S))
M_CFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.c))
M_CPPFILES	:=	$(foreach dir, $(M_SRCDIRS), $(wildcard $(dir)/*.cpp))

M_SDEPS	:=	$(M_SFILES:.S=.o.d)
M_CDEPS	:=	$(M_CFILES:.c=.o.d)
M_CPPDEPS	:=	$(M_CPPFILES:.cpp=.o.d)
M_DEPS		:=	$(M_SDEPS) $(M_CDEPS) $(M_CPPDEPS)

M_SOBJS	:=	$(M_SFILES:.S=.o)
M_COBJS	:=	$(M_CFILES:.c=.o)
M_CPPOBJS	:=	$(M_CPPFILES:.cpp=.o)
M_OBJS		:=	$(M_SOBJS) $(M_COBJS) $(M_CPPOBJS)

VPATH		:=	$(X_OBJDIRS) $(M_OBJDIRS)
.PHONY:	all clean xclean fixup module mclean

#
# Xboot rules
#
all : $(X_NAME) fixup
clean : xclean

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@.elf
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@.elf $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) $(X_OCFLAGS) $@.elf $@.bin

$(X_SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(X_ASFLAGS) $(X_INCDIRS) -c $< -o $@
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(X_CFLAGS) $(X_INCDIRS) -c $< -o $@
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_CPPOBJS) : .obj/%.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(X_CXXFLAGS) $(X_INCDIRS) -c $< -o $@	
	@$(CXX) $(X_CXXFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

xclean:
	@$(RM) $(X_DEPS) $(X_OBJS) $(X_OBJDIRS) $(X_OUT)
	@echo Clean xboot complete.

#
# Module rules
#
module : $(M_NAME)

$(M_NAME) : $(M_OBJS)
	@echo [M][LD] Linking $@.so
	@$(CC) $(M_LDFLAGS) $(M_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@.so $(M_LIBS)

$(M_SOBJS) : %.o : %.S
	@echo [M][AS] $<
	@$(AS) $(M_ASFLAGS) $(M_INCDIRS) -c $< -o $@
	@$(AS) $(M_ASFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

$(M_COBJS) : %.o : %.c
	@echo [M][CC] $<
	@$(CC) $(M_CFLAGS) $(M_INCDIRS) -c $< -o $@
	@$(CC) $(M_CFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

$(M_CPPOBJS) : %.o : %.cpp
	@echo [M][CXX] $<
	@$(CXX) $(M_CXXFLAGS) $(M_INCDIRS) -c $< -o $@	
	@$(CXX) $(M_CXXFLAGS) -MD -MP -MF $@.d $(M_INCDIRS) -c $< -o $@

mclean:
	@$(RM) $(M_DEPS) $(M_OBJS) $(M_NAME).map $(M_NAME).so
	@echo Clean module complete.

#
# Include the dependency files, should be place the last of makefile
#
sinclude $(shell $(MKDIR) $(X_OBJDIRS) $(X_OUT)						\
			&& $(RM) .obj/romdisk										\
			&& $(RM) .obj/romdisk.cpio									\
			&& $(RM) .obj/init/version.o								\
			&& $(RM) .obj/drivers/romdisk/data.o						\
			&& $(CP) romdisk .obj										\
			&& $(CP) arch/$(ARCH)/$(MACH)/romdisk .obj				\
			&& $(CD) .obj/romdisk										\
			&& $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio	\
			&& $(CD) ../..)												\
			$(X_DEPS) $(M_DEPS)
