
.PHONY : all
.PHONY : init
.PHONY : init.force
.PHONY : check
.PHONY : boost
.PHONY : boost.force
.PHONY : cryptopp
.PHONY : clean


MAKEFILE = Makefile
#MAKEFILE = Makefile.win32

CHECK_CPP11_DIR = ./tools/cpp11_check

all:
	@echo "do nothing"

init: check boost cryptopp
init.force: check boost.force cryptopp

check:
	@test `command -v dos2unix` || ( echo -e "Error: 'dos2unix' is missing! please install 'dos2unix' and try again.\n\te.g. sudo apt-get/yum install dos2unix " && dos2unix )
	cd $(CHECK_CPP11_DIR); make

boost:
	cd contrib/boost; make boost -f $(MAKEFILE)
boost.force:
	cd contrib/boost; make boost.force

cryptopp:
	cd contrib/cryptopp; make

clean:
	cd contrib/boost; make clean -f $(MAKEFILE)
	cd contrib/cryptopp; make clean

