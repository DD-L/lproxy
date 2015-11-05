CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
ROOT = $(PWD)/../../
TMP = ./~tmp
BIN = ./bin

WORKERS_SRC = $(ROOT)/workers
LOGROTATE_SRC = $(ROOT)/logrotate
PYTHON_SRC = $(ROOT)/python
EXCEPT_SRC = $(ROOT)/except
LOG_SRC = $(ROOT)/log
LOG_LIB_DIR = $(LOG_SRC)/lib

CFLAGS =  -Wall -D_REENTRANT $(Container_Type) -g -std=gnu++0x   # win32: -std=c++11


INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ -I$(ROOT)  
#STDLIB = -stdlib=libc++  # win32

BOOST_LINK_DIR = $(ROOT)/boost_build/link
BOOST_LINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			 $(BOOST_LINK_DIR)/dateprebuild.o \
			 $(BOOST_LINK_DIR)/tprebuild.o

ALL_LINK = $(WORKERS_SRC)/link/logworkers.o \
		   $(LOGROTATE_SRC)/link/logrotate.o \
		   $(PYTHON_SRC)/lib/libpyfunc.a \
		   $(EXCEPT_SRC)/link/except.o \
		   $(BOOST_LINK)

LIBPATH = -L$(LOG_LIB_DIR) -L$(PYTHON_SRC)/lib
LIBS += $(LIBPATH)  -llog $(PYTHON_LIB) -lpthread -lrt -ldl -lutil -lz

all: init TARGET

init:
ifndef Container_Type
	$(info [ err] Container_Type not set!)
	$(info [info] e.g )
	$(info [info] $$ Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE')
	$(info [info] $$ make ... Container_Type=$$Container_Type ...)
	$(error error)
endif
ifndef PYTHON_INCLUDE
	$(info PYTHON_INCLUDE not set !)
	$(info e.g. $$ make ... PYTHON_INCLUDE=/usr/include/python2.7 ...)
	$(error error)
endif
ifndef PYTHON_LIB
	$(info [ err] PYTHON_LIB not set !)
	$(info [info] e.g. $$ make ... PYTHON_LIB=-lpython2.7 ...)
	$(error error)
endif
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(WORKERS_SRC); make Container_Type=$(Container_Type) USE_LOCK=-DLOG_USE_LOCK
	cd $(LOGROTATE_SRC); make PYTHON_INCLUDE=$(PYTHON_INCLUDE) 


TARGET: $(BIN)/test_logworkers $(BIN)/test_logworkers.static

$(BIN)/test_logworkers: $(TMP)/test_logworkers.o $(ALL_LINK)
	$(XX) $^ -o $@  $(LIBS)
$(BIN)/test_logworkers.static: $(TMP)/test_logworkers.o $(ALL_LINK)
	$(XX) $^ -o $@ -static $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)
	cd $(LOG_SRC); make clean

