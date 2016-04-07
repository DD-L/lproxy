# lproxy

1. 编译安装 `lproxy`

	```shell
	$ cd /path/to/lproxy
	$ make init
	$ make lss       # 如果是在 Cygwin 上编译，换成 make lss.cygwin
	$ make install   # 程序文件和配置文件会被复制到 /path/to/lproxy/bin 文件夹内
	```
	若要编译 `DEBUG` 版本的 `lss`，对 `make lss` 添加 `DEBUG=enable` 选项即可。


2. 运行 `lproxy` 服务

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


3. 配置文件

	* local 端配置文件 [local-config.json](./lss/local-config.json.md)
	* server 端配置文件 [server-config.json](./lss/server-config.json.md)


4. `/path/to/lproxy/Makefile` “伪目标”说明

	| 伪目标       | 作用       |
	|--------------|------------|
	| `all`        | do nothing |
	| `init`       | 依次执行伪目标 `check` `boost` `cryptopp`|
	| `init.force` | 依次执行伪目标 `check` `boost.force` `cryptopp`|
	| `check`      | 检查编译环境：1.系统是否安装`dos2unix`; 2. 是否支持 C++11|
	| `boost`      | 释放 boost 库源码 （lproxy 是以 boost 源码嵌入的方式完成编译的）|
	| `boost.force`| 强制释放 boost 库源码|
	| `cryptopp`   | 下载并编译 `cryptopp` 静态库|
	| `lss`        | 编译 lss, 执行 `cd /path/to/lproxy/src/core/lss; make -f Makefile`|
	| `lss.cygwin` | 在 Cygwin 环境下编译 lss，执行 `cd /path/to/lproxy/src/core/lss; make -f Makefile.Cygwin`|
	| `lss.clean`  | 执行 `cd /path/to/lproxy/src/core/lss; make clean` |
	| `install`    | 会将编译好的 lss 二进制程序及配置文件拷贝到 `/path/to/lproxy/bin` 目录下|
	| `uninstall`  | 删除安装， 会执行 `$(RM) /path/to/lproxy/bin` |
	| `clean`      | 依次清除 “先前释放的 boost 库源码”，“cryptocpp 源码及其静态库”|


5. 补充说明

   * 在释放 `boost` 库源码时，会检测系统是否安装 `7z`工具，如果检测不到 `7z` `7za` `7zr` 当中的任何一个，则会尝试编译一个 `7zr`; Windows 环境下（`cd path\to\lproxy\contrib\boost; make -f Makefile.win32`）如果在系统中检测不到 `7z` 工具，会直接使用 `path\to\lproxy\tools\7zip\bin.win32\7za.exe.win32`。
   * `lss` 暂未提供在 Windows/MinGW 环境下编译支持。

## Docker 支持

* `lproxy` 

	1. 获取 `lproxy` Docker 镜像
	
		该镜像只包含必要的 `lproxy` 二进制程序、配置文件 及其 运行环境。 **（此部分尚未完成）**
	
	2. 在容器中运行 `lproxy` 服务
	
		**（此部分尚未完成）**

* `lproxy-dev`
	
	1. 获取 `lproxy-dev` Docker 镜像

		该镜像包含 `lproxy` [lss 分支](https://github.com/DD-L/lproxy/tree/lss)源码 及其完整的 开发、编译、调试环境。
		
		* docker.io

			see [deel/lproxy-dev](https://hub.docker.com/r/deel/lproxy-dev/)

			```shell
			$ sudo docker pull deel/lproxy-dev
			```
		* daocloud.io

			目前在 daocloud.io 没有公开该镜像的访问控制，如果有需要请参考 [Dockerfile](https://github.com/DD-L/lproxy/blob/docker-dev/docker/dev/Dockerfile) 

	2. 运行 lproxy-dev 容器

		参考示例：

		```shell
		$ # 创建并运行一个临时容器 --rm
		$ sudo docker run --rm --name lproxy_dev -p 8087-8088:8087-8088 -it deel/lproxy-dev
		```
		容器的 bash 就位前，会先自动拉取 lproxy 的 lss 分支的源码，以确保容器里拥有最新的源码。之后就可以在容器里开森的编译、调试运行 lproxy 服务了


## [`lproxy/src/`](../src) 中的所有组件


### 1. static_analysis

cpp 代码静态检查

* [cppcheck](./static_analysis/cppcheck/readme.md)
* [pclint](./static_analysis/pclint/readme.md)

### 2. store

适用于“多生产者-多消费者” 的模板仓库

1. test
	* [test](../src/core/store/test)
2. 详细文档
	* [Store](./store/store.md)
	* [Store::push & Store::pop 性能测试](../src/core/store/test/testscript/readme.md)

### 3. log

日志库

1. 使用示例
	* [demo](./log/demo.md)
	* [demo2](./log/demo2.md)
	* [lss_log_demo](./log/lss_log_demo.md)
2. 详细文档
	* [log_types](./log/log_types.md)
	* [loglevel](./log/loglevel.md) (以及被 [遗弃](./log/loglevel.old.md) 的版本)
	* [LogStoreInterface](./log/logstoreinterface.md)
	* [LogStore_priority](./log/logstore_priority.md)
	* [LogStore_lockfree](./log/logstore_lockfree.md)
	* [log_tools::priority_queue](./log/priority_queue.md)
	* [LogManager](./log/logmanager.md)
	* [logging](./log/logging.md)
	* [LogOutput_if](./log/logoutput_if.md)
	* [LogOutput](./log/logoutput.md)
	* [LogOutput2](./log/logoutput2.md)
	* [init_simple](./log/init_simple.md)
3. [how-to book](../src/core/log/HOWTO)
	
### 4. exception

异常类型组件

1. 使用示例
	* [example](./except/example.md)
2. 详细文档
	* [exception](./except/except.md)

### 5. language

多国语言组件

1. 使用示例
	* [demo](./language/demo.md)
2. 详细文档
	* [Lang](./language/lang.md)

### 6. deel.boost.python

* see [documents](../src/core/deel.boost.python/README.md)

### 7. python

cpp 调用 python 函数 的简易工具

1. 使用示例
	* [demo](./python/demo.md)
2. 详细文档
	* [pyinit](./python/pyinit.md)
	* [GetPyFunc](./python/getpyfunc.md)
3. [how-to book](../src/core/python/HOWTO)

### 8. logrotate

日志转储 （对持续膨胀的日志文件进行分片压缩）

1. 使用示例
	* [demo](./logrotate/demo.md)
2. 详细文档
	* [Logrotate](./logrotate/logrotate.md)
3. [how-to book](../src/core/logrotate/HOWTO)

### 9. workers

一种资源池实现 (该分支尚未合并, 遗留。代码: [here](https://github.com/DD-L/lproxy/tree/feature-workers/src/core/workers))

### 10. crypto

加解密工具

1. 使用示例
	* [demo](./crypto/demo.md)
2. 详细文档
	* [encryptor](./crypto/encryptor.md)
	* [xor](./crypto/xor.md)
	* [rc4](./crypto/rc4.md)
	* [aes](./crypto/aes.md)
	* [rsa](./crypto/rsa.md)
	* [base64](./crypto/base64.md)
	* [md5](./crypto/md5.md)
	* [异常处理](./crypto/exception.md)
3. [how-to book](../src/core/crypto/HOWTO)

### 11. program_options

命令行选项工具

1. 使用示例
	* [demo](./program_options/demo.md)
2. 详细文档
	* [program_options](./program_options/program_options.md)

### 12. lss

lproxy 核心模块

* see [documents](./lss/README.md)

