#include <iostream>
#include "store.h"

using namespace std;


void test() {
	
	typedef Store<int> Int_Store;
	const Int_Store& ca = Int_Store::get_const_instance();
	Int_Store& ma = Int_Store::get_mutable_instance();

	cout << ca.size() << endl; // 0
	ma.push(100);
	ma.push(200);
	ma.push(300);
	cout << ma.size() << endl; // 2
	//Int_Store::resize(1);
	//ma.push(300);
	//cout << ma.size() << endl; // 3

	int b;
	ma.pop(b);
	cout << ca.size() << endl; // 2
	cout << b << endl; // 100
	int c;
	ma.pop(&c);
	cout << ca.size() << endl; // 1
	cout << c << endl; // 200
	ma.pop(c);
	cout << ca.size() << endl;
	cout << c << endl; // 300
	cout << boolalpha << ca.empty() << endl; // ture


}

#include <queue>
int main() {
	test();
	return 0;
}


