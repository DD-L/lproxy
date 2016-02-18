
TMP_FILE = tmp
XX = $(CXX) -std=gnu++0x -o $(TMP_FILE)
TEST_FILE = test_cpp11.cpp
RM = rm -rf

all:
	@echo -e "\nchecking: 'C++11' support..."
	$(XX)  $(TEST_FILE)
	make clean
	@echo

clean:
	$(RM) $(TMP_FILE)
