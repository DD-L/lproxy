CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
TMP = ./link

CFLAGS = -Wall -D_REENTRANT -g -std=gnu++0x   # win32: -std=std=c++11
INCLUDE = -I$(PWD)/../../../contrib/boost_1_57_0/
#STDLIB = -stdlib=libc++  # win32
LIBS = $(LIBPATH) -lpthread -lrt

BOOSTTHREAD = $(TMP)/sysprebuild.o  $(TMP)/tprebuild.o

all: init TARGET

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)

TARGET: $(BOOSTTHREAD)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@


clean:
	$(RM) $(TMP)

