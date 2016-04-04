#include <language/lang.h>
#include <language/langEN.hpp>
#include <iostream>
using namespace std;

void test() {
	cout << "=====test=======" << endl;
	Lang::setLang(new LangEN());
	std::cout << Lang("hehe") << std::endl;
	cout << "====end test====" << endl;
}
