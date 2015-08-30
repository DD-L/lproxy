$ # make
$ make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
$
$ # install
$ mkdir -p /opt/cppcheck/cfg/
$ cp -a ./cppcheck /opt/cppcheck/
$ cp -a ./htmlreport/cppcheck-htmlreport /opt/cppcheck/
$ cp -a ./cfg/std.cfg /opt/cppcheck/cfg/
$ # export PATH=${PATH}:/opt/cppcheck

cppcheck 的使用：
$ cp /opt/cppcheck/cfg/std.cfg ./
$ cppcheck --xml --std=c++11 --template=gcc --enable=all -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
$ rm ./std.cfg


cppcheck-htmlreport的使用
$ #pip install Pygments
cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR



1. 工具简介

	1. 下载安装（Linux）：
		1. 官方网址：http://sourceforge.net/projects/cppcheck/
		2. 安装方式：解压后，直接make进行编译，完成后，make install进行安装
		3. 验证安装：cppcheck -v 查看是否安装成功

	2. 特点介绍
		1. 主要检查编译器通常不能发现的缺陷。
		2. cppcheck能够发现很多错误，但不能发现所有的错误。

2. 基本使用
	1. 检查单个文件：cppcheck foo.c
	2. 检查文件夹：cppcheck path
	3. 排除一个文件或者一个文件夹：cppcheck -isrc/foo.c src

	4. 使能检查规则：
		1. 默认：--enable=error
		2. --enable=all 
		3. --enable=unusedFuntion path
		4. --enable=style

	5. 规则定义：
		error：出现的错误
		warning：为了预防bug防御性编程建议信息
		style：编码格式问题（没有使用的函数、多余的代码等）
		portablity：移植性警告。该部分如果移植到其他平台上，可能出现兼容性问题
		performance：建议优化该部分代码的性能
		information：一些有趣的信息，可以忽略不看的。

	6. 保存结果到文件中：重定向“>”
	7. 多线程检查代码（提高检查速度，充分利用CPU功能）：cppcheck -j 4 src

3. 高级使用

	1. xml 输出
		1. 使用方式：cppcheck --xml-version=2 foo.cpp

		2. error的组成元素

				id：error的id
				severity：error的性质（error、warning......）
				msg：error的具体信息（短格式）
				verbose：error的信息（长格式）
				location的组成元素：
				file：含有相对或者绝对路径的文件名
				line：行数
				msg：消息描述

	2. 改良输出结果
		cppcheck --template=vs path （Visual Studio 兼容模式）
		cppcheck --template=gcc path （Gcc兼容模式）
		cppcheck --template={"{file},{line},{severity},{id},{message}"} （自定义模式）

	3. 输出过滤器（选特定的错误信息输出）
		1. 命令行模式：cppcheck --suppress=memleak:src/file1.cpp src/
		2. 使用文件模式（将过滤规则存到文件中）：cppcheck --suppressions suppressions.txt src/

4. 报告美化
	1. ./cppcheck test.cpp --xml 2> err.xml
	2. htmlreport/cppcheck-htmlreport --file=err.xml --report-dir=test1 --source-dir=.

5 GUI应用（cppcheck提供GUI的用户接口，比较简单，略去不写）

6. 参考cppcheck用户手册