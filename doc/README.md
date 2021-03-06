
*目录*

1. *[lproxy](#lproxy)*
	1. *获取 lproxy 源码*
	2. *编译安装 lproxy*
	3. *运行 lproxy 服务*
	4. *配置文件*
	5. */path/to/lproxy/Makefile “伪目标”*
	7. *Makefile 公共变量*
	6. *补充说明*
		* *boost 源码*
		* *从零配置编译环境*
		* *Windows/MinGW 下编译 lproxy*
		* *直接获取 lproxy 二进制程序*
		* *不推荐在Cygwin 平台下使用 lproxy*
2. *[Docker 支持](#docker-支持)*
	1. *lproxy 镜像*
		1. *获取 lproxy Docker 镜像*
		2. *在容器中运行 lproxy 服务*
	2. *lproxy-dev 镜像*
		1. *获取 lproxy-dev Docker 镜像*
		2. *运行 lproxy-dev 容器*
3. *[Releases](#releases)*
4. *[即刻免费体验 lproxy 远程代理服务](#即刻体验-lproxy-远程代理服务)*
5. *[简易的 lproxy server 端集群部署方案](#简易的-lproxy-server-端集群部署方案)*
6. *[lproxy 前进计划](#lproxy-前进计划)*
7. *[开发者文档](#开发文档)*
8. *[测试](#测试)*
9. *[TODO list](#todo-list)*


## lproxy

`lproxy` 是一套轻巧的、一对多的、安全的 Socks5 网络代理服务。

1. 获取 `lproxy` 源码

	```shell
	$ git clone https://github.com/DD-L/lproxy.git lproxy
	$ cd lproxy
	$ git submodule init
	$ git submodule update
	```

2. 编译安装 `lproxy`

	*以 linux 为例*

	```shell
	$ cd /path/to/lproxy
	$ make init
	$ make lss GDB= CXXFLAGS=-O2   # 如果是在 Cygwin 上编译，换成 make lss.cygwin
	$ make install                 # 程序文件和配置文件会被复制到 /path/to/lproxy/bin 文件夹内
	```
	若要编译 `DEBUG` 版本的 `lss`，对 `make lss` 添加 `DEBUG=enable` 选项即可。更多信息请查阅 [Makefile Variables](./MakefileVariables.md)。


3. 运行 `lproxy` 服务

	* 运行 local 端程序示例:

		```shell
		$ ./bin/lsslocal.exe -c ./bin/local-config.json
		```
		`-c` 选项指定配置文件。如果不指定，则会在当前工作目录寻找一个名字叫做 `local-config.json` 的文件

	* 运行 server 端程序示例:

		```shell
		$ ./bin/lssserver.exe -c ./bin/server-config.json
		```
		`-c` 选项指定配置文件。如果不指定，则会在当前工作目录寻找一个名字叫做 `server-config.json` 的文件

	更多运行参数, 请查阅 `--help`


4. 配置文件

	* local 端配置文件 [local-config.json](./lss/local-config.json.md)
	* server 端配置文件 [server-config.json](./lss/server-config.json.md)


5. `/path/to/lproxy/Makefile` “伪目标”说明：[PHONY_TARGET](./lproxy_Makefile_phony_target.md)

6. Makefile 变量

	* [Makefile Variables](./MakefileVariables.md)

7. 补充说明

   * 在执行 `make init`，释放 `boost` 库源码时，会检测系统是否安装 `7z` 工具，如果检测不到 `7z` `7za` `7zr` 当中的任何一个，则会尝试编译一个 `7zr`；Windows 环境下（`cd path\to\lproxy\contrib\boost; make -f Makefile.win32`）如果在系统中检测不到 `7z` 工具，会直接使用 `path\to\lproxy\tools\7zip\bin.win32\7za.exe.win32`。
   * 如果你想一切都从零开始：配置编译环境、编译 lproxy 以及运行各个组件的 test 等等，那么 [CI 脚本](../.travis.yml) 有可能会帮助到你。
   * ~~`lss` 暂未提供在 Windows/MinGW 环境下编译支持。~~ 现已支持
   * 想要编译一个 Windows/MinGW 平台的 `lproxy` ？ *注意，在 MinGW 上编译 lproxy ，目前还没能做到非常简便的操作* ，可以参考这里 [MinGW-Builds](./mingwbuilds.md) 得到 **Windows 平台的 lproxy** 二进制程序。
   * Android 平台的 local 端项目： [https://github.com/DD-L/lproxy-local-android](https://github.com/DD-L/lproxy-local-android)
   * 想直接获取二进制程序？:point_right: [Releases](#releases)
   * 强烈不推荐使用 Cygwin 平台的 `lproxy`，该平台下的 `lproxy` 的问题较多 （这与 boost.asio 在 Cygwin 上的实现有关）。

## Docker 支持

* `lproxy` 

	1. 获取 `lproxy` Docker 镜像
	
		该镜像只包含必要的 `lproxy` 二进制程序、配置文件 及其 运行环境。
		* docker.io

			see [deel/lproxy](https://hub.docker.com/r/deel/lproxy/)

			```shell
			$ sudo docker pull deel/lproxy
			```
		* daocloud.io

			目前在 daocloud.io 没有公开该镜像的访问控制，如果有需要请参照 [“lproxy 镜像制作参考”](https://github.com/DD-L/lproxy-docker/tree/master/lproxy)

	2. 在容器中运行 `lproxy` 服务
		
		```shell
		$ sudo docker run -d --name lproxy -p 8087-8088:8087-8088 -i deel/lproxy
		```
		* local 端运行示例

			```shell
			$ sudo docker exec -i lproxy ./lsslocal.exe  #--help
			```
		* server 端运行示例

			```shell
			$ sudo docker exec -i lproxy ./lssserver.exe #--help
			```

* `lproxy-dev`
	
	1. 获取 `lproxy-dev` Docker 镜像

		该镜像包含 `lproxy` 的[ lss 分支](https://github.com/DD-L/lproxy/tree/lss)源码 及其完整的 开发、编译、运行调试环境。
		
		* docker.io

			see [deel/lproxy-dev](https://hub.docker.com/r/deel/lproxy-dev/)

			```shell
			$ sudo docker pull deel/lproxy-dev
			```
		* daocloud.io

			目前在 daocloud.io 没有公开该镜像的访问控制，如果有需要请参考 [Dockerfile](https://github.com/DD-L/lproxy-docker/blob/master/dev/Dockerfile)

	2. 运行 lproxy-dev 容器

		参考示例：

		```shell
		$ # 创建并运行一个 临时(--rm) 容器
		$ sudo docker run --rm --name lproxy_dev -p 8087-8088:8087-8088 -it deel/lproxy-dev
		```
		默认情况下，容器的 bash 就位前，会先自动拉取 lproxy 的 lss 分支的源码，以确保容器里拥有最新的源码。之后就可以在容器里开森的编译、调试运行 lproxy 服务了

## Releases

包含 `Docker 镜像` 和 `Windows 平台的二进制程序`。

*`Android 平台的 lproxy-local` 二进制程序请移步 [https://github.com/DD-L/lproxy-local-android/releases](https://github.com/DD-L/lproxy-local-android/releases)*

> **lproxy 版本号命名规范，及各版本间兼容情况说明：[VersionNumber](./VersionNumber.md)**

* **[v0.2.2](https://github.com/DD-L/lproxy/releases/tag/0.2.2)**

	此次修订，除了部分优化外，主要为 Android 平台的移植扫清了底层障碍。Android 项目链接：[lproxy-local-android](https://github.com/DD-L/lproxy-local-android/releases)

	* 优化调用接口，为以后提供 API 做准备。
	* 优化加解密底层库。在 linux 平台下，release 二进制程序总体积（文件大小）被缩减到原来的 20%；满载时 CPU 占用率也大大降低 （*[相关链接](./lss/LoadTesting.md)）*。
	* 修复 Windows 平台的程序在关闭退出时短暂的假死现象。
	* 去除 local 端无必要的重复加密指令。
	* 修复 bug [#166](https://github.com/DD-L/lproxy/issues/166)。
	* *`v0.2.x` 与 `v0.1.x` 互不兼容。*

* **[v0.2.1](https://github.com/DD-L/lproxy/releases/tag/0.2.1)**

	* 优化转发逻辑。在网络欠佳的情况下，减少丢包。
	* 优化 buffer_size。转发数据的 buffer 字节大小，由 `1024` 修订为 `4096`，据说这样可以提升吞吐量。
	* 启用 keepalive 支持。 **issue [#146](https://github.com/DD-L/lproxy/issues/146)**
	* `v0.2.x` 与 `v0.1.x` 互不兼容，*各个版本号之间的兼容规则请在 [VersionNumber](./VersionNumber.md) 中查阅*。

* **[v0.2.0.20160429_Beta](https://github.com/DD-L/lproxy/releases/tag/0.2.0.20160429_Beta)**

	* 优化 socks5 处理，**issue [#127](https://github.com/DD-L/lproxy/issues/127)**。减轻 lproxy-server 端负担，进一步提升 lproxy 并发能力；减少 local 与 server 之间的网络 I/O，提高网络利用率。但是，这种优化，导致其**与之前的 `v0.1.x` 版本的程序互不兼容**，*各个版本号之间的兼容规则请在 [VersionNumber](./VersionNumber.md) 中查阅*。
	* bug 修复，**fixed [#130](https://github.com/DD-L/lproxy/issues/130)**。
	* lproxy-server 端 timeout 优化。
	* 支持 `-v` 和 `--version` 选项。

* **[v0.1.0.20160418_Beta](https://github.com/DD-L/lproxy/releases/tag/0.1.0.20160418_Beta)**

	* 匆匆发布的第一个测试版本

## 即刻体验 `lproxy` 远程代理服务

*这只是一个演示*

预先准备：

1. 本地需要一个 `lproxy` local 端程序：`lsslocal.exe` （[Download](https://github.com/DD-L/lproxy/releases)）。
2. 注册一个 [daocloud.io](https://www.daocloud.io/) 账号。

接下来，只需简单的几个步骤，就能即刻免费体验 `lproxy`：[详尽的图文教程](https://github.com/DD-L/DailyNotes/blob/master/lproxy/demo_on_daocloud/README.md)

## 简易的 `lproxy` server 端集群部署方案

1. 方案一：[ServerClusters1](./ServerClusters1.md)
2. 方案二：*正在完善中...*

## `lproxy` 前进计划

*正在完善中...*

## 开发文档

[`lproxy/src/` 中的所有组件](./develop.md)

| | 简述 |
|---------------------------|:------------------|
| [static_analysis](./develop.md#1-static_analysis) | cpp 代码静态检查 |
| [boost_build](./develop.md#2-boost_build)         | -                |
| [store](./develop.md#3-store)                     | 适用于“多生产者-多消费者” 的模板仓库 |
| [log](./develop.md#4-log)                         | *非常优秀的* 日志库 |
| [exception](./develop.md#5-exception)             | 异常类型组件 |
| [language](./develop.md#6-language)               | 多国语言组件 |
| [deel.boost.python](./develop.md#7-deelboostpython) | - |
| [python](./develop.md#8-python)                   | cpp 调用 python 函数 的简易工具 |
| [logrotate](./develop.md#9-logrotate)             | 日志转储 （对持续膨胀的日志文件进行分片压缩） |
| [workers](./develop.md#10-workers)                | Worker 线程池 |
| [crypto](./develop.md#11-crypto)                  | 加解密工具 |
| [program_options](./develop.md#12-program_options)| 命令行选项工具 |
| [lss](./develop.md#13-lss)                        | lproxy 核心模块 |

## 测试

*待完善*

*Help wanted:  希望得到专业测试帮助*

1. [并发](./lss/StressTesting.md)
2. [负载](./lss/LoadTesting.md)

## TODO list

[TODO](https://github.com/DD-L/lproxy/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3ATODO)