CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp
LINK = ./link

PWD = $(shell pwd)/
BOOST_BUILT = $(PWD)/../boost_built
BOOST_LINK_DIR = $(BOOST_BUILT)/link

EXCEPT_DIR = $(PWD)/../except
EXCEPT_LINK_DIR = $(EXCEPT_DIR)/link

ROOT = $(PWD)../

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(ROOT)/../../contrib/boost/boost_1_57_0/ -I$(ROOT) -I. 

LIBS = $(LIBPATH) -lpthread -lrt

BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuilt.o

EXCEPTLINK = $(EXCEPT_LINK_DIR)/except.o

LOGLINK = $(LINK)/loglevel.o $(LINK)/log_types.o

ALL_LINK = $(BOOSTLINK) $(EXCEPTLINK) $(LOGLINK)


TESTDIR = ./test

all: loglink TestQueue TestLogEx TestLockfree TestLogLevel

loglink: init $(LOGLINK)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	[ -d '$(LINK)' ] || mkdir $(LINK)

$(BIN)/%: $(TMP)/%.o  $(ALL_LINK) 
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

TestQueue:
	@echo "Compiling $@ ..."
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
	$(RM) $(TMP) $(BIN) $(LINK)
