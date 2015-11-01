#ifndef GETPYFUNC_H
#define GETPYFUNC_H
/*
 *  GetPyFunc
 *
 * 1.   特点：封装对Python操作的操作，可以直接获取python的函数对象
 * 2.   原型：class GetPyFunc;
 * 3.   说明： 
 *      1.  特别要提醒的是，如果在执行python过程中，
 *          有异常boost::python::error_already_set被抛出, 
 *          程序会exit(-1), 并在 stderr 输出错误信息。
 *      2.  变长参数模板函数
 *          template<typename ...T> object operator() (T&&... arg);
 *          可以在C代码中直接像调用python函数一样使用. 注意需要C++11支持
 *          同时需注意传入的参数类型应为python C API所能接受的数据类型,
 *          比如std::string就不可以。
 *      3.  [过时的]重载多达10个版本的模板函数operator()的代码,
 *          采用如上所说的变长参数模板函数, 但是需要C++11的支持.
 *          被注释掉的代码像boost::bind()一样仅仅支持最多传人9个不同类型参数;
 *          新代码的可传参数个数则不受限制, 所以弃用过时的代码.
 *
 *  4. 依赖:
 *      1. python v2.7.x (推荐'2.7.5'及其以上的2.7版本)
 *      2. boost::python
 */

// example:
//
//./example.py:
//#! /usr/bin/python
//#encoding=utf-8
//def pyfunc(num, msg = 'hello'):
//  print msg
//  return num + 100
//
//example.cpp:
//#include "getpyfunc.h"
//#include <iostream>
//int main() {
//      const std::string filename = "./example.py";
//      GetPyFunc pyfunc(filename.c_str(), "pyfunc");
//      object rett = pyfunc(2, "haha");
//      std::cout << extract<int>(rett) << std::endl;
//      
//      GetPyFunc pyfunc2(pyfunc);
//      std::cout << extract<int>(pyFunc2(200)) << std::endl;
//      return 0;
//}
//             
//out:
//haha
//102
//Hello
//300


#define BOOST_PYTHON_SOURCE

#ifdef _WIN32
#pragma comment(lib, "python27.lib")
#endif

#include <deel.boost.python/revise/boost/python.hpp> 
#include "python/pyinit.h"

#include <string>
//using namespace boost::python;

class GetPyFunc {
    public:
        GetPyFunc(const std::string& pyfile, const std::string& function)
                    : func(function) {
            main_ns = boost::python::import("__main__").attr("__dict__");
            exec_file(pyfile.c_str(), main_ns);
        }
        GetPyFunc(const GetPyFunc& that) {
            main_ns = that.main_ns;
            func = that.func;
        }
        GetPyFunc& operator= (const GetPyFunc& that) {
            main_ns = that.main_ns;
            func = that.func;
            return *this;
        }


#if __cplusplus >= 201103L
        // c++0x
        // object operator()(...);
        // The number of parameters and parameter types are unlimited
        template<typename ...T>
        boost::python::object operator()(T&&... arg) 
        try {
            return main_ns[func.c_str()](std::forward<T>(arg)...);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }
#else   // __cplusplus < 201103L
        // Low version
        // with 10 operator()(...)
        // No.1
        boost::python::object operator()()
        try {
            return main_ns[func.c_str()]();
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }
        // No.2
        template<typename __11>
        boost::python::object operator()(const __11& arg)
        try {
            return main_ns[func.c_str()](arg);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.3
        template<typename __21, typename __22>
        boost::python::object operator()(const __21& arg1, const __22& arg2)
        try {
            return main_ns[func.c_str()](arg1, arg2);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.4
        template<typename __31, typename __32, typename __33>
        boost::python::object operator()(const __31& arg1, const __32& arg2, 
                const __33& arg3)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.5
        template<typename __41, typename __42, typename __43, typename __44>
        boost::python::object operator()(const __41& arg1, const __42& arg2, 
                        const __43& arg3, const __44& arg4)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.6
        template<typename __51, typename __52, typename __53, 
                typename __54, typename __55>
        boost::python::object operator()(const __51& arg1, const __52& arg2, 
                const __53& arg3, const __54& arg4, const __55& arg5)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4, arg5);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.7
        template<typename __61, typename __62, typename __63, 
                typename __64, typename __65, typename __66>
        boost::python::object operator()(const __61& arg1, const __62& arg2, 
                const __63& arg3, const __64& arg4, const __65& arg5, 
                const __66& arg6)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4, arg5, arg6);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.8
        template<typename __71, typename __72, typename __73, 
                typename __74, typename __75, typename __76, typename __77>
        boost::python::object operator()(const __71& arg1, const __72& arg2, 
                const __73& arg3, const __74& arg4, const __75& arg5, 
                const __76& arg6, const __77& arg7)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4, 
                    arg5, arg6, arg7);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.9
        template<typename __81, typename __82, typename __83, 
                typename __84, typename __85, typename __86,
                typename __87, typename __88>
        boost::python::object operator()(const __81& arg1, const __82& arg2, 
                const __83& arg3, const __84& arg4, const __85& arg5, 
                const __86& arg6, const __87& arg7, const __88& arg8)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4, 
                                        arg5, arg6, arg7, arg8);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }

        // No.10
        template<typename __91, typename __92, typename __93, 
                typename __94, typename __95, typename __96, 
                typename __97, typename __98, typename __99>
        boost::python::object operator()(const __91& arg1, const __92& arg2, 
                const __93& arg3, const __94& arg4, const __95& arg5, 
                const __96& arg6, const __97& arg7, const __98& arg8, 
                const __99& arg9)
        try {
            return main_ns[func.c_str()](arg1, arg2, arg3, arg4, arg5, 
                                        arg6, arg7, arg8, arg9);
        }
        catch (boost::python::error_already_set&) {
            pyexception();
            return boost::python::api::object();
        }
        // Damn, I don't want to write any more, 
        // and now, with 10 Enough, like boost::bind()
#endif // __cplusplus
    private:
        void pyexception() throw () {
            fprintf(stderr, "catch python exception: ");
            pyinit::err_print();
            fprintf(stderr, "Exit. [code:-1]");
            exit(-1);
        }
    private:
        pyinit                 pinit;
        boost::python::object  main_ns;
        std::string            func;
};
#endif // GETPYFUNC_H

