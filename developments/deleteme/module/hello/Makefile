#
# Makefile for module
#

#
# You must pass XBOOT_PATH CROSS_COMPILE and PLATFORM variable.
#
XBOOT_PATH		?=
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

AS			:= $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:= $(CROSS_COMPILE)gcc
CXX			:= $(CROSS_COMPILE)g++
LD			:= $(CROSS_COMPILE)ld
AR			:= $(CROSS_COMPILE)ar
OC			:= $(CROSS_COMPILE)objcopy
OD			:= $(CROSS_COMPILE)objdump
RM			:= rm -fr

ASFLAGS		:=	-g -ggdb -Wall -O3 -ffreestanding -fPIC
CFLAGS		:=	-g -ggdb -Wall -O3 -ffreestanding -fPIC
LDFLAGS		:=	-shared -nostartfiles -nodefaultlibs -nostdlib -fPIC
LIBDIRS		:=	-L .
LIBS		:=	-lgcc
INCDIRS		:=	-I $(XBOOT_PATH)/src/include \
				-I $(XBOOT_PATH)/src/arch/$(ARCH)/include \
				-I $(XBOOT_PATH)/src/arch/$(ARCH)/$(MACH)/include \
				-I $(XBOOT_PATH)/src/external/json-parser-1.1.0 \
				-I .
SRCDIRS		:=	.

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
OBJS		:= $(SOBJS) $(COBJS)
NAME		:= hello

.PHONY: all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [LD] Linking $@.so
	@$(CC) $(LDFLAGS) $(LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@.so $(LIBS)

$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(OBJS) $(NAME).map $(NAME).so
