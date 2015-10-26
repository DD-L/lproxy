CC = gcc
XX = g++
RM = rm -rf

Container_Type = -DLOG_PRIORITY_QUEUE
#Container_Type = -DLOG_LOCKFREE_QUEUE

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
ROOT = $(PWD)../../

BOOST_BUILT = $(ROOT)/boost_built
BOOST_LINK_DIR = $(BOOST_BUILT)/link

EXCEPT_DIR = $(ROOT)/except
EXCEPT_LINK_DIR = $(EXCEPT_DIR)/link

LOG_DIR = $(ROOT)/log
LOG_LINK_DIR = $(LOG_DIR)/link

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(PWD)/../../../../contrib/boost/boost_1_57_0/ -I$(ROOT) 

LIBS = $(LIBPATH) -lpthread -lrt

BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuilt.o

EXCEPTLINK = $(EXCEPT_LINK_DIR)/except.o

LOGLINK = $(LOG_LINK_DIR)/loglevel.o \
		  $(LOG_LINK_DIR)/log_types.o \
		  $(LOG_LINK_DIR)/logging.o

ALL_LINK = $(BOOSTLINK) $(EXCEPTLINK) $(LOGLINK)

TESTQUEUE = $(BIN)/test_priority_queue
TESTLOGEX = $(BIN)/test_logval_extra
TESTLOCKFREE = $(BIN)/test_lockfree
TESTLOGLEVEL = $(BIN)/test_loglevel
TESTLOGMANAGER = $(BIN)/test_logmanager



TestQueue: init $(TESTQUEUE)
TestLogEx: init $(TESTLOGEX)
TestLockfree: init $(TESTLOCKFREE)
TestLogLevel: init $(TESTLOGLEVEL)
TestLogManager: init $(TESTLOGMANAGER)

TARGET = $(TESTQUEUE) \
		 $(TESTLOGEX) \
		 $(TESTLOCKFREE) \
		 $(TESTLOGLEVEL) \
		 $(TESTLOGMANAGER)

all: init  $(TARGET)


init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(LOG_DIR); make loglink Container_Type=$(Container_Type)

$(BIN)/%: $(TMP)/%.o  $(ALL_LINK) 
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

