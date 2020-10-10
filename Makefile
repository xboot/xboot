#
# Top Makefile
#

TOPDIR = $(shell dirname $(shell pwd)/$(lastword $(MAKEFILE_LIST)))

.PHONY: all clean

all:
	@TOPDIR="$(TOPDIR)" $(MAKE) -s -C src all

clean:
	@TOPDIR="$(TOPDIR)" $(MAKE) -s -C src clean
