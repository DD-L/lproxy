.PHONY : all
.PHONY : init
.PHONY : init.force
.PHONY : check
.PHONY : boost
.PHONY : boost.force
.PHONY : cryptopp
.PHONY : lss
.PHONY : lss.cygwin
.PHONY : lss.clean
.PHONY : install
.PHONY : uninstall
.PHONY : clean

BIN = ./bin
RM  = rm -rf
CP  = cp -a

BOOST_MAKEFILE = Makefile
#BOOST_MAKEFILE = Makefile.win32

CHECK_CPP11_DIR = ./tools/cpp11_check

all:
	@echo "do nothing"

init: check boost cryptopp
init.force: check boost.force cryptopp

check:
	@test `command -v dos2unix` || ( echo -e "Error: 'dos2unix' is missing! please install 'dos2unix' and try again.\n\te.g. sudo apt-get/yum install dos2unix " && dos2unix )
	cd $(CHECK_CPP11_DIR); make

boost:
	cd contrib/boost; make boost -f $(BOOST_MAKEFILE)
boost.force:
	cd contrib/boost; make boost.force

cryptopp:
	cd contrib/cryptopp; make

lss:
	cd src/core/lss; make -j 4

lss.cygwin:
	cd src/core/lss; make -f Makefile.Cygwin -j 4

lss.clean:
	cd src/core/lss; make clean

install:
	[ -d '$(BIN)' ] || mkdir $(BIN)
	$(CP) src/core/lss/bin/lsslocal.exe $(BIN)/lsslocal.exe
	$(CP) src/core/lss/bin/lssserver.exe $(BIN)/lssserver.exe
	$(CP) src/core/lss/bin/lkeep.exe $(BIN)/lkeep.exe
	$(CP) src/core/lss/local-config.json $(BIN)/local-config.json
	$(CP) src/core/lss/server-config.json $(BIN)/server-config.json

uninstall:
	$(RM) $(BIN)

clean:
	cd contrib/boost; make clean -f $(BOOST_MAKEFILE)
	cd contrib/cryptopp; make clean

