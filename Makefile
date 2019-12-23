#
# Top makefile
#

.PHONY: all clean

all:
	@$(MAKE) -s -C src all

clean:
	@$(MAKE) -s -C src clean
