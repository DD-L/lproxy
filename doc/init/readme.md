初始化编译环境

------------------

$ROOT - 项目根目录


	cd $ROOT
	make init

make init （快速初始化）将做以下事情：

1. 检查g++对C++11的支持情况
2. 释放boost库
	> boost源码是用7z压缩的，所以解压依赖7z工具。自动化脚本会在系统中寻找7z/7za/7zr，如果都没有找到，会自己编译一个7zr工具。
	>> linux下的7z工具被安装在：$ROOT/tools/7zip/bin 目录下
	>> win32的7z工具被放置在： $ROOT/tools/7zip/bin.win32 目录下
3. 配置libevent库
4. 配置libev库

（如果要强制初始化，可以使用 ' make init.force ' ）

-----------------

附自动化脚本 ./boost_init.sh 的帮助：

	$ ./boost_init.sh --help
	Usage:
	        ./boost_init.sh [ option ]
	
	option:
	  -h, --help:   show help
	  -force:       Forced initialization


------------------