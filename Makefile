#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#


.PHONY: all module module-clean clean

all:
	@$(MAKE) -C src all

module:
	@$(MAKE) -C src module

module-clean:
	@$(MAKE) -C src module-clean
		
clean:
	@$(MAKE) -C src clean
