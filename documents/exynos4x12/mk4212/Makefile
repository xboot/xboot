#
# makefile for mk4212.
# create by: jianjun jiang <jerryjianjun@gmail.com>.
#

CROSS		?=
AS			:= $(CROSS)as
CC			:= $(CROSS)gcc
CXX			:= $(CROSS)g++
LD			:= $(CROSS)ld
AR			:= $(CROSS)ar
OC			:= $(CROSS)objcopy
OD			:= $(CROSS)objdump
ST			:= $(CROSS)strip
RM			:= -rm -fr

.PHONY: all docs clean

all:
	$(CC) mk4212.c -o mk4212 -static
	$(ST) mk4212

clean:
	$(RM) mk4212 *.o *~
