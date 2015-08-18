CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
BOOST_BUILT = $(PWD)/../../boost_built
BOOST_LINK_DIR = $(BOOST_BUILT)/link


CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(PWD)/../../../../contrib/boost_1_57_0/ -I$(PWD)../../store/ -I$(PWD)../

LIBS = $(LIBPATH) -lpthread -lrt

BOOSTLINK = $(BOOST_LINK_DIR)/sysprebuild.o \
			  $(BOOST_LINK_DIR)/tprebuild.o \
			  $(BOOST_LINK_DIR)/dateprebuilt.o

TESTQUEUE = $(BIN)/test_priority_queue
TESTLOGEX = $(BIN)/test_logval_extra
TESTLOCKFREE = $(BIN)/test_lockfree


TestQueue: init $(TESTQUEUE)
TestLogEx: init $(TESTLOGEX)
TestLockfree: init $(TESTLOCKFREE)

TARGET = $(TESTQUEUE) \
		 $(TESTLOGEX) \
		 $(TESTLOCKFREE)

all: init $(TARGET)


init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(BOOST_BUILT); make -j

$(BIN)/%: $(TMP)/%.o $(BOOSTLINK)
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

