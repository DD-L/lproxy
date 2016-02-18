### 初始化编译环境

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

### 关于 Makefile

lproxy 中可以使用的 Makefile 变量：

##### 使用方法:

```shell
$ make [变量=值] ...
```

##### 类型及规则

1. 覆盖式:

    | 变量| 默认值     |
    |-----|------------|
	| CC  |  gcc       |
	| CXX |  g++       |
	| AR  |  ar        |
	| RM  |  rm -rf    |

	'覆盖式' 变量一旦通过 `make 变量=值` 的方式传进去，它将覆盖 Makefile 的原有的变量值。

2. 增量式:

    | 变量    | 含义                                   |
    |---------|----------------------------------------|
	| CFLAGS  |  $(CC) 使用的参数                      |
	| CXXFLAGS|  $(CXX) 使用的参数                     |
	| INCPATH |  $(CC) 和 $(CXX) 共用的 include path   |
	| LIBS    |  $(CC) 和 $(CXX) 共用的 `-l` 选项      |
	| LIBPATH |  $(CC) 和 $(CXX) 共用的 `-L` 选项      |

	'增量式' 变量通过 `make 变量=值` 的方式传进去后，原先预设的值不会丢失，它会将你设置的变量放在靠前的位置，然后紧跟着是原先预设的值。假如 Makefile 中原先定义的变量 CFLAGS 值为 `-Wall` ,执行 `make CFLAGS=-g` , CFLAGS 的值会变成 `-g -Wall` 。

	注意：INCPATH 和 LIBPATH 变量中的路径必须为绝对路径

3. 开关式:

	| 变量| 值 | 含义 |
    |-----|----|------|
    | GDB | -g | $(CC) 和 $(CXX) 的 `-g` 选项。 即 gdb 默认开启，若要关闭 gdb 支持, 给 make 一个空值参数即可： `make GDB=` |
	