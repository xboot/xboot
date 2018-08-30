#
# Makefile for libsandbox
#

AS			:= $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:= $(CROSS_COMPILE)gcc
CXX			:= $(CROSS_COMPILE)g++
LD			:= $(CROSS_COMPILE)ld
AR			:= $(CROSS_COMPILE)ar
OC			:= $(CROSS_COMPILE)objcopy
OD			:= $(CROSS_COMPILE)objdump
RM			:= rm -fr

ASFLAGS		+= -g -ggdb -Wall -O3
CFLAGS		+= -g -ggdb -Wall -O3
INCDIRS		:= -I .
SRCDIRS		:= .

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
OBJS		:= $(SOBJS) $(COBJS)
NAME		:= libsandbox

.PHONY: all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [AR] Archiving $@.a
	@$(AR) -rcs $@.a $(OBJS)

$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(OBJS) $(NAME).a
