RM = rm -rf
GDB := -g

PWD = $(shell pwd)
LINK = ./link

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall -D_REENTRANT $(GDB) -std=gnu++0x #-std=std=c++11


override INCPATH  += -I$(PWD)/../../../contrib/boost/boost_1_57_0/
#STDLIB = -stdlib=libc++  # win32
override LIBS     += $(LIBPATH) -lpthread -lrt

BOOSTTHREAD = $(LINK)/sysprebuild.o  $(LINK)/tprebuild.o 
BOOSTDATETIME = $(LINK)/dateprebuild.o

all: init TARGET

init:
	[ -d '$(LINK)' ] || mkdir $(LINK)

TARGET: $(BOOSTTHREAD) $(BOOSTDATETIME)

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@


clean:
	$(RM) $(LINK)

