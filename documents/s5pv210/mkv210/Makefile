#
# makefile for mkv210.
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
	$(CC) mkv210.c -o mkv210 -static
	$(ST) mkv210

clean:
	$(RM) mkv210 *.o *~
