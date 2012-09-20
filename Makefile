#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#


.PHONY: all module mclean clean

all:
	@$(MAKE) -C src all

module:
	@$(MAKE) -C src module

mclean:
	@$(MAKE) -C src mclean

clean:
	@$(MAKE) -C src clean
