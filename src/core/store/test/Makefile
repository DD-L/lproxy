RM = rm -rf
GDB := -g

BIN = ./bin
TMP = ./~tmp

PWD = $(shell pwd)/
BOOST_BUILD = $(PWD)/../../boost_build
BOOST_LINK = $(BOOST_BUILD)/link

MACRO_1 = -D_STD_QUEUE
MACRO_2 = -D_STD_PRIORITY_QUEUE
MACRO_3 = -D_BOOST_LOCKFREE_QUEUE
MACRO_4 = -D_BOOST_LOCKFREE_STACK

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall -D_REENTRANT $(GDB) -std=gnu++0x   # win32: -std=std=c++11
override INCPATH  += -I$(PWD)/../../../../contrib/boost/boost_1_57_0/ -I$(PWD)../
#STDLIB = -stdlib=libc++  # win32

BOOSTTHREAD = $(BOOST_LINK)/sysprebuild.o \
			  $(BOOST_LINK)/tprebuild.o

override LIBS += $(LIBPATH) -lpthread -lrt

SRC = test.cpp
TARGET = $(BIN)/test1 $(BIN)/test2 $(BIN)/test3 $(BIN)/test4

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(BOOST_BUILD); make -j

$(BIN)/test1: $(TMP)/test_std_queue.o $(BOOSTTHREAD)
	$(CXX) -o $@ $^ $(LIBS)
$(BIN)/test2: $(TMP)/test_std_priority_queue.o $(BOOSTTHREAD)
	$(CXX) -o $@ $^ $(LIBS)
$(BIN)/test3: $(TMP)/test_boost_lookfree_queue.o $(BOOSTTHREAD)
	$(CXX) -o $@ $^ $(LIBS)
$(BIN)/test4: $(TMP)/test_boost_lookfree_stack.o $(BOOSTTHREAD)
	$(CXX) -o $@ $^ $(LIBS)


$(TMP)/test_std_queue.o: $(SRC) 
	$(CXX) $(CXXFLAGS) $(MACRO_1) $(STDLIB) $(INCPATH) -c $< -o $@
$(TMP)/test_std_priority_queue.o: $(SRC)
	$(CXX) $(CXXFLAGS) $(MACRO_2) $(STDLIB) $(INCPATH) -c $< -o $@
$(TMP)/test_boost_lookfree_queue.o: $(SRC)
	$(CXX) $(CXXFLAGS) $(MACRO_3) $(STDLIB) $(INCPATH) -c $< -o $@
$(TMP)/test_boost_lookfree_stack.o: $(SRC)
	$(CXX) $(CXXFLAGS) $(MACRO_4) $(STDLIB) $(INCPATH) -c $< -o $@


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@


clean:
	$(RM) $(TMP) $(BIN)

