# 
# make            # or make make_and_install_cppcheck
# make check      # check env
# make clean      # clean src
# make uninstall  # uninstall cppcheck
#

RM=rm -rf
PWD=$(shell pwd)
TOOL_PACKAGE=cppcheck-1.69.tar.bz2
UNPACK=tar jxvf
TOOL_SRC=./cppcheck-1.69/
MAKE_CMD=make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
BIN=./bin/
all: make_and_install_cppcheck

make_and_install_cppcheck:
#	make
	@echo -e "\nmaking cppcheck"
	make clean
	$(UNPACK) $(TOOL_PACKAGE)
	cd $(TOOL_SRC); $(MAKE_CMD)
#	install
	@echo -e "\ninstall cppcheck to ./bin/"
	mkdir -p $(BIN)/cfg
	cp -a $(TOOL_SRC)/cppcheck $(BIN)
	cp -a $(TOOL_SRC)/htmlreport/cppcheck-htmlreport $(BIN)
	cp -a $(TOOL_SRC)/cfg/std.cfg $(BIN)/cfg
#	clean
	@echo -e "\nclean tmp"
	make clean

#	check
	@echo -e "\ncheck env"
	make check

	@echo -e "\ncppcheck installation has been completed."
	@echo -e "run 'make uninstall' can uninstall cppcheck. Current path=$(PWD)\n"

clean:
	$(RM) $(TOOL_SRC)

check:
	dos2unix ./check_env.sh > /dev/null 2>&1
	sh ./check_env.sh

uninstall:
	$(RM) $(BIN)



