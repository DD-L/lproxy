RM = rm -rf
GDB := -g

PWD := $(shell pwd)
LINK = ./link
DEMO = ./demo

SRCCOREDIR = $(PWD)/..

override CFLAGS   += -Wall $(GDB) -I$(SRCCOREDIR) 
override CXXFLAGS += -Wall $(GDB) -I$(SRCCOREDIR) -std=c++1y


TARGET = $(LINK)/lang.o


all: init $(TARGET)

init:
	[ -d '$(LINK)' ] || mkdir $(LINK)

#$(TARGET): $(LINK)/lang.o
#	$(CXX) -o $@ $^ $(LIBS)

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@


clean:
	$(RM) $(LINK)
	cd $(DEMO); make clean 
