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
CXXFLAGS	+= -g -ggdb -Wall -O3
MCFLAGS		+=
INCDIRS		+= -I .
SRCDIRS		+= .

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
CPPOBJS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o)) 
OBJS		:= $(SOBJS) $(COBJS) $(CPPOBJS)

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

$(CPPOBJS) : %.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $(MCFLAGS) $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(OBJS) $(NAME).a
	@echo Clean complete.
