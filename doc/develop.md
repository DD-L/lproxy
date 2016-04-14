## [`lproxy/src/`](../src) 中的所有组件

### 1. static_analysis

cpp 代码静态检查

* [cppcheck](./static_analysis/cppcheck/readme.md)
* [pclint](./static_analysis/pclint/readme.md)

### 2. boost_build

lproxy 是以 boost 源码嵌入的方式完成编译的，`boost_build` 中包含了必须事先编译才能使用 boost 库组件，这些 boost 组件为 lproxy 所用。

### 3. store

适用于“多生产者-多消费者” 的模板仓库

1. test
	* [test](../src/core/store/test)
2. 详细文档
	* [Store](./store/store.md)
	* [Store::push & Store::pop 性能测试](../src/core/store/test/testscript/readme.md)

### 4. log

日志库

1. [特性](./log/feature.md)
2. 使用示例
	* [demo](./log/demo.md)
	* [demo2](./log/demo2.md)
	* [lss_log_demo](./log/lss_log_demo.md)
3. 详细文档
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
4. [how-to book](../src/core/log/HOWTO)

### 5. exception

异常类型组件

1. 使用示例
	* [example](./except/example.md)
2. 详细文档
	* [exception](./except/except.md)

### 6. language

多国语言组件

1. 使用示例
	* [demo](./language/demo.md)
2. 详细文档
	* [Lang](./language/lang.md)

### 7. deel.boost.python

* see [documents](../src/core/deel.boost.python/README.md)

### 8. python

cpp 调用 python 函数 的简易工具

1. 使用示例
	* [demo](./python/demo.md)
2. 详细文档
	* [pyinit](./python/pyinit.md)
	* [GetPyFunc](./python/getpyfunc.md)
3. [how-to book](../src/core/python/HOWTO)

### 9. logrotate

日志转储 （对持续膨胀的日志文件进行分片压缩）

1. 使用示例
	* [demo](./logrotate/demo.md)
2. 详细文档
	* [Logrotate](./logrotate/logrotate.md)
3. [how-to book](../src/core/logrotate/HOWTO)

### 10. workers

Worker 线程池 (该分支尚未合并, 遗留。代码: [here](https://github.com/DD-L/lproxy/tree/feature-workers/src/core/workers))

### 11. crypto

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

### 12. program_options

命令行选项工具

1. 使用示例
	* [demo](./program_options/demo.md)
2. 详细文档
	* [program_options](./program_options/program_options.md)

### 13. lss

lproxy 核心模块

* see [documents](./lss/README.md)