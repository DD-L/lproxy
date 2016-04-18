RM = rm -rf
MAKE = make
.PHONY : all
.PHONY : git_clone
.PHONY : clean
CRYPTOPP_SRC = ./cryptopp
CRYPTOPP_STATIC_LIB = $(CRYPTOPP_SRC)/libcryptopp.a
LIBEXIST = $(shell if [ -f $(CRYPTOPP_STATIC_LIB) ]; then echo "exist"; else echo "notexist"; fi;)

#SRCEXIST= $(shell if [ -d $(CRYPTOPP_SRC) ]; then echo "exist"; else echo "notexist"; fi; )
SRCEXIST= $(shell if [ -f $(CRYPTOPP_SRC)/aes.h ]; then echo "exist"; else echo "notexist"; fi; )

all: git_clone
ifneq ('$(LIBEXIST)', 'exist')
	cd $(CRYPTOPP_SRC); $(MAKE) libcryptopp.a -j 4
	cd $(CRYPTOPP_SRC); $(RM) *.o
endif

git_clone:
ifneq ('$(SRCEXIST)', 'exist')
	#git clone https://github.com/weidai11/cryptopp.git
	git clone https://github.com/DD-L/cryptopp
	# or
	# tar xvfJ ./cryptopp.bak.tar.xz; mv cryptopp.bak cryptopp
endif

clean:
	$(RM)  $(CRYPTOPP_SRC)
