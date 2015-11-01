# 须保证python库完整安装 如:sudo apt-get install python-dev 
ifndef PYTHON_LIB_PATH
$(info [info] PYTHON_LIB_PATH not set !)
$(info [info] default: PYTHON_LIB_PATH=/usr/lib)
PYTHON_LIB_PATH = /usr/lib
endif

CC = gcc
XX = g++
RM = rm -rf

TMP = ./~tmp
BIN = ./bin


PWD = $(shell pwd)/
ROOT = $(PWD)/../../


PYTHON_BUILD = $(ROOT)/python
PYTHON_LINK_DIR = $(PYTHON_BUILD)/link
PYGETFUNC_LIB_DIR = $(PYTHON_BUILD)/lib

LOGROTATE = $(ROOT)/logrotate
LOGROTATE_LINK_DIR = $(LOGROTATE)/link


DEEL_BOOST_PYTHON=$(ROOT)/deel.boost.python/../


CFLAGS =  -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=c++11
LIBPATH += -L$(PYTHON_LIB_PATH)


LIBS = $(LIBPATH) $(PYTHON_LIB) -lpthread -lrt -ldl -lutil -lz	

INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ \
		  -I$(PYTHON_BUILD) -I$(DEEL_BOOST_PYTHON) \
		  -I$(PYTHON_INCLUDE) -I$(LOGROTATE) -I.
#STDLIB = -stdlib=libc++  # win32


PYTHON_LINK = $(PYTHON_LINK_DIR)/pyprebuild.o $(PYTHON_LINK_DIR)/pyinit.o
LOGROTATE_LINK = $(LOGROTATE_LINK_DIR)/logrotate.o 
PYGETFUNC_LIB = $(PYGETFUNC_LIB_DIR)/libpyfunc.a

#ALL_LINK = $(PYTHON_LINK) $(LOGROTATE_LINK)
ALL_LINK =  $(LOGROTATE_LINK) $(PYGETFUNC_LIB)

TARGET = $(BIN)/demo.static $(BIN)/demo

all: init $(TARGET)
	cp ../logrotate.py $(BIN)/
	cp ./1.txt $(BIN)/

init:
ifndef PYTHON_INCLUDE
	$(info [ err] PYTHON_INCLUDE not set !)
	$(info [info] e.g. $$ make ... PYTHON_INCLUDE=/usr/include/python2.7 ...)
	$(error error)
endif
ifndef PYTHON_LIB
	$(info [ err] PYTHON_LIB not set !)
	$(info [info] e.g. $$ make ... PYTHON_LIB=-lpython2.7 ...)
	$(error error)
endif
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
ifdef REBUILD
	cd $(PYTHON_BUILD); make PYTHON_LIB_PATH=$(PYTHON_LIB_PATH)
	cd $(LOGROTATE); make PYTHON_LIB_PATH=$(PYTHON_LIB_PATH)
else
	cd $(PYTHON_BUILD); make clean; make PYTHON_LIB_PATH=$(PYTHON_LIB_PATH)
	cd $(LOGROTATE); make clean; make PYTHON_LIB_PATH=$(PYTHON_LIB_PATH)
endif


$(BIN)/demo.static: $(TMP)/demo.o $(ALL_LINK)
	$(XX) $^ -o $@	-static  $(LIBS)

$(BIN)/demo: $(TMP)/demo.o $(ALL_LINK)
	$(XX) $^ -o $@	$(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)


