#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#


.PHONY: all doc clean

all:
	@$(MAKE) -C src all

doc:
	@$(MAKE) -C src doc

clean:
	@$(MAKE) -C src clean
