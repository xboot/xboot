obj-y	+= $(patsubst $(src)/%, %, $(wildcard $(src)/$(ARCH)/))
obj-y	+= $(patsubst $(src)/%.c, %.o, $(wildcard $(src)/*.c))
obj-y	+= $(patsubst $(src)/%.S, %.o, $(wildcard $(src)/*.S))