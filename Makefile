
MAKEFILE = Makefile
#MAKEFILE = Makefile.win32

CHECK_CPP11_DIR = ./tools/cpp11_check

all:
	@echo "do nothing"

init: check boost
init.force: check boost.force

check:
	cd $(CHECK_CPP11_DIR); make

boost:
	cd contrib; make boost -f $(MAKEFILE)
boost.force:
	cd contrib; make boost.force

clean:
	cd contrib; make clean -f $(MAKEFILE)

