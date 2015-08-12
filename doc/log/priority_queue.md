class template
#log_tools::priority_queue

template <typename value_type, typename PriorityFactor = LogType>
class priority_queue;

## priority_queue

std::priority_queue的堆排序算法不符合日志库设计需求，这里特别为日志库定制的mini版的优先队列 log_tools::priority_queue.已实现为泛型模板, 以便其它用途.


比如，针对当前日志库设计：
<pre>
优先成员指针mp 这样定义:
LogType LogVal::* mp = &LogVal::log_type;

优先因子vfactor这样定义:
LogType f[2] = {FATAL, ERROR}; 
std::vector<LogType> vfactor(f, f + 2);

若日志插入顺序是：
{1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}
则底层容器存储的顺序和插入的顺序相同:
{1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}
则日志弹出的顺序则是:
{4, FATAL} {5, FATAL} {3, ERROR} {6, ERROR} {1, INFO} {2, WARN}

同样的, 如果优先因子vfactor在定义时只有一个元素: FATAL
则日志弹出的顺序为:
{4, FATAL} {5, FATAL} {1, INFO} {2, WARN} {3, ERROR} {6, ERROR}

如果优先因子为空, 则日志弹出顺序和容器的元素存储顺序相同:
{1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}

</pre>

有关 LogType 和 LogVal，请移步[here](./log_types.md)

##log_tools::priority_queue类摘要

<pre><code>
template < typename value_type, typename PriorityFactor >
class priority_queue {
public:
	// init/settings
	static void settings(
			// value_type类成员指针
			PriorityFactor value_type::* __value_mp,
			// 优先因子(们) 
			const std::vector<PriorityFactor>& __vfactor 
	) ;

	// push
	void push(const value_type& __x);
	void push(value_type&& __x);

	// pop
	void pop();

	// top
	value_type& top();
	const value_type& top() const;
	// front, call top()， just for Store
	value_type& front() { return top(); }
	const value_type& front() const { return top(); }

	// empty/size
	bool empty() const;
	size_t size();

	//  constructor/destructor
	priority_queue(void);
	virtual ~priority_queue();
</code></pre>

## Example

* 准备 
	<pre>
	#include < iostream >
	#include "log_types.h"      // src/log/log_types.h
	#include "priority_queue.h" // src/log/priority_queue.h
	#include "store.h"          // src/store/store.h
	typedef log_tools::priority_queue<LogVal, LogType> LogQueue;
	</pre>

* 初始化优先因子
	1. 优先因子为空, 输出顺序同输入的顺序，FIFO
		<pre>
		std::vector<LogType> vfactor;
		//assert(vfactor.empty());
		</pre>
	2. 优先因子元素只有一个: FATAL, 即优先输出FATAL
		<pre>
		LogType factors[1] = {FATAL};
		std::vector<LogType> vfactor(factors, factors + 1);
		</pre>
	3. 优先因子元素有2个: FATAL 和 ERROR，且 FATAL的优先级大于ERROR, 即优先输出FATAL，然后再优先输出ERROR
		<pre>
		LogType factors[2] = {FATAL, ERROR};
		std::vector<LogType> vfactor(factors, factors + 2);
		</pre>
	4. ...

* Then
	<pre>
	// settings
	LogQueue::settings(&LogVal::log_type, vfactor);
	
	typedef Store< LogVal, LogQueue > LogStore;
	LogStore& logstore = LogStore::get_mutable_instance();
	
	logstore.push({ 
			log_tools::local_time(), INFO, "1", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({
			log_tools::local_time(), ERROR, "2", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), FATAL, "3", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), WARN, "4", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), ERROR, "5", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	
	
	// output
	LogVal val;
	for (int i = 0; i < 5; ++i) {
		logstore.pop(val);
		std::cout << log_tools::time2string(val.now)
			<< " [" << log_tools::logtype2string(val.log_type) 
			<< "] " << val.msg << " [pid:" 
			<< val.pid << "] [FUNC:" << val.func_name << "] " 
			<< val.file_name << ":" << val.line_num << std::endl;
	}
	</pre>

* 则针对以上3列举的3种初始化优先因子的方式的输出结果：
	1. 优先因子为空
		<pre>
		2015-Aug-12 23:51:39.325037 [INFO ] 1 [pid:0x20000038] [FUNC:test] demo.cpp:38
		2015-Aug-12 23:51:39.326037 [ERROR] 2 [pid:0x20000038] [FUNC:test] demo.cpp:43
		2015-Aug-12 23:51:39.326037 [FATAL] 3 [pid:0x20000038] [FUNC:test] demo.cpp:48
		2015-Aug-12 23:51:39.326037 [WARN ] 4 [pid:0x20000038] [FUNC:test] demo.cpp:53
		2015-Aug-12 23:51:39.326037 [ERROR] 5 [pid:0x20000038] [FUNC:test] demo.cpp:58
		</pre>
	2. 优先因子元素只有一个: FATAL
		<pre>
		2015-Aug-12 23:51:39.326037 [FATAL] 3 [pid:0x20000038] [FUNC:test] demo.cpp:48
		2015-Aug-12 23:51:39.325037 [INFO ] 1 [pid:0x20000038] [FUNC:test] demo.cpp:38
		2015-Aug-12 23:51:39.326037 [ERROR] 2 [pid:0x20000038] [FUNC:test] demo.cpp:43
		2015-Aug-12 23:51:39.326037 [WARN ] 4 [pid:0x20000038] [FUNC:test] demo.cpp:53
		2015-Aug-12 23:51:39.326037 [ERROR] 5 [pid:0x20000038] [FUNC:test] demo.cpp:58
		</pre>
	3. 优先因子元素有2个: FATAL 和 ERROR
		<pre>
		2015-Aug-12 23:51:39.326037 [FATAL] 3 [pid:0x20000038] [FUNC:test] demo.cpp:48
		2015-Aug-12 23:51:39.326037 [ERROR] 2 [pid:0x20000038] [FUNC:test] demo.cpp:43
		2015-Aug-12 23:51:39.326037 [ERROR] 5 [pid:0x20000038] [FUNC:test] demo.cpp:58
		2015-Aug-12 23:51:39.325037 [INFO ] 1 [pid:0x20000038] [FUNC:test] demo.cpp:38
		2015-Aug-12 23:51:39.326037 [WARN ] 4 [pid:0x20000038] [FUNC:test] demo.cpp:53
		</pre>


-------------------------------------

##### ** 关于log_tools::priority_queue底层容器的说明

底层容器如果像std::priority_queue一样选用std::vector,则必须在插入时就已经做好规定的排序，才能降低push/pop综合成本,但这增加了push的时间复杂度,而push通常和使用者的主业务密切相关。

所以底层容器改用std::list. 有相关资料显示，与vector相比,list在尾部插入简单类型数据，只有大概到了10,000 ~ 100,000级别时，才会显现出差别; 而遍历元素出现劣势的情况则大概到了10,000,000级别.在日志模型中这足够使用了, 并且list从中间移除元素效率远大于其他容器.







