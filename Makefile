#
# Top Makefile
#

export TOPDIR = $(shell dirname $(shell pwd)/$(lastword $(MAKEFILE_LIST)))

.PHONY: all clean

all:
	@$(MAKE) -s -C src all

clean:
	@$(MAKE) -s -C src clean
