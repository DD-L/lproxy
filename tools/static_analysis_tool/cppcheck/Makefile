# 
# make            # or make make_and_install_cppcheck
# make check      # check env
# make clean      # clean src
# make uninstall  # uninstall cppcheck
#

RM = rm -rf
PWD = $(shell pwd)
TOOL_PACKAGE = cppcheck-1.69.tar.bz2
UNPACK = tar jxvf
TOOL_SRC = ./cppcheck-1.69/
MAKE_CMD = make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
BIN = ./bin/

all: make_and_install_cppcheck

make_and_install_cppcheck:
#	make
	@echo -e "\ninfo: making cppcheck"
	make clean
	$(UNPACK) $(TOOL_PACKAGE)
	cd $(TOOL_SRC); $(MAKE_CMD)
#	install
	@echo -e "\ninfo: install cppcheck to $(PWD)/bin/"
	mkdir -p $(BIN)/cfg
	cp -a $(TOOL_SRC)/cppcheck $(BIN)
	cp -a $(TOOL_SRC)/htmlreport/cppcheck-htmlreport $(BIN)
	cp -a $(TOOL_SRC)/cfg/std.cfg $(BIN)/cfg
#	clean
	@echo -e "\ninfo: clean tmp"
	make clean

#	check
	@echo -e "\ninfo: check env"
	make check

	@echo -e "\ninof: cppcheck installation has been completed."
	@echo -e "info: run 'make uninstall' can uninstall cppcheck. current path=$(PWD)\n"

clean:
	$(RM) $(TOOL_SRC)

check:
	dos2unix ./check_env.sh > /dev/null 2>&1
	sh ./check_env.sh

uninstall:
	$(RM) $(BIN)



