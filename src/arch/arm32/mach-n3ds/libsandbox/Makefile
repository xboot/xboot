#
# Makefile for module.
#

CROSS_COMPILE ?= $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-

AS			:= $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:= $(CROSS_COMPILE)gcc
CXX			:= $(CROSS_COMPILE)g++
LD			:= $(CROSS_COMPILE)ld
AR			:= $(CROSS_COMPILE)gcc-ar
OC			:= $(CROSS_COMPILE)objcopy
OD			:= $(CROSS_COMPILE)objdump
RM			:= rm -fr

ASFLAGS		+= -g -ggdb -Wall -O3
CFLAGS		+= -g -ggdb -Wall -O3
MCFLAGS		+= -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -mword-relocations -fomit-frame-pointer -ffunction-sections
INCDIRS		+= -I . -I $(DEVKITPRO)/libctru/include
SRCDIRS		+= .

DEFINES		+= -DARM11 -D_3DS
ASFLAGS		+= -ffreestanding -std=gnu99 $(DEFINES)
CFLAGS		+= -ffreestanding -std=gnu99 $(DEFINES)

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
OBJS		:= $(SOBJS) $(COBJS)

OBJDIRS		:= $(patsubst %, %, $(SRCDIRS))
NAME		:= libsandbox
VPATH		:= $(OBJDIRS)

.PHONY:		all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [AR] Archiving $@.a
	@$(AR) -rcs $@.a $(OBJS)
	
$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) $(MCFLAGS) $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) $(MCFLAGS) $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(DEPS) $(OBJS) $(NAME).a *~
