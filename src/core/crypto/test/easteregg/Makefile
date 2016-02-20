RM = rm -rf
GDB := -g

TMP = ./~tmp
BIN = ./bin

PWD = $(shell pwd)
ROOT = $(PWD)/../../../

CRYPTO_SRC = $(ROOT)/crypto

CRYPTOPP = $(ROOT)/../../contrib/cryptopp
CRYPTOPP_SRC = $(CRYPTOPP)/cryptopp

BOOST = $(ROOT)/../../contrib/boost/boost_1_57_0

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall $(GDB) -std=gnu++0x   # win32: -std=std=c++11
override INCPATH  +=  -I$(ROOT) -I$(CRYPTOPP) -I$(BOOST) -I. 

#override LIBS += $(LIBPATH) -lpthread -lrt


CRYPTO_STATIC_LIB = $(CRYPTO_SRC)/lib/libcrypto.a
CRYPTO_DYNAMIC_LIB = $(CRYPTO_SRC)/lib/libcrypto.so

CRYPTOPP_STATIC_LIB = $(CRYPTOPP_SRC)/libcryptopp.a

LIBEXIST = $(shell if [ -f $(CRYPTOPP_STATIC_LIB) ]; then echo "exist"; else echo "notexist"; fi;)


all: init $(BIN)/mysecret.static

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)
	cd $(CRYPTO_SRC); make 
ifneq ('$(LIBEXIST)', 'exist')
	cd $(CRYPTOPP_SRC); make libcryptopp.a -j 4
endif

#$(BIN)/mysecret: $(TMP)/mysecret.o $(CRYPTO_STATIC_LIB) $(CRYPTOPP_STATIC_LIB) 
#	$(CXX) $^ -o $@  $(LIBS)

$(BIN)/mysecret.static: $(TMP)/mysecret.o $(CRYPTO_STATIC_LIB) $(CRYPTOPP_STATIC_LIB)
	$(CXX) $^ -o $@ -static $(LIBS)

$(TMP)/%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@
$(TMP)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STDLIB) $(INCPATH) -c $< -o $@

clean:
	$(RM) $(TMP) $(CRYPTOLIB)
