CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
BOOST_BUILT = $(PWD)/../../boost_built
BOOST_LINK_DIR = $(BOOST_BUILT)/link

EXCEPT_DIR = $(PWD)/../../except
EXCEPT_LINK_DIR = $(EXCEPT_DIR)/link

ROOT = $(PWD)../../

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(PWD)/../../../../contrib/boost/boost_1_57_0/ -I$(ROOT) 

LIBS = $(LIBPATH) -lpthread -lrt

BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuilt.o

EXCEPTLINK = $(EXCEPT_LINK_DIR)/except.o

ALL_LINK = $(BOOSTLINK) $(EXCEPTLINK)

TESTQUEUE = $(BIN)/test_priority_queue
TESTLOGEX = $(BIN)/test_logval_extra
TESTLOCKFREE = $(BIN)/test_lockfree
TESTLOGLEVEL = $(BIN)/test_loglevel


TestQueue: init $(TESTQUEUE)
TestLogEx: init $(TESTLOGEX)
TestLockfree: init $(TESTLOCKFREE)
TestLogLevel: init $(TESTLOGLEVEL)

TARGET = $(TESTQUEUE) \
		 $(TESTLOGEX) \
		 $(TESTLOCKFREE) \
		 $(TESTLOGLEVEL)

all: init $(TARGET)


init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(BOOST_BUILT); make -j
	cd $(EXCEPT_DIR); make -j

$(BIN)/%: $(TMP)/%.o $(ALL_LINK) 
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

