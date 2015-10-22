CC = gcc
XX = g++
RM = rm -rf

# Settings Container Type
Container_Type = -DLOG_PRIORITY_QUEUE
#Container_Type = -DLOG_LOCKFREE_QUEUE


LINK = ./link
LOGLIB = ./lib

TESTDIR = ./test
DEMODIR = ./demo

PWD = $(shell pwd)
ROOT = $(PWD)/../
BOOST_BUILT = $(ROOT)/boost_built
BOOST_LINK_DIR = $(BOOST_BUILT)/link

EXCEPT_DIR = $(ROOT)/except
EXCEPT_LINK_DIR = $(EXCEPT_DIR)/link


CFLAGS = -Wall -D_REENTRANT $(Container_Type) -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ -I$(ROOT) -I. 

LIBS = $(LIBPATH) -lpthread -lrt

BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuilt.o

EXCEPTLINK = $(EXCEPT_LINK_DIR)/except.o

LOGLINK = $(LINK)/loglevel.o $(LINK)/log_types.o $(LINK)/logging.o

ALL_LINK = $(BOOSTLINK) $(EXCEPTLINK) $(LOGLINK)

LOG_STATIC_LIB = $(LOGLIB)/liblog.a
LOG_DYNAMIC_LIB = $(LOGLIB)/liblog.so


all: loglib

demo: loglib 
	@echo "Compiling Log-demo ..."
	cd $(DEMODIR); make

test: TestQueue TestLogEx TestLockfree TestLogLevel

#loglib: loglink $(LOG_STATIC_LIB) $(LOG_DYNAMIC_LIB)
loglib: loglink $(LOG_STATIC_LIB)

$(LOG_STATIC_LIB): $(LOGLINK)
	ar crv $@ $^
$(LOG_DYNAMIC_LIB): $(LOGLINK)
	g++ -shared $^ -o $@
	

loglink: init $(LOGLINK)

init:
	[ -d '$(LINK)' ] || mkdir $(LINK)
	[ -d '$(LOGLIB)' ] || mkdir $(LOGLIB)



$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

# 测试用例
TestQueue:
	@echo -e "\nCompiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[log_tools::priority_queue] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

TestLogEx:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[test_logval_extra] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

TestLockfree:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[test_lockfree] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

TestLogLevel:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[test_loglevel] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

clean:
	cd $(TESTDIR); make clean
	cd $(DEMODIR); make clean
	$(RM) $(LINK) $(LOGLIB)
