PHONY	+= conf

conf : $(obj)/config/autoconf.h

$(obj)/config/autoconf.h : $(src)/xconfigs.h \
		$(wildcard $(BUILD_SRC)/arch/$(ARCH)/$(MACH)/include/configs.h)
	$(if $(wildcard $(obj)/config),:,@echo [MKDIR] $(obj)/config && $(MKDIR) $(obj)/config)
	$(if $(wildcard $(obj)/config/autoconf.h),:,@touch $(obj)/config/autoconf.h)
	@$(CC) -E -P -dM -I$(BUILD_SRC)/arch/$(ARCH)/$(MACH)/include $(src)/xconfigs.h \
	| sed -n -e "/\#define\s\+CONFIG_[[:alnum:]_]*/p" \
	> $(obj)/config/autoconf.temp
	@$(CD) $(obj)/config && grep -vxf autoconf.temp autoconf.h \
	| sed -n -e "s/\#define\s\+CONFIG_\([[:alnum:]_]*\).*/\L\1\E.h/p" \
	| xargs $(RM)
	@$(CD) $(obj)/config && grep -vxf autoconf.h autoconf.temp \
	| sed -n -e "s/\#define\s\+CONFIG_\([[:alnum:]_]*\).*/\L\1\E.h/p" \
	| xargs touch autoconf.h
	@$(RM) $(obj)/config/autoconf.h
	@$(MV) $(obj)/config/autoconf.temp $(obj)/config/autoconf.h