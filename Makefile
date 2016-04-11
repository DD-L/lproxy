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
.PHONY : boost_build.clean
.PHONY : except.clean
.PHONY : language.clean
.PHONY : store.clean
.PHONY : crypto.clean
.PHONY : log.clean
.PHONY : python.clean
.PHONY : logrotate.clean
.PHONY : src.clean
.PHONY : contrib.clean
.PHONY : all.clean
.PHONY : clean

BIN  := ./bin
RM   := rm -rf
CP   := cp -a
MAKE := make

BOOST_MAKEFILE = Makefile
#BOOST_MAKEFILE = Makefile.win32

CHECK_CPP11_DIR = ./tools/cpp11_check

all:
	@echo "do nothing"

init: check boost cryptopp
init.force: check boost.force cryptopp

check:
	@test `command -v dos2unix` || ( echo -e "Error: 'dos2unix' is missing! please install 'dos2unix' and try again.\n\te.g. sudo apt-get/yum install dos2unix " && dos2unix )
	cd $(CHECK_CPP11_DIR); $(MAKE)

boost:
	cd contrib/boost; $(MAKE) boost -f $(BOOST_MAKEFILE)
boost.force:
	cd contrib/boost; $(MAKE) boost.force

cryptopp:
	cd contrib/cryptopp; $(MAKE)

lss:
	cd src/core/lss; $(MAKE)

lss.cygwin:
	cd src/core/lss; $(MAKE) -f Makefile.Cygwin

lss.clean:
	cd src/core/lss; $(MAKE) clean

install:
	[ -d '$(BIN)' ] || mkdir $(BIN)
	$(CP) src/core/lss/bin/lsslocal.exe $(BIN)/lsslocal.exe
	$(CP) src/core/lss/bin/lssserver.exe $(BIN)/lssserver.exe
	$(CP) src/core/lss/bin/lkeep.exe $(BIN)/lkeep.exe
	$(CP) src/core/lss/local-config.json $(BIN)/local-config.json
	$(CP) src/core/lss/server-config.json $(BIN)/server-config.json

uninstall:
	$(RM) $(BIN)

boost_build.clean:
	cd src/core/boost_build; $(MAKE) clean

except.clean:
	cd src/core/except; $(MAKE) clean

language.clean:
	cd src/core/language; $(MAKE) clean

store.clean:
	cd src/core/store; $(MAKE) clean

crypto.clean:
	cd src/core/crypto; $(MAKE) clean

log.clean:
	cd src/core/log; $(MAKE) clean

python.clean:
	cd src/core/python; $(MAKE) clean

logrotate.clean:
	cd src/core/logrotate; $(MAKE) clean

src.clean: boost_build.clean except.clean language.clean store.clean crypto.clean log.clean python.clean logrotate.clean lss.clean

contrib.clean:
	cd contrib/boost; $(MAKE) clean -f $(BOOST_MAKEFILE)
	cd contrib/cryptopp; $(MAKE) clean

all.clean: src.clean contrib.clean

clean:
	@echo "do nothing"
