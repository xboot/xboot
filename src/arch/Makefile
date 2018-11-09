extra-y	+= $(patsubst $(obj)/%, %, $(HEAD_OBJS))

obj-y	+= $(patsubst $(src)/%, %, $(wildcard $(src)/$(ARCH)/lib/cpu/))
obj-y	+= $(patsubst $(src)/%, %, $(wildcard $(src)/$(ARCH)/$(MACH)/command/))
obj-y	+= $(patsubst $(src)/%, %, $(wildcard $(src)/$(ARCH)/$(MACH)/driver/))

obj-y	+= $(filter-out $(extra-y), \
			$(patsubst $(src)/%.c, %.o, $(wildcard $(src)/$(ARCH)/$(MACH)/*.c)) \
			$(patsubst $(src)/%.S, %.o, $(wildcard $(src)/$(ARCH)/$(MACH)/*.S)) \
			$(patsubst $(src)/%.S, %.o, $(wildcard $(src)/$(ARCH)/lib/*.S)) \
			$(patsubst $(src)/%.c, %.o, $(wildcard $(src)/$(ARCH)/lib/*.c)))