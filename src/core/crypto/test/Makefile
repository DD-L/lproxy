CC = gcc
XX = g++
RM = rm -rf
AR = ar

TMP = ./~tmp
BIN = ./bin

PWD = $(shell pwd)
ROOT = $(PWD)/../../

CRYPTO_SRC = $(ROOT)/crypto

CRYPTOPP = $(ROOT)/../../contrib/cryptopp
CRYPTOPP_SRC = $(CRYPTOPP)/cryptopp

CFLAGS = -Wall -g
CPPFLAGS = -Wall -g -std=gnu++0x   # win32: -std=std=c++11
INCLUDE =  -I$(ROOT) -I$(CRYPTOPP) -I. 

#LIBS = $(LIBPATH) -lpthread -lrt


CRYPTO_STATIC_LIB = $(CRYPTO_SRC)/lib/libcrypto.a
CRYPTO_DYNAMIC_LIB = $(CRYPTO_SRC)/lib/libcrypto.so

CRYPTOPP_STATIC_LIB = $(CRYPTOPP_SRC)/libcryptopp.a

LIBEXIST = $(shell if [ -f $(CRYPTOPP_STATIC_LIB) ]; then echo "exist"; else echo "notexist"; fi;)


all: init $(BIN)/test $(BIN)/test.static

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(CRYPTO_SRC); make 
ifneq ('$(LIBEXIST)', 'exist')
	cd $(CRYPTOPP_SRC); make libcryptopp.a -j 4
endif

$(BIN)/test: $(TMP)/test.o $(CRYPTO_STATIC_LIB) $(CRYPTOPP_STATIC_LIB) 
	$(XX) $^ -o $@  $(LIBS)

$(BIN)/test.static: $(TMP)/test.o $(CRYPTO_STATIC_LIB) $(CRYPTOPP_STATIC_LIB)
	$(XX) $^ -o $@ -static $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CPPFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(TMP) $(CRYPTOLIB)
