#
# makefile for mkheader.
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
	$(CC) mkheader.c -o mkheader -static
	$(ST) mkheader

clean:
	$(RM) mkheader *.o *~
