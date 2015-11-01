CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
BOOST_BUILD = $(PWD)/../../boost_build
BOOST_LINK = $(BOOST_BUILD)/link

MACRO_1 = -D_STD_QUEUE
MACRO_2 = -D_STD_PRIORITY_QUEUE
MACRO_3 = -D_BOOST_LOCKFREE_QUEUE
MACRO_4 = -D_BOOST_LOCKFREE_STACK

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
INCLUDE = -I$(PWD)/../../../../contrib/boost/boost_1_57_0/ -I$(PWD)../
#STDLIB = -stdlib=libc++  # win32

BOOSTTHREAD = $(BOOST_LINK)/sysprebuild.o \
			  $(BOOST_LINK)/tprebuild.o

LIBS = $(LIBPATH) -lpthread -lrt

SRC = test.cpp
TARGET = $(BIN)/test1 $(BIN)/test2 $(BIN)/test3 $(BIN)/test4

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(BOOST_BUILD); make -j

$(BIN)/test1: $(TMP)/test_std_queue.o $(BOOSTTHREAD)
	$(XX) -o $@ $^ $(LIBS)
$(BIN)/test2: $(TMP)/test_std_priority_queue.o $(BOOSTTHREAD)
	$(XX) -o $@ $^ $(LIBS)
$(BIN)/test3: $(TMP)/test_boost_lookfree_queue.o $(BOOSTTHREAD)
	$(XX) -o $@ $^ $(LIBS)
$(BIN)/test4: $(TMP)/test_boost_lookfree_stack.o $(BOOSTTHREAD)
	$(XX) -o $@ $^ $(LIBS)


$(TMP)/test_std_queue.o: $(SRC) 
	$(XX) $(CFLAGS) $(MACRO_1) $(STDLIB) $(INCLUDE) -c $< -o $@
$(TMP)/test_std_priority_queue.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_2) $(STDLIB) $(INCLUDE) -c $< -o $@
$(TMP)/test_boost_lookfree_queue.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_3) $(STDLIB) $(INCLUDE) -c $< -o $@
$(TMP)/test_boost_lookfree_stack.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_4) $(STDLIB) $(INCLUDE) -c $< -o $@


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

