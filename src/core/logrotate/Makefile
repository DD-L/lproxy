# 须保证python库完整安装 如:sudo apt-get install python-dev 
CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
ROOT = $(PWD)/../
LINK = ./link

CFLAGS =  -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=c++11

DEEL_BOOST_PYTHON=$(ROOT)/deel.boost.python/../
PYTHON_BUILD = $(ROOT)/python

INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ \
		  -I$(DEEL_BOOST_PYTHON) -I$(PYTHON_INCLUDE) \
		  -I$(PYTHON_BUILD)
#STDLIB = -stdlib=libc++  # win32


LOGRATATE = $(LINK)/logrotate.o 

all: init TARGET

init:
ifndef PYTHON_INCLUDE
	$(info PYTHON_INCLUDE not set !)
	$(info e.g. $$ make ... PYTHON_INCLUDE=/usr/include/python2.7 ...)
	$(error error)
endif
	[ -d '$(LINK)' ] || mkdir $(LINK)

TARGET: $(LOGRATATE)

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(LINK)

