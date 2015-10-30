/*
 *	日志转储 Logrotate [C++ 部分]
 *
 *	1. 特点: 对运行日志压缩分片, 支持同时绑定多个ofstream(日志)对象
 *	2. 原型: class Logrotate;
 *	3. 方法摘要: 
 *		1. 构造析构和拷贝.
 *			Logrotate() = default;			
 *			Logrotate(const Logrotate&) = delete;
 *			Logrotate& operator= (const Logrotate&) = delete;
 *			virtual ~Logrotate() {}
 *		2. ofstream对象 和 日志文件(名) 的绑定.
 *			bool bind(std::ofstream& logfile, const std::string& filename, filesize_t maxsize);
 *			如果绑定成功返回true, 否则返回false. 
 *			例如绑定时文件被占用或当前要绑定的ofstream之前未close(), 都将引发绑定失败.
 *			此方法将代替ofstream::open()函数.
 *			参数一: 要绑定的ofstream对象;
 *			参数二: 要绑定的日志文件名;
 *			参数三: 允许日志文件最大尺寸, 单位byte. 此参数构成触发压缩分片的条件.
 *		3. 日志转储执行函数, 每执行一次就尝试压缩分片一次, 此方法依赖python部分.
 *			void action(void);
 *		4. 两个静态成员变量:
 *			1. static std::string pyfilename;	// = "./logrotate.py"					
 *				依赖的python程序, 默认为"./logrotate.py"
 *			2. static std::string pyfunc;		// = "logrotate"
 *				在python程序中依赖的函数, 默认为"logrotate"
 *		5. [私有成员] 压缩分片方法.
 *			bool rotate(const std::string& filename);
 *	4. 直接依赖:
 *		1. GetPyFunc 类
 *		2. 日志转储的python部分
 *	5. python 部分见 logrotate.py
 */

//example:
//
//#include "logrotate.h"
//void foo() {
//	std::ofstream log1, log2;
//	Logrotate lrt;
//	// ofstream object('log') bind log file('./example.log') together,
//	// and set the maximum allowable size of the log file.
//	bool ret1 = lrt.bind(log1, "./example1.log", 20 * 1024 * 1024/* 20MB */);
//	bool ret2 = lrt.bind(log2, "./example2.log", 20 * 1024 * 1024/* 20MB */);
//	// one compression slice. Generally, it is used in the loop.
//	for (int i = 0; i < 10; ++i) {
//		lrt.action();
//	}
//}
// 
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

 


#ifndef LOGROTATE_H
#define LOGROTATE_H

#include <iostream>
#include <fstream>
#include <map>

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

#endif // LOGROATE_H
