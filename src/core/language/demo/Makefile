RM = rm -rf
GDB := -g

PWD := $(shell pwd)
TMP = ./~tmp
BIN = ./bin

LANGSRC = $(PWD)/..
SRCDIR = $(PWD)/../..

LANGLINK = $(LANGSRC)/link/lang.o

override CFLAGS   += -Wall $(GDB) -I$(SRCDIR)
override CXXFLAGS += -Wall $(GDB) -I$(SRCDIR) -std=c++1y

TARGET = ./bin/demo

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(LANGSRC); make

$(TARGET): $(TMP)/main.o $(TMP)/test.o $(LANGLINK)
	$(CXX) -o $@ $^ $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@

clean:
	$(RM) $(TMP) $(BIN)
