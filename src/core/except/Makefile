CC = gcc
XX = g++
RM = rm -rf

PWD = $(shell pwd)
TMP = ./link
ROOT = $(PWD)/../

CFLAGS = -Wall -g -std=gnu++0x # win32: -std=std=c++11
INCLUDE = -I$(ROOT) 



all: init TARGET

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)

TARGET: $(TMP)/except.o

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(TMP)

