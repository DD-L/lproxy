RM=rm -rf
PWD=$(shell pwd)
TOOL_PACKAGE=p7zip_9.38.1_src_all.tar.bz2
UNPACK=tar jxvf
TOOL_SRC=./p7zip_9.38.1
MAKE_CMD=make 7zr
BIN=./bin

all: make_and_install

make_and_install:
	@echo -e "\ninfo: making 7zr"
	make clean
	$(UNPACK) $(TOOL_PACKAGE)
#	previous
	@echo -e "\nFirst of all, fix a manifest error in 7z source code, or it may affect its compilation"
	@echo "file: $(PWD)/$(TOOL_SRC)/CPP/Common/StringConvert.cpp:181:"
	@echo -e "\tUString &srcString = src;       /*original code*/"
	@echo -e "\t--->"
	@echo -e "\tconst UString &srcString = src; /*modified code*/"
	#
	# 先修正7z源码一处错误（否则在某些主机上编译可能会失败）:
	# file: $(PWD)/$(TOOL_SRC)/CPP/Common/StringConvert.cpp:181:
	# 	UString &srcString = src;
	# 看了下源码，它的本意应该是
	# 	const UString &srcString = src;
	# 这确实是一个低级错误
	#
	sed -i '181c const UString &srcString = src; /*(modified by Deel)before modifying: UString &srcString = src;*/' $(TOOL_SRC)/CPP/Common/StringConvert.cpp

#	make
	cd $(TOOL_SRC); $(MAKE_CMD)
	
	
#	install
	@echo -e "\ninfo: install 7zr to $(PWD)/bin/"
	mkdir -p $(BIN)
	cp -a $(TOOL_SRC)/bin/7zr $(BIN)/
#	clean
	@echo -e "\ninfo: clean tmp"
	make clean
#	check
	@echo -e "\ninfo: check env"
	make check
	
	@echo -e "\ninfo: 7zr installation has been completed."
	@echo -e "info: run 'make uninstall' can uninstall 7zr. current path=$(PWD)\n"

clean:
	$(RM) $(TOOL_SRC)

check:
	dos2unix ./check_env.sh > /dev/null 2>&1
	sh ./check_env.sh

uninstall:
	$(RM) $(BIN)


