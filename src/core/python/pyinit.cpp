#include <deel.boost.python/revise/boost/python.hpp>
#include <Python.h>
#include "python/pyinit.h"

pyinit::pyinit(int initsigs/* = 1*/) {
	assert(initsigs == 0 || initsigs == 1);
	Py_InitializeEx(initsigs);
}
bool pyinit::isInitialized() {
	return Py_IsInitialized();
}
const char* pyinit::version() {
	return Py_GetVersion();
}
void pyinit::err_print() {
	PyErr_Print();
}
