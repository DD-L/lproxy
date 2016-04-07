# Makefile Variables

依据 `lproxy` 使用的 Makefile **公共**变量规则，大致可分为两类：“`可覆盖型变量`”和“`+= 型变量`”。

### 1. “可覆盖型变量”

| 变量     | 默认值  | 说明|
|----------|---------|--------------------------------------------|
| `RM`     | rm -rf  | 删除文件(夹)                               |
| `AR`     | ar      | 打包静态库                                 |
| `CP`     | cp -a   | 拷贝文件（夹）                             |
| `GDB`    | -g      | gdb                                        |
| `CC`     | cc      | C 语言编译器；`cc` 通常是指向 `gcc`        |
| `CXX`    | g++     | C++ 语言编译器                             |
| `LIBPATH`| 空      |给编译器在链接时指定静态库或动态库的搜索路径|

顾名思义，这些变量的值在执行 make 语句时可以被替换。

*注：目前 lproxy 默认开启 GDB 选项，如果要关闭 GDB，请给 `make` 传递 `GDB=`， 即赋予其空值即可*

### 2. “+= 型变量”

| 变量      | 默认值              | 用途 |
|-----------|---------------------|-----------------------------------------------|
| `CFLAGS`  | 视具体 Makefile 而定| C 语言编译器的参数，针对 `*.c`文件            |
| `CXXFLAGS`| 视具体 Makefile 而定| C++ 语言编译器的参数，针对 `*.cpp` `*.hpp`文件|
| `INCPATH` | 视具体 Makefile 而定| 给编译器在编译时指定 `#include` 搜索路径      |
| `LIBS`    | 视具体 Makefile 而定| 给编译器在链接时指定动态库或静态库            |

“+= 型变量”不可被覆盖，但是可以在**默认值前面**追加。


比如在 Makefile 中 `CXXFLAGS` 有可能这样被定义：

```Makefile
override CXXFLAGS += -Wall $(GDB) -std=c++1y 
```
如果在执行 make 时给 `CXXFLAGS` 赋值为`-O2`，即 `CXXFLAGS=-O2`，则 `CXXFLAGS` 的值会变成

```
-O2 -Wall $(GDB) -std=c++1y
```

### 变量如何传递到 Makefile

1. 方式一：通过 shell 变量来传递。比如
	
	```shell
	$ export GDB=
	$ export CXXFLAGS=-O2
	$ make ...
	```

2. 方式二：通过 make 选项来传递。比如

	```shell
	$ make ... GDB=  CXXFLAGS=-O2  ...
	```
