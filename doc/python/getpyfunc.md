python/GetPyFunc.h

-----

# GetPyFunc

```cpp
class GetPyFunc;
```

GetPyFunc 封装了对 Python 的操作，可以直接获取 python 的函数对象。


### 类摘要

```cpp
class GetPyFunc {
    public:
        GetPyFunc(const std::string& pyfile, const std::string& function);
        GetPyFunc(const GetPyFunc& that);
        GetPyFunc& operator= (const GetPyFunc& that);

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

		boost::python::object operator()();
		
		template<typename __11>
		boost::python::object operator()(const __11& arg);
		
		template<typename __21, typename __22>
		boost::python::object operator()(const __21& arg1, const __22& arg2);
		
		template<typename __31, typename __32, typename __33>
		boost::python::object operator()(const __31& arg1, const __32& arg2,
				const __33& arg3);

		template<typename __41, typename __42, typename __43, typename __44>
		boost::python::object operator()(const __41& arg1, const __42& arg2,
				const __43& arg3, const __44& arg4);

		template<typename __51, typename __52, typename __53,
				typename __54, typename __55>
		boost::python::object operator()(const __51& arg1, const __52& arg2,
				const __53& arg3, const __54& arg4, const __55& arg5);

	    template<typename __61, typename __62, typename __63, 
				typename __64, typename __65, typename __66>
		boost::python::object operator()(const __61& arg1, const __62& arg2, 
                const __63& arg3, const __64& arg4, const __65& arg5, 
                const __66& arg6);

		template<typename __71, typename __72, typename __73, 
                typename __74, typename __75, typename __76, typename __77>
        boost::python::object operator()(const __71& arg1, const __72& arg2, 
                const __73& arg3, const __74& arg4, const __75& arg5, 
                const __76& arg6, const __77& arg7);

		template<typename __81, typename __82, typename __83, 
                typename __84, typename __85, typename __86,
                typename __87, typename __88>
        boost::python::object operator()(const __81& arg1, const __82& arg2, 
                const __83& arg3, const __84& arg4, const __85& arg5, 
                const __86& arg6, const __87& arg7, const __88& arg8);

		template<typename __91, typename __92, typename __93, 
                typename __94, typename __95, typename __96, 
                typename __97, typename __98, typename __99>
        boost::python::object operator()(const __91& arg1, const __92& arg2, 
                const __93& arg3, const __94& arg4, const __95& arg5, 
                const __96& arg6, const __97& arg7, const __98& arg8, 
                const __99& arg9);
};
```

在 C++11 以后的版本中可接受的 python 函数参数个数没有限制。但在 C++11 之前的版本中，python 函数 最多只能 9 个参数。



### 其他一些需要留意的地方

1. 如果在执行 python 过程中出错，有异常 `boost::python::error_already_set` 被抛出，程序会`exit(-1)`, 并在 `stderr` 输出错误信息。

2. 变长参数模板函数 `template<typename ...T> object operator() (T&&... arg);` 可以在C代码中直接像调用 python 函数一样使用. 注意需要C++11支持。同时需注意传入的参数类型应为 python C API 所能接受的数据类型, 比如 `std::string` 就不可以。

3. [过时的]重载多达10个版本的模板函数 `operator()`。

4. 依赖 boost.python

5. 依赖 python v2.7.x (推荐'2.7.5'及其以上的2.7版本). 

	需要 `python_dev`支持, 比如 Ubuntu 上安装方法： `sudo apt-get install python-dev` 
	
	1. 在 windows 上如果要想支持 python 3.x . 需要修改 `getpyfunc.h` 文件中的

		```cpp
		#ifdef _WIN32
		#pragma comment(lib, "python27.lib")
		#endif
		```
		将 `#pragma comment(lib, "python27.lib")` 修改成你使用的 python 版本即可。
	
	2. 在 linux 上使用时，只要编译连接时指定正确的 python 路径即可.

		比如在编译是指定 PYTHON_INCLUDE , 这是 `Python.h` 所在路径 ：
		
		```cpp
		make  PYTHON_INCLUDE=/usr/include/python2.7
		```
		在编译时指定 PYTHON_INCLUDE， 链接时指定 PYTHON_LIB_PATH 和 PYTHON_LIB
		```cpp
		make  PYTHON_INCLUDE=/usr/include/python2.7 PYTHON_LIB_PATH=/usr/lib PYTHON_LIB=-lpython2.7
		```
		注意， 如你在 Cygwin 下链接 python 库，那么 `PYTHON_LIB=-lpython2.7` 需要改成 `PYTHON_LIB=-lpython2.7.dll` 
		

		
	

	