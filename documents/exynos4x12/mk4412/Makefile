#
# makefile for mk4412.
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
	$(CC) mk4412.c -o mk4412 -static
	$(ST) mk4412

clean:
	$(RM) mk4412 *.o *~
