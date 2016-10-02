#ifndef LOGWORKERS_H
#define LOGWORKERS_H
/*
 * LogWorkers 类 (工种: 日志输出)
 *
 * 1. 说明: 对日志输出的操作模块, 继承自Workers抽象类，单件实现
 * 2. 原型: class LogWorkers : public Workers<LogWorkers>;
 * 3. 摘要: 
 *		1. 3个私有重写方法:
 *			1. virtual void prepare(); 
 *				进入流程循环之前的处理函数, 用于输出进程信息和标识所在模块
 *			2. virtual void process(void);
 *				一个(次)工作流程
 *			3. virtual void interruption_respond();
 *				对于工作线程中断后的响应函数, 用于将错误信息及所在模块标识
 *				输出到m_interrupt_os. 其中操作m_interrupt_os时使用了LogOut
 *				中的互斥锁，避免线程冲突.
 *		2. 2个公有静态成员:
 *			1. static std::ostream& m_interrupt_os;
 *			处理中断时使用的输出流, 会在interruption_respond()中被使用, 
 *			初始值为std::cout. 可用在GUI编程时对输出流重新赋值以满足需求.
 *	
 *			2. static Logrotate* logrt; // 为NULL时不执行日志转储
 *			处理日志转储所使用的Logrotate对象指针, 初始值为NULL
 *
 * 4. 直接依赖:
 *		1. 日志模块, 包括日志生产和LogOutput
 *		2. Workers抽象类
 *		3. 日志转储模块 Logrotate
 *
 */

///////////////////////////////////////////////////////////////////////
//example:
//
// produce_log(); // some loginfo, logwarning or logerr
// bool ret = LogOut::get_mutable_instance().bind(std::cout);
// cout << boolalpha << ret << endl;
//
// Workers<LogWorkers>& workers = LogWorkers::get_mutable_instance();
// //workers.start(2/*, false*/); // waiting...
// workers.start(3, true);
// boost::this_thread::sleep_for(chrono::milliseconds(50));
// workers.interrupt_all(/*false*/); // waiting...
// //boost::this_thread::sleep_for(chrono::milliseconds(100));
//
//a possible output:
//
// true
// Wed Dec 17 20:27:52 2014    [INFO]: produce log thread[id: 0x20000038] starts [main.cpp:8]
// Wed Dec 17 20:27:52 2014 [WARNING]: test... [main.cpp:10]
// Wed Dec 17 20:27:52 2014 [WARNING]: test... [main.cpp:10]
// Wed Dec 17 20:27:52 2014 [WARNING]: test... [main.cpp:10]
// Wed Dec 17 20:27:52 2014 [WARNING]: test... [main.cpp:10]
// Wed Dec 17 20:27:52 2014 [WARNING]: test... [main.cpp:10]
// Wed Dec 17 20:27:52 2014    [INFO]: Now start a Workers thread(id:0x20014ce0, module:LogWorkers)! [logworkers.cpp:14]
// Wed Dec 17 20:27:52 2014    [INFO]: Now start a Workers thread(id:0x20015258, module:LogWorkers)! [logworkers.cpp:14]
// Wed Dec 17 20:27:52 2014    [INFO]: Now start a Workers thread(id:0x200155d8, module:LogWorkers)! [logworkers.cpp:14]
// Wed Dec 17 20:27:52 2014 [WARNING]: Workers thread(id:0x20015258, module:LogWorkers) is interrupted! [logworkers.cpp:30]
// Wed Dec 17 20:27:52 2014 [WARNING]: Workers thread(id:0x20014ce0, module:LogWorkers) is interrupted! [logworkers.cpp:30]
// Wed Dec 17 20:27:52 2014 [WARNING]: Workers thread(id:0x200155d8, module:LogWorkers) is interrupted! [logworkers.cpp:30]
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include "workers/workers.h"
//#include "log/logoutput.h" // is used in the function process() 
#include "logrotate/logrotate.h"
#include "log/init_simple.h"

class LogWorkers : public Workers<LogWorkers> {
	public:
		/* 处理中断时使用的输出流, 初始值为std::cout */
		static std::ostream& m_interrupt_os; 
		/* 处理日志转储所使用的Logrotate对象, 初始值为NULL */
		static Logrotate* logrt; // 为NULL时不执行日志转储
	private:
		// 进入流程循环之前的处理函数
		virtual void prepare();
		// 一个工作流程
		virtual void process(void);
		// 对于线程中断后的响应函数
		virtual void interruption_respond();
		LogWorkers& operator=(const LogWorkers&) = delete;
    private:
        std::shared_ptr<LogVal> val;
};
#endif // LOGWORKERS_H
