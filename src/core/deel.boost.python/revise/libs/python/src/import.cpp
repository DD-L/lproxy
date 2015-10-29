// Copyright Stefan Seefeld 2005.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <deel.boost.python/revise/boost/python/import.hpp>
#include <deel.boost.python/revise/boost/python/borrowed.hpp>
#include <deel.boost.python/revise/boost/python/extract.hpp>
#include <deel.boost.python/revise/boost/python/handle.hpp>

namespace boost 
{ 
namespace python 
{

object BOOST_PYTHON_DECL import(str name)
{
  // should be 'char const *' but older python versions don't use 'const' yet.
  char *n = python::extract<char *>(name);
  python::handle<> module(PyImport_ImportModule(n));
  return python::object(module);
}

}  // namespace boost::python
}  // namespace boost
