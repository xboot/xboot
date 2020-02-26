SFILES	:= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))
CFILES	:= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))

SDEPS	:= $(patsubst %, .obj/%, $(SFILES:.S=.o.d))
CDEPS	:= $(patsubst %, .obj/%, $(CFILES:.c=.o.d))
DEPS	:= $(SDEPS) $(CDEPS)

SOBJS	:= $(patsubst %, .obj/%, $(SFILES:.S=.o))
COBJS	:= $(patsubst %, .obj/%, $(CFILES:.c=.o))
OBJS	:= $(SOBJS) $(COBJS)

ifneq ($(strip $(NAME)),)
all : $(NAME)
else
all : $(OBJS)
endif

$(NAME) : $(OBJS)
ifneq ($(strip $(OBJS)),)
	$(LD) -r -o $@ $^
else
	@$(AR) -rcs $@
endif

$(SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

sinclude $(DEPS)
