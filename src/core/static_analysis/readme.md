src/core/static_analysis/readme.md

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
	# export PATH=${PATH}:/opt/cppcheck # 配置环境变量 （必要）
	
####usage:

	1. cppcheck 使用简单示例
		1. cppcheck
			cp /opt/cppcheck/cfg/std.cfg ./
			cppcheck --xml --std=c++11 --template=gcc --enable=all -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
			rm ./std.cfg
		2. cppcheck-htmlreport (python tool，依赖python运行环境，本机v2.7.x)
			#pip install Pygments #依赖Pygments模块
			cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR
	
	2. 自动化脚本 
		躶用cppcheck太麻烦，于是自己编写了静态检查自动化脚本 Makefile, cppcheck_report.sh。
		使用示例如下
		1. 生成报告
			make # 或者 make all 
			会在当前目录生成项目所有源码的静态检查报告，报告在xxx_report目录内
			make log
			只会在当前目录生成log模块的静态检查报告，其它项目模块类推
		2. 清理
			make all.clean
			会删除项目所有源码的静态检查报告，并删除./std.cfg
			make log.clean
			只会删除log模块的静态检查报告，其它项目模块类推
