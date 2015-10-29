CC = gcc
XX = g++
RM = rm -rf

ifndef USE_LOCK
$(info [info] USE_LOCK not set!)
$(info [info] default: USE_LOCK=-DLOG_USE_LOCK)
# 多线程日志输出
USE_LOCK = -DLOG_USE_LOCK
endif

TMP = ./~tmp
BIN = ./bin


PWD = $(shell pwd)/
ROOT = $(PWD)/../../


BOOST_BUILD = $(ROOT)/boost_build
BOOST_LINK_DIR = $(BOOST_BUILD)/link

EXCEPT_DIR = $(ROOT)/except
EXCEPT_LINK_DIR = $(EXCEPT_DIR)/link

LOG_DIR = $(ROOT)/log
LOG_LIB_DIR = $(LOG_DIR)/lib

CFLAGS = -Wall -D_REENTRANT $(Container_Type) $(USE_LOCK) -g -std=gnu++0x   # win32: -std=std=c++11
INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ -I$(ROOT) -I.
LIBPATH = -L$(LOG_LIB_DIR)
LIBS = $(LIBPATH)  -llog -lpthread -lrt


LOG_STATIC_LIB = $(LOG_LIB_DIR)/liblog.a
LOG_DYNAMIC_LIB = $(LOG_LIB_DIR)/liblog.so


BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuild.o

EXCEPTLINK = $(EXCEPT_LINK_DIR)/except.o

DEMOLINK = $(TMP)/test_logoutput.o

ALL_LINK = $(BOOSTLINK) $(EXCEPTLINK) $(DEMOLINK)

#TARGET = $(BIN)/demo.static $(BIN)/demo.dynamic
TARGET = $(BIN)/demo.static


#TestLogLevel:
#	@echo "Compiling $@ ..."
#	cd $(TESTDIR); make $@
#	@echo
#	@echo "[test_loglevel] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
#	@echo

all: init $(TARGET)

init:
ifndef Container_Type
	$(info [ err] Container_Type not set!)
	$(info [info] e.g )
	$(info [info] $$ Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE')
	$(info [info] $$ make ... Container_Type=$$Container_Type ...)
	$(error error)
endif
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(LOG_DIR); make loglib Container_Type=$(Container_Type)

$(BIN)/demo.static: $(TMP)/demo.o $(ALL_LINK)
	$(XX) $^ -o $@  -static  $(LIBS)
$(BIN)/demo.dynamic: $(TMP)/demo.o $(ALL_LINK) 
	$(XX) $^ -o $@  $(LIBS)


$(BIN)/test: $(TMP)/test.o $(TMP)/log.o  $(ALL_LINK)
	$(XX) -o $@ $^ $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(TMP) $(BIN)

