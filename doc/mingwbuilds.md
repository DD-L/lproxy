### 编译一个 Windows 版本的 `lproxy`

Windows 版本的 lproxy 二进制程序下载： [稍等](#)

**注意这里的编译是在 MinGW32 下完成的**

下面是我的编译环境：

1. Qt 5.5 for Desktop (MinGW 4.9.2 32bit)

	这是一个 Qt 5.5 自带的 32 位的 MinGW， gcc 版本是 `4.9.2`， make 程序是 `mingw32-make`

2. git version 2.7.0.windows.1
	
	git 来源： [https://git-for-windows.github.io/](https://git-for-windows.github.io/)

3. Cygwin

	目前在 Windows 上编译 `lproxy` 还不是做的那么完美，仍需 Cygwin 的部分支持。

4. Windows 系统环境变量 %PATH% 摘要：

	注意顺序 `mingw`、`system32`、`cygwin`、`git` 之间的顺序。

	```
	> echo %PATH%
	D:\Qt\QtDev\5.5\mingw492_32\bin;D:\Qt\QtDev\Tools\mingw492_32\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;D:\DEV\Perl64\bin;D:\DEV\Perl64\site\bin;D:\DEV\cygwin\bin;D:\DEV\Ruby22\bin;D:\DEV\Python35;D:\DEV\Python35\Scripts;D:\DEV\Git\bin;
	```
5. 硬件及系统

	* CPU: Intel(R) Core(TM) i5-2520M CPU @ 2.50GHz 2.50 GHz
	* RAM: 8.00 GB
	* OS:  Windows 10 x64

	这里列出 CPU 信息，是因为在编译 `lproxy` 的依赖库 `cryptopp` 时 *可能* 需要了解。 issue: [#116](https://github.com/DD-L/lproxy/issues/116)


##### 编译步骤：

下面语句都是在 MinGW shell 中完成的。

1.  Get 源码。

	```shell
	git clone https://github.com/DD-L/lproxy.git
	cd lproxy
	git submodule update --init
	```

2. 释放 boost 源码。**`%lproxy_root%` 代表 `lproxy` 项目根目录，下同**

	```
	cd %lproxy_root%\contrib\boost
	mingw32-make -f Makefile.win32
	```
3. 编译 `cryptopp` 静态库。

	```
	cd %lproxy_root%\contrib\cryptopp
	mingw32-make MAKE=mingw32-make CXXFLAGS="-DNDEBUG -O2 -mtune=pentium4"
	```

	注意，在我的编译和调试环境中给 gcc 添加了 `-mtune=pentium4` 选项，详细信息移步 issue: [#116](https://github.com/DD-L/lproxy/issues/116)

4. 编译 `boost_build`

	```
	cd %lproxy_root%\src\core\boost_build
	mingw32-make GDB= CXXFLAGS=-O2 -f Makefile.win32
	```
5. 编译 `log` 库

	```
	cd %lproxy_root%\src\core\log
	mingw32-make MAKE=mingw32-make CC=gcc GDB= CXXFLAGS=-O2 Container_Type=-DLOG_LOCKFREE_QUEUE -f Makefile.win32
	```

7. 编译 `crypto`

	```
	cd %lproxy_root%\src\core\crypto
	mingw32-make GDB= CXXFLAGS=-O2 MAKE=mingw32-make CC=gcc -f Makefile.win32
	```

8. 编译 `lss`

	```
	cd %lproxy_root%\src\core\lss
	mingw32-make MAKE=mingw32-make CC=gcc GDB= CXXFLAGS=-O2 -f Makefile.win32
	```

9. install

	```
	cd %lproxy_root%
	mingw32-make install
	```
	至此，`lproxy` 二进制文件被复制到 `%lproxy_root%\bin` 目录下，编译结束。

##### MinGW 下的 Makefile.win32 说明

基本上 [Makefile 公共变量](./MakefileVariables.md) 在 Makefile.win32 中都适用。

##### 其他编译单元举例：

* 清除释放的 boost 源码

	```
	cd %lproxy_root%\contrib\boost
	mingw32-make clean -f Makefile.win32 
	```

* 清除 cryptopp 源码 及 编译生成的二级制文件

	```
	cd %lproxy_root%\contrib\cryptopp
	mingw32-make MAKE=mingw32-make clean
	```
* 清除 crypto 及其 test 的二进制文件

	```
	cd %lproxy_root%\src\core\crypto
	mingw32-make MAKE=mingw32-make clean -f Makefile.win32
	```

* 异常组件编译与清理

	```
	cd %lproxy_root%\src\core\except
	mingw32-make MAKE=mingw32-make  -f Makefile.win32
	mingw32-make MAKE=mingw32-make  -f Makefile.win32 clean
	```

* 日志库 及其 demo 的 编译与清理

	```
	cd %lproxy_root%\src\core\log
	```
	* 编译 LOG_LOCKFREE_QUEUE 版本的日志库
	
		```
		mingw32-make MAKE=mingw32-make CC=gcc GDB= CXXFLAGS=-O2 Container_Type=-DLOG_LOCKFREE_QUEUE -f Makefile.win32
		```
	* 编译 LOG_PRIORITY_QUEUE 版本的日志库

		```
		mingw32-make MAKE=mingw32-make CC=gcc GDB= CXXFLAGS=-O2 Container_Type=-DLOG_LOCKFREE_QUEUE -f Makefile.win32
		```

	* 编译 demo / demo2 编译命令同上

		```
		cd %lproxy_root%\src\core\log\demo
		cd %lproxy_root%\src\core\log\demo2
		```
	 	如果日志输出线程大于 1 个，需要定义 `USE_LOCK=-DLOG_USE_LOCK`

	* `log` 库 test 部分暂时还没有提供 Windows 环境下的编译方法。

	* 清理 日志库 及其 demo 二进制文件
		
		```
		mingw32-make clean -f Makefile.win32
		```
* 清理异常组件的 二进制文件

	```
	cd %lproxy_root%\src\core\except
	mingw32-make clean -f Makefile.win32
	```

* 编译 DEBUG 版本 的 lss

	```
	cd %lproxy_root%\src\core\lss
	mingw32-make MAKE=mingw32-make CC=gcc DEBUG=enable -f Makefile.win32
	```

* 清理 lss 二进制文件

	```
	cd %lproxy_root%\src\core\lss
	mingw32-make clean -f Makefile.win32
	```