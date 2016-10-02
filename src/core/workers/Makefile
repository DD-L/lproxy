CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
ROOT = $(PWD)/../
LINK = ./link

LOG_SRC = $(ROOT)/log

CFLAGS =  -Wall -D_REENTRANT $(Container_Type) -g -std=gnu++0x   # win32: -std=c++11


INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ \
		  -I$(ROOT)/ -I.
#STDLIB = -stdlib=libc++  # win32

all: init TARGET

init:
ifndef Container_Type
	$(info [ err] Container_Type not set!)
	$(info [info] e.g )
	$(info [info] $$ Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE')
	$(info [info] $$ make ... Container_Type=$$Container_Type ...)
	$(error error)
endif
	[ -d '$(LINK)' ] || mkdir $(LINK)
	cd $(LOG_SRC); make loglib Container_Type=$(Container_Type)

TARGET: $(LINK)/logworkers.o

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(LINK)

