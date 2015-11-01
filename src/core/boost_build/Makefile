CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
LINK = ./link

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x # win32: -std=std=c++11


INCLUDE = -I$(PWD)/../../../contrib/boost/boost_1_57_0/
#STDLIB = -stdlib=libc++  # win32
LIBS = $(LIBPATH) -lpthread -lrt

BOOSTTHREAD = $(LINK)/sysprebuild.o  $(LINK)/tprebuild.o 
BOOSTDATETIME = $(LINK)/dateprebuild.o

all: init TARGET

init:
	[ -d '$(LINK)' ] || mkdir $(LINK)

TARGET: $(BOOSTTHREAD) $(BOOSTDATETIME)

$(LINK)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(LINK)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(LINK)

