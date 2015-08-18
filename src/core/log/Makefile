
TESTDIR = ./test

all: TestQueue TestLogEx TestLockfree

TestQueue:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[log_tools::priority_queue] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

TestLogEx:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[test_logval_extra] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

TestLockfree:
	@echo "Compiling $@ ..."
	cd $(TESTDIR); make $@
	@echo
	@echo "[test_lockfree] Test cases compiled successfully in '"`pwd`/$(TESTDIR)"/bin/'"
	@echo

clean:
	cd $(TESTDIR); make clean
