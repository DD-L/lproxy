# lss

* [lss 包结构介绍](./pack.md)

* lss 基本架构 及其 session 大致流程示意图.

	* lproxy v0.2.1

		* [svg](./lproxy_v0.2.1.svg)

		![lproxy](./lproxy_v0.2.1.png)

	* lproxy v0.2.0

		* [svg](./lproxy_v0.2.0.svg)

		* [lproxy](./lproxy_v0.2.0.png)
	
	* lproxy v0.1.x

		* [svg](./lproxy_v0.1.x.svg)
		* [png](./lproxy_v0.1.x.png)

* [压力(并发)测试](./StressTesting.md)

* 追加测试: [负载测试](./LoadTesting.md)

* 编译 lss

	1. 在编译 lss 之前，请先确保已经在 lproxy 的根目录执行了 `make init`.

		```shell
		$ cd /path/to/lproxy/ && make init
		```

	2. 在 linux 上编译示例：

		```shell
		$ cd /path/to/lproxy/src/core/lss
		$ make
		```

	3. 在 Cygwin 上编译示例：

		```shell
		$ cd /path/to/lproxy/src/core/lss
		$ make -f Makefile.Cygwin
		```

	4. 编译 `DEBUG` 版本的 lss

		```shell
		$ # 添加 DEBUG=enable 选项即可
		$ cd /path/to/lproxy/src/core/lss
		$ make DEBUG=enable        # -f Makefle.Cygwin
		```
* [how-to book](../../src/core/lss/HOWTO)