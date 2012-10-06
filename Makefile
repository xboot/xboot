#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#

.PHONY: all clean module mclean

all:
	@$(MAKE) -C src all

clean:
	@$(MAKE) -C src clean
	
module:
	@$(MAKE) -C src module

mclean:
	@$(MAKE) -C src mclean
