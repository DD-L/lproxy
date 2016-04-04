#include <language/lang.h>
#include <language/langEN.hpp>
#include <language/langCN.hpp>

#include "test.h"
#include <iostream>
#include <cstring>
using namespace std;
int main() {
	{
	Lang::setLang(new LangCN());
	std::cout << Lang("hehe") << std::endl;
	std::cout << (Lang("hehe") + "llo") << std::endl;
	std::cout << (Lang("hehe") + string("llo")) << std::endl;
	std::cout << (Lang("hehe") + Lang("hello")) << std::endl;

	test();
	std::cout << Lang("hehe") << std::endl;
	std::cout << Lang("hello") << std::endl;

	Lang::setLang(new LangCN());
	std::cout << Lang("hehe") << std::endl;
	}
	return 0;
}
