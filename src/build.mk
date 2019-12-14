all :

X_SFILES	:=	$(foreach dir, $(X_SRCDIR), $(wildcard $(dir)/*.S))
X_CFILES	:=	$(foreach dir, $(X_SRCDIR), $(wildcard $(dir)/*.c))

X_SDEPS		:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o.d))
X_CDEPS		:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o.d))
X_DEPS		:=	$(X_SDEPS) $(X_CDEPS)

X_SOBJS		:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o))
X_COBJS		:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o))
X_OBJS		:=	$(X_SOBJS) $(X_COBJS)

sinclude $(X_DEPS)

.PHONY:	all

ifneq ($(strip $(X_NAME)),)
all : $(X_NAME) ;
else
#Just compiler, dont link
all : $(X_OBJS) ;
endif

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@
ifneq ($(strip $(X_OBJS)),)
	@$(LD) -r -o $@ $^
else
	$(RM) $@;$(AR) rcs $@
endif


$(X_SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@