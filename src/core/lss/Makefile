RM = rm -rf
GDB := -g

TMP = ./~tmp
BIN = ./bin

MACROS = -D_REENTRANT -DBOOST_REGEX_NO_LIB \
		 -DBOOST_DATE_TIME_SOURCE -DBOOST_SYSTEM_NO_LIB \
		 $(Log_Container_Type) $(LSS_DEBUG) $(ASIO_DEBUG)

override CFLAGS   += -Wall $(GDB)
override CXXFLAGS += -Wall $(MACROS) $(GDB) -std=c++1y
override LIBS     += $(LIBPATH) -lpthread -lrt

override INCPATH  +=  -I$(CORE_DIR) -I$(BOOST) -I$(CRYPTOPP) -I. 


include ./Makefile.COMMON

