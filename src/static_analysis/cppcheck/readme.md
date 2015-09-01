src/static_analysis/cppcheck/readme.md

----------------------------------

static analysis
===============
### 代码静态检查



####tools:

	cppcheck
	cppcheck-htmlreport

####source:

	$ROOT/tools/static_analysis_tool/cppcheck/cppcheck-1.69.tar.bz2
	
	<$ROOT为项目根目录，下同>
####manual:

	$ROOT/tools/static_analysis_tool/cppcheck/manual.pdf
	
####install:
	1. 手动安装
		(1) make
			cd $ROOT/tools/static_analysis_tool/cppcheck
			tar xvf cppcheck-1.69.tar.bz2
			cd cppcheck-1.69
			make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
		(2) install
			cd $ROOT/tools/static_analysis_tool/cppcheck
			mkdir -p ./bin/cfg/
			cp -a ./cppcheck ./bin/
			cp -a ./htmlreport/cppcheck-htmlreport ./bin/
			cp -a ./cfg/std.cfg ./bin/cfg/
			chmod +x ./bin/cppcheck
		(3) check
			1. 文件 ./bin/cppcheck 必须有执行权限
			2. 文件 ./bin/cfg/std.cfg 必须存在
			3. 文件 ./bin/cppcheck-htmlreport 的执行：
				1. 依赖python运行环境，2.7.x 或 更高版本
				2. 依赖python模块Pygments ( http://pygments.org/ )
						如果缺失Pygments, 请执行 pip install Pygments 安装;
						或到 https://pypi.python.org/pypi/Pygments 下载安装
					
						Pygments是一个语法高亮工具，可用于例如论坛、wiki和其他方面的 Web 应用；
						提供命令行工具和开发包，可输出包括 HTML, RTF, LaTex and ANSI 序列等格式。
	
	2. 自动化脚本安装(推荐)
		1. 方式一：
			cd $ROOT/tools/static_analysis_tool/cppcheck; make
		2. 方式二： (墙裂推荐)
			cd $ROOT/src/static_analysis/cppcheck; make init

####uninstall

	1. 手动卸载
		cd $ROOT/tools/static_analysis_tool/cppcheck
		rm -rf ./cppcheck-1.69
		rm -rf ./bin
	2. 自动化脚本卸载
		cd $ROOT/tools/static_analysis_tool/cppcheck
		make uninstall

####usage:

	1. cppcheck 简单使用示例
		1. cppcheck
			cp $ROOT/tools/static_analysis_tool/cppcheck/bin/cfg/std.cfg ./
			$ROOT/tools/static_analysis_tool/cppcheck/bin/cppcheck --xml --std=c++11 --template=gcc --enable=all --force -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
			rm ./std.cfg
		2. cppcheck-htmlreport (python tool，依赖python运行环境，本机v2.7.x)
			#pip install Pygments #依赖Pygments模块, 参见 install|手动安装|check
			$ROOT/tools/static_analysis_tool/cppcheck/bin/cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR
	
	2. 自动化脚本 
		躶用cppcheck太麻烦，于是编写了静态检查自动化脚本 Makefile, cppcheck_report.sh。
		（注意严格的代码静态检查会很耗时, 大部分时间都是在扫描的库）
		
		注：以下涉及到的命令，在没有特别说明的情况下，都是在 $ROOT/src/static_analysis/cppcheck 路径下完成的

		1. 配置脚本（一般不需要手动配置）
			1. 修改cppcheck_report.sh中的 XX 变量.(一般不需修改)
			
				XX=g++
				如果你使用其它g++编译器(比如交叉编译器)，需在此修改；否则没有必要修改XX.

			2. 修改cppcheck_report.sh脚本文件中的 INCLUDEFILES 变量.(一般不需修改)
			
				如果 cppcheck_report.sh 没有得到正确的$XX编译器的include搜索路径列表，需在此修改；
				否则没有必要修改INCLUDEFILES.
				
				手动获取编译器include搜索路径列表方法:
					$ touch ./test.cpp
					$ g++ -v ./test.cpp -o a.out
					$ rm ./test.cpp ./a.out
					在输出结果中:
						#include "..."
						#include <...>
					的下面就是g++程序的头文件搜索列表.

			3. 如果想缩短扫描时间，修改cppcheck_report.sh中的 cppcheck 参数即可: 剔除 --force 选项
			
			4. 脚本cppcheck_report.sh的执行依赖python运行环境, 如果缺失python, 请先安装python(推荐版本>=v2.7.x)

		2. 使用示例如下：
			1. 生成报告
				1. make # 或者 make all 
					会在当前目录生成项目所有源码的静态检查报告，报告在xxx_report/xxx_report目录内
				
				2. make 源码目录
					生成单个报告				
	
					“源码目录”是相对 $ROOT/src/ 的相对路径
	
					比如：make core/log
					只会在当前目录生成 core/log 目录的静态检查报告:core_report/log_report，
					脚本会对“源码目录”的每一层目录都加上_report后缀。其它项目模块类推。

					再如 make core, 只会在当前目录生成 core 目录的静态检查报告:core_report, 
					core_report包含core源码目录所有源文件的报告。

			2. 报告清理
				1. make all.clean
					会删除项目所有源码的静态检查报告，并删除 ./std.cfg ./gcc_include_cpp_path;
						* ./std.cfg:               cppcheck程序所需
						* ./gcc_include_cpp_path： 自动化脚本cppcheck_report.sh所需
					如果当前路径存在./core_report或./gui_report目录, 也会将 ./core_report
					和 ./gui_report 一并删除
				
				2. make 源码目录.clean
					单个清理
					
					“源码目录”是相对 $ROOT/src/ 的相对路径		
	
					例如：make core/log.clean
					只会删除 core/log 模块的静态检查报告，其它项目模块类推
