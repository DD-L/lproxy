CC = gcc
XX = g++
RM = rm -rf
AR = ar

TMP = ./~tmp
CRYPTOLIB = ./lib

TESTDIR = ./test

PWD = $(shell pwd)
ROOT = $(PWD)/../

CRYPTOPP = $(ROOT)/../../contrib/cryptopp
CRYPTOPP_SRC = $(CRYPTOPP)/cryptopp
SRCEXIST= $(shell if [ -f $(CRYPTOPP_SRC)/aes.h ]; then echo "exist"; else echo "notexist"; fi; )
#LIBEXIST = $(shell if [ -f $(CRYPTOPP_SRC)/libcryptopp.a ]; then echo "exist"; else echo "notexist"; fi;)

CFLAGS = -Wall -g
CPPFLAGS = -Wall -g -std=gnu++0x  
# win32: -std=std=c++11
INCLUDE =  -I$(ROOT) -I$(CRYPTOPP) -I. 

#LIBS = $(LIBPATH) -lpthread -lrt


ALL_LINK = $(TMP)/encryptor.o $(TMP)/xor_crypto.o \
		   $(TMP)/rc4_crypto.o $(TMP)/rc4.o \
		   $(TMP)/aes_crypto.o

CRYPTO_STATIC_LIB = $(CRYPTOLIB)/libcrypto.a
CRYPTO_DYNAMIC_LIB = $(CRYPTOLIB)/libcrypto.so

.PHONY : all
.PHONY : init
.PHONY : test
.PHONY : clean

all: init $(CRYPTO_STATIC_LIB)

test:
	cd $(TESTDIR); make

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(CRYPTOLIB)' ] || mkdir $(CRYPTOLIB)
ifneq ('$(SRCEXIST)', 'exist')
	cd $(CRYPTOPP); make
endif
#ifneq ('$(LIBEXIST)', 'exist')
#	cd $(CRYPTOPP_SRC); make libcryptopp.a -j 4
#endif

$(CRYPTO_STATIC_LIB): $(ALL_LINK)
	$(AR) crv $@ $^

$(CRYPTO_DYNAMIC_LIB): $(ALL_LINK)
	$(XX) -shared $^ -o $@


$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(XX) $(CPPFLAGS) $(STDLIB) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(TMP) $(CRYPTOLIB)
