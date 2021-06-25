#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A7__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=vfpv4 -mfloat-abi=hard -marm -mno-thumb-interwork -mno-unaligned-access

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/linux/mksunxi
MKZ			:= arch/$(ARCH)/$(MACH)/tools/linux/mkz
endif
ifeq ($(strip $(HOSTOS)), windows)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/windows/mksunxi
MKZ			:= arch/$(ARCH)/$(MACH)/tools/windows/mkz
endif

UNIQUEID	:= ""
ENCRYPT_KEY	:= "679408dc82ae80d411d5d9720b65a43fc4f1534fa563fb28c6cd8928e46aaae9"
PUBLIC_KEY	:= "03cfd18e4a4b40d6529448aa2df8bbb677128258b8fbfc5b9e492fbbba4e84832f"
PRIVATE_KEY	:= "dc57b8a9e0e2b7f8b4c929bd8db2844e53f01f171bbcdf6e628908dbf2b2e6a9"
MESSAGE		:= "https://github.com/xboot/xboot"

xend:
	@echo Make header information for brom booting
	@$(MKSUNXI) $(X_NAME).bin
	@$(MKZ) -majoy 3 -minior 0 -patch 0 -r 65536 -k $(ENCRYPT_KEY) -pb $(PUBLIC_KEY) -pv $(PRIVATE_KEY) -m $(MESSAGE) -g $(UNIQUEID) -i $(UNIQUEID) $(X_NAME).bin $(X_NAME).bin.z
