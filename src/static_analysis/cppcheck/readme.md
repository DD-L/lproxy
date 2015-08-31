src/static_analysis/cppcheck/readme.md

----------------------------------

static analysis
===============
### 代码静态检查



####tools:

	cppcheck
	cppcheck-htmlreport

####source:

	tools/static_analysis_tool/cppcheck/cppcheck-1.69.tar.bz2

####manual:

	tools/static_analysis_tool/cppcheck/manual.pdf
	
####install:

	tar xvf cppcheck-1.69.tar.bz2
	cd cppcheck-1.69
	make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
	
	mkdir -p /opt/cppcheck/cfg/
	cp -a ./cppcheck /opt/cppcheck/
	cp -a ./htmlreport/cppcheck-htmlreport /opt/cppcheck/
	cp -a ./cfg/std.cfg /opt/cppcheck/cfg/
	# export PATH=${PATH}:/opt/cppcheck # 配置环境变量 （必要的配置项目，自动化脚本所需）
	
####usage:

	1. cppcheck 简单使用示例
		1. cppcheck
			cp /opt/cppcheck/cfg/std.cfg ./
			cppcheck --xml --std=c++11 --template=gcc --enable=all --force -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
			rm ./std.cfg
		2. cppcheck-htmlreport (python tool，依赖python运行环境，本机v2.7.x)
			#pip install Pygments #依赖Pygments模块
			cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR
	
	2. 自动化脚本 
		躶用cppcheck太麻烦，于是自己编写了静态检查自动化脚本 Makefile, cppcheck_report.sh。
		（注意严格的代码静态检查会很耗时, 大部分时间都是在扫描的库）

		1. 配置脚本（一般不需要手动配置）
			1. 修改cppcheck_report.sh中的 XX 变量.(一般不需修改)
				XX=g++
				如果你使用其它g++编译器(比如交叉编译器)，需在此修改；否则没有必要修改XX.
			2. 修改cppcheck_report.sh脚本文件中的 INCLUDEFILES 变量.(一般不需修改)
				如果 cppcheck_report.sh 没有得到正确的$XX编译器的include搜索路径列表，需在此修改；否则没有必要修改INCLUDEFILES.
				
				手动获取编译器include搜索路径列表方法:
					$ g++ -v ./test.cpp -o a.out; rm a.out
					在输出结果中:
						#include "..."
						#include <...>
					的下面就是g++程序的头文件搜索列表.
			3. 如果想缩短扫描时间，修改cppcheck_report.sh中的 cppcheck 参数即可: 剔除 --force 选项

		2. 使用示例如下
			1. 生成报告
				1. make # 或者 make all 
					会在当前目录生成项目所有源码的静态检查报告，报告在xxx/xxx_report目录内
				
				2. make 源码目录
					生成单个报告				
	
					源码目录是相对 src/ 的相对路径
	
					比如：make core/log
					只会在当前目录生成 core/log 目录的静态检查报告:core/log_report，其它项目模块类推
			2. 清理
				1. make all.clean
					会删除项目所有源码的静态检查报告，并删除 ./std.cfg ./gcc_include_cpp_path;
						* ./std.cfg:               cppcheck程序所需
						* ./gcc_include_cpp_path： 自动化脚本cppcheck_report.sh所需
					如果当前路径存在./core或./gui目录，且其为空目录，也会将 ./core 和 ./gui 一并删除
				
				2. make 源码目录.clean
					单个清理
					
					源码目录是相对 src/ 的相对路径			
	
					例如：make core/log.clean
					只会删除 core/log 模块的静态检查报告，其它项目模块类推
