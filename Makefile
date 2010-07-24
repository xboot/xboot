#
# Makefile for xboot.
# Create by: jianjun jiang <jerryjianjun@gmail.com>.
#


.PHONY: all docs clean

all:
	@$(MAKE) -C src all

docs:
	@$(MAKE) -C src docs

clean:
	@$(MAKE) -C src clean
