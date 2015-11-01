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

CFLAGS =  -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=c++11


DEEL_BOOST_PYTHON=$(ROOT)/deel.boost.python/../



LIBPATH += -L$(PYTHON_LIB_PATH)


LIBS = $(LIBPATH) $(PYTHON_LIB) -lpthread -lrt -ldl -lutil -lz  

INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ \
		  -I$(PYTHON_BUILD) -I$(DEEL_BOOST_PYTHON) -I$(PYTHON_INCLUDE) -I.
#STDLIB = -stdlib=libc++  # win32


PYTHON_LINK = $(PYTHON_LINK_DIR)/pyprebuild.o $(PYTHON_LINK_DIR)/pyinit.o
PYGETFUNC_LIB = $(PYGETFUNC_LIB_DIR)/libpyfunc.a

TARGET = $(BIN)/demo.static $(BIN)/demo

all: init $(TARGET)

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
	cd $(PYTHON_BUILD); make clean; make PYTHON_LIB_PATH=$(PYTHON_LIB_PATH)

$(BIN)/demo.static: $(TMP)/demo.o $(PYGETFUNC_LIB)
	$(XX) $^ -o $@  -static  $(LIBS)

$(BIN)/demo: $(TMP)/demo.o $(PYGETFUNC_LIB)
	$(XX) $^ -o $@  $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

