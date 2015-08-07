CC = gcc
XX = g++
RM = rm -rf

BIN = ./bin
TMP = ./~tmp


MACRO_1 = -D_STD_QUEUE
MACRO_2 = -D_STD_PRIORITY_QUEUE
MACRO_3 = -D_BOOST_LOCKFREE_QUEUE
MACRO_4 = -D_BOOST_LOCKFREE_STACK

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
INCLUDE = -I../../../../contrib/boost_1_57_0/ -I../
#STDLIB = -stdlib=libc++  # win32

BOOSTTHREAD = sysprebuild.o tprebuild.o

LIBS = $(LIBPATH) -lpthread -lrt

SRC = test.cpp
TARGET = test1 test2 test3 test4

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)

test1: test_std_queue.o $(BOOSTTHREAD)
	cd $(TMP); $(XX) -o ../$(BIN)/$@ $^ $(LIBS)
test2: test_std_priority_queue.o $(BOOSTTHREAD)
	cd $(TMP); $(XX) -o ../$(BIN)/$@ $^ $(LIBS)
test3: test_boost_lookfree_queue.o $(BOOSTTHREAD)
	cd $(TMP); $(XX) -o ../$(BIN)/$@ $^ $(LIBS)
test4: test_boost_lookfree_stack.o $(BOOSTTHREAD)
	cd $(TMP); $(XX) -o ../$(BIN)/$@ $^ $(LIBS)


test_std_queue.o: $(SRC) 
	$(XX) $(CFLAGS) $(MACRO_1) $(STDLIB) $(INCLUDE) -c $< -o $(TMP)/$@
test_std_priority_queue.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_2) $(STDLIB) $(INCLUDE) -c $< -o $(TMP)/$@
test_boost_lookfree_queue.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_3) $(STDLIB) $(INCLUDE) -c $< -o $(TMP)/$@
test_boost_lookfree_stack.o: $(SRC)
	$(XX) $(CFLAGS) $(MACRO_4) $(STDLIB) $(INCLUDE) -c $< -o $(TMP)/$@


%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(TMP)/$@
%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $(TMP)/$@


clean:
	$(RM) $(TMP) $(BIN)

