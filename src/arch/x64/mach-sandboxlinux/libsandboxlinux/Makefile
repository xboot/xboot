#
# Makefile for module.
#

CROSS_COMPILE	?= 
EXT_CFLAGS		?=
EXT_LIBS		?=

AS		:= $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC		:= $(CROSS_COMPILE)gcc
CXX		:= $(CROSS_COMPILE)g++
LD		:= $(CROSS_COMPILE)ld
AR		:= $(CROSS_COMPILE)ar
OC		:= $(CROSS_COMPILE)objcopy
OD		:= $(CROSS_COMPILE)objdump
RM		:= rm -fr


ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS		:=
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:= -m64 -mmmx -msse -msse2 -mssse3 -mfpmath=sse

LIBDIRS		:=
LIBS 		:= -lgcc -lc -lm -lpthread $(EXT_LIBS)

INCDIRS		:= -I . $(EXT_CFLAGS)
SRCDIRS		:= .


SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SDEPS		:= $(patsubst %, %, $(SFILES:.S=.o.d))
CDEPS		:= $(patsubst %, %, $(CFILES:.c=.o.d))
CPPDEPS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o.d))
DEPS		:= $(SDEPS) $(CDEPS) $(CPPDEPS)

SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
CPPOBJS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o)) 
OBJS		:= $(SOBJS) $(COBJS) $(CPPOBJS)

OBJDIRS		:= $(patsubst %, %, $(SRCDIRS))
NAME		:= libsandboxlinux
VPATH		:= $(OBJDIRS)

.PHONY:		all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [AR] Archiving $@.a
	@$(AR) $(ARFLAGS) $@.a $(OBJS)
	
$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) $(INCDIRS) -c $< -o $@
	@$(AS) $(ASFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(CPPOBJS) : %.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@	
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(DEPS) $(OBJS) $(NAME).a *~
	@echo Clean complete.
