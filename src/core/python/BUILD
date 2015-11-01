1. Build boost.python link

	target:
		link/pyprebuild.o
		link/pyinit.o

	command:
		e.g.
		$ make PYTHON_INCLUDE=/usr/include/python2.7
	
	tips:
		PYTHON_INCLUDE is the path to python header files, Python.h ... 

2. Build test
	
	target:
		./test/bin/demo.static
		./test/bin/demo

	commad:
		e.g.
		$ cd ./test
		$ make PYTHON_INCLUDE=/usr/include/python2.7 PYTHON_LIB_PATH=/usr/lib PYTHON_LIB=-lpython2.7
	tips:
		PYTHON_INCLUDE is the path to python header files, Python.h ...
		PYTHON_LIB_PATH is the path to python library files, libpython2.7.so libpython2.7.a ...
		                default path: /usr/lib
		PYTHON_LIB is '-l' option of gcc, -lpython2.7, -lpython2.7.dll ...
		                please using -lpython2.7.dll on Cygwin.

3. Run test
	
	command:
		$ cd ./test
		$ ./bin/demo.static
		$ ./bin/demo
	
	tips:
		the working directory of 'demo / demo.static' must be './test'



