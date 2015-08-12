CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
BOOST_BUILT = $(PWD)/../../boost_built
BOOST_LINK = $(BOOST_BUILT)/link


CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
#STDLIB = -stdlib=libc++  # win32
INCLUDE = -I$(PWD)/../../../../contrib/boost_1_57_0/ -I$(PWD)../../store/ -I$(PWD)../

BOOSTTHREAD = $(BOOST_LINK)/sysprebuild.o \
			  $(BOOST_LINK)/tprebuild.o\
			  $(BOOST_LINK)/dateprebuilt.o


LIBS = $(LIBPATH) -lpthread -lrt

SRC = test_priority_queue.cpp
TARGET = $(BIN)/test_priority_queue

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(BOOST_BUILT); make -j

$(BIN)/%: $(TMP)/%.o $(BOOSTTHREAD)
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

