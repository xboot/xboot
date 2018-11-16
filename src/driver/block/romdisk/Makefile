obj-y	+= data.o
obj-y	+= romdisk.o

PHONY	+= romdisk
PREPARE	:= romdisk

romdisk :
	@echo [CP] $(BUILD_SRC)/romdisk
	@$(CP) $(BUILD_SRC)/romdisk .
	@echo [CP] $(BUILD_SRC)/arch/$(ARCH)/$(MACH)/romdisk
	@$(CP) $(BUILD_SRC)/arch/$(ARCH)/$(MACH)/romdisk .
	@echo [CPIO] romdisk.cpio
	@$(CD) romdisk && $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio