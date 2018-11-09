extra-y	+= start.o
extra-y	+= $(patsubst $(src)/%.c, %.o, $(wildcard $(src)/sys-*.c))
obj-y	+= $(filter-out $(extra-y), \
			$(patsubst $(src)/%.c, %.o, $(wildcard $(src)/*.c)) \
			$(patsubst $(src)/%.S, %.o, $(wildcard $(src)/*.S)))

HEAD_OBJS	:= $(ARCH)/$(MACH)/start.c $(ARCH)/$(MACH)/sys-*.c $(ARCH)/lib/memset.S $(ARCH)/lib/memset.o