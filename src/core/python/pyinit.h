#ifndef PYINIT_H
#define PYINIT_H
#include <boost/noncopyable.hpp>
class pyinit : boost::noncopyable {
	public:
		pyinit(int initsigs = 1);
		~pyinit() {
			// Py_Finalize();
		}
		bool isInitialized();
		const char* version();
		static void err_print();
};
#endif // PYINIT_H
