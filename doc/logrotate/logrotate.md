* logrotate/logrotate.h

* logrotate/logrotate.py

------

# Logrotate

对程序运行生成的日志文件进行分片压缩的工具, 支持同时绑定多个 `std::ofstream` (日志文件流)对象。

此工具依赖

1. boost.python
2. python 2.7.x

### 日志转储 C++ 部分

```cpp
class Logrotate;
```
直接依赖:

1. 类 `GetPyFunc`
2. 日志转储的 python 部分

##### Logrotate  类摘要

```cpp
class Logrotate {
	public:
		typedef std::streampos	filesize_t; /* byte */
	public:
		Logrotate() = default;			
		Logrotate(const Logrotate&) = delete;
		Logrotate& operator= (const Logrotate&) = delete;
		virtual ~Logrotate() {}

		// 如果绑定失败会返回false.
		// 例如绑定时文件被占用或当前ofstream之前未close(), 都将引发绑定失败.
		bool bind(std::ofstream& logfile, const std::string& filename, filesize_t maxsize/* byte */);
	
		// 日志转储执行(操作)函数
		void action(void);

	public:
		static std::string pyfilename;	// = "./logrotate.py"
		static std::string pyfunc;		// = "logrotate"

	private:
		bool rotate(const std::string& filename);

	private:
		typedef std::pair<std::string, filesize_t>	value_t;
		std::map<std::ofstream*, value_t>			logm;
};
```
##### 一些说明

* `bool bind(std::ofstream& logfile, const std::string& filename, filesize_t maxsize/* byte */);`

	功能： 完成 std::ofstream 对象 和 日志文件(名) 的绑定. 
	1. 如果绑定成功返回 true, 否则返回 false. 如绑定时文件被占用或当前要绑定的 std::ofstream 之前未 `close()`, 都将引发绑定失败.
	 
	2. 此方法内置 `ofstream::open()` 函数.
	3. 参数说明
		1. 参数一 `logfile`:  要绑定的ofstream对象;
		2. 参数二 `filename`: 要绑定的日志文件名;
		3. 参数三 `maxsize`:  允许日志文件最大尺寸, 单位 byte. 此参数构成触发压缩分片的条件.

* `void action(void);`
	
	日志转储执行函数，每执行一次就尝试压缩分片一次, 此方法依赖 python 代码部分.

* 两个静态成员变量

	1. `static std::string pyfilename;	/* = "./logrotate.py" */`
	
		依赖的 python 程序, 默认为"./logrotate.py"

	2. `static std::string pyfunc;		/* = "logrotate" */`
		
		在 python 程序中依赖的函数, 默认为 `logrotate`


### 日志转储 python 部分

1. 方法摘要:

   1. `read_gz_file(path)` 读取 gz 文件内容
   2. `write_gz_file(path, content)` 创建 gz 文件
   3. `read_txt_write_gz(tpath, gzpath)` 读取 text 文件并创建 gz 文件
   4. `logrotate(filename)` 暴露给 C 程序的函数, 参数为日志文件名

2. 详细代码 [here](../../src/core/logrotate/logrotate.py)

3. 遗留部分:

   feature：只保留最近 N 天的日志分片。 由于开发进度要求，此版本暂不实现此 feature, 故作遗留. 


### 简单代码示例

```cpp
//example:

#include <logrotate/logrotate.h>
void foo() {
	std::ofstream log1, log2;
	Logrotate lrt;
	// std::ofstream object('log') bind log file('./example.log') together,
	// and set the maximum allowable size of the log file.
	bool ret1 = lrt.bind(log1, "./example1.log", 20 * 1024 * 1024/* 20MB */);
	bool ret2 = lrt.bind(log2, "./example2.log", 20 * 1024 * 1024/* 20MB */);
	// one compression slice. Generally, it is used in the loop.
	for (int i = 0; i < 10; ++i) {
		lrt.action();
	}
}

//// 则生成压缩分片文件名的格式如下所示:
//example1.log.2014-12-24_00-37-33.gz
//example1.log.2014-12-24_00-37-34.gz
//example1.log.2014-12-24_00-37-34_01.gz
//example1.log.2014-12-24_00-37-34_02.gz
//example1.log.2014-12-24_00-37-34_03.gz
//example1.log.2014-12-24_00-37-34_04.gz
//example1.log.2014-12-24_00-37-35.gz
//example1.log.2014-12-24_00-37-35_01.gz
//example1.log.2014-12-24_00-37-35_02.gz
//example1.log.2014-12-24_00-38-51.gz
//example2.log.2014-12-24_00-37-33.gz
//example2.log.2014-12-24_00-37-34.gz
//example2.log.2014-12-24_00-37-34_01.gz
//example2.log.2014-12-24_00-37-35.gz
//example2.log.2014-12-24_00-37-35_01.gz
//example2.log.2014-12-24_00-37-35_02.gz
//example2.log.2014-12-24_00-38-51.gz
//
```



   