#include <iostream>

#include "store2.h"

void foo() {
	typedef Store<int> IntStore;
	typedef Store<int, std_priority_queue<int> > IntStoreP;
	typedef Store<int, boost::lockfree::queue<int> > IntStoreB;
	
	IntStore& intstore = IntStore::get_mutable_instance();
	IntStoreP& intstorep = IntStoreP::get_mutable_instance();
	IntStoreB& intstoreb = IntStoreB::get_mutable_instance();
	
	intstore.empty();
	intstorep.empty();
	intstoreb.empty();
	intstoreb.empty();
}

int main() {
	foo();
	return 0;
}
