RM = rm -rf
GDB := -g

PWD = $(shell pwd)
TMP = ./~tmp
BIN = ./bin

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall $(GDB) -std=c++1y

TARGET = ./bin/demo

all: init $(TARGET)

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)

$(TARGET): $(TMP)/main.o
	$(CXX) -o $@ $^ $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@

clean:
	$(RM) $(TMP) $(BIN)

