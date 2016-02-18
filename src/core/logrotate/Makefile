# 须保证python库完整安装 如:sudo apt-get install python-dev 
RM = rm -rf
GDB := -g

PWD = $(shell pwd)
ROOT = $(PWD)/../
LINK = ./link

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall -D_REENTRANT $(GDB) -std=gnu++0x   # win32: -std=c++11

DEEL_BOOST_PYTHON=$(ROOT)/deel.boost.python/../
PYTHON_BUILD = $(ROOT)/python

override INCPATH  += -I$(ROOT)/../../contrib/boost/boost_1_57_0/ \
		  			-I$(DEEL_BOOST_PYTHON) -I$(PYTHON_INCLUDE) \
		  			-I$(PYTHON_BUILD)
#override STDLIB += -stdlib=libc++  # win32


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
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@


clean:
	$(RM) $(LINK)

