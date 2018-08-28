#
# Makefile for xboot
# Create by: Jianjun Jiang <8192542@qq.com>
#

.PHONY: all clean

all:
	@$(MAKE) -C src all

clean:
	@$(MAKE) -C src clean
