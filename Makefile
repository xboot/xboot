#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#


.PHONY: all module clean

all:
	@$(MAKE) -C src all

module:
	@$(MAKE) -C src module
	
clean:
	@$(MAKE) -C src clean
