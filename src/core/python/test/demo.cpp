
#include "getpyfunc.h"
#include <iostream>

int main(int argc, char* argv[]) {
	/*
	// 注意文件路径
	const std::string filename = "./getinstance.py";

	std::cout << argv[0] << std::endl; 


	GetPyFunc func(filename.c_str(), "get_instance");
	object ret = func("01", "config.xml");
	std::cout << extract<char*>(ret["curl"]) << std::endl;

	GetPyFunc func3(func);
	ret = func3("01");
	std::cout << extract<char*>(ret["regex"]) << std::endl;

	GetPyFunc func2(filename.c_str(), "get_instance");
	object ret2 = func2("01", "config.xml");
	std::cout << extract<char*>(ret2["name"]) << std::endl;
*/

	GetPyFunc foo("./test.py", "foo");
	{
		boost::python::object ret = foo(1);
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	{
		boost::python::object ret = foo(2, "qwerty");
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	{
		boost::python::object ret = foo();
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	
    return 0;
}

