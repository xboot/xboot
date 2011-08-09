#
# makefile for mkv310.
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
	$(CC) mkv310.c -o mkv310 -static
	$(ST) mkv310

clean:
	$(RM) mkv310 *.o *~
