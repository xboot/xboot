LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/inc -I$(LOCAL_DIR)/port

SRCS-y += \
	$(LOCAL_DIR)/src/nes.c \
	$(LOCAL_DIR)/src/nes_apu.c \
	$(LOCAL_DIR)/src/nes_cpu.c \
	$(LOCAL_DIR)/src/nes_default.c \
	$(LOCAL_DIR)/src/nes_mapper.c \
	$(LOCAL_DIR)/src/nes_ppu.c \
	$(LOCAL_DIR)/src/nes_rom.c \
	$(wildcard $(LOCAL_DIR)/src/nes_mapper/*.c) \
	$(LOCAL_DIR)/port/nes_port.c \
	$(LOCAL_DIR)/port/cmd-nes.c
