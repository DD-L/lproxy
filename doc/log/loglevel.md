log/loglevel.hpp

------------------------------

旧的实现文档在这里[here](./loglevel.old.md), 它未完成就被丢弃了。

这里，新的实现比 loglevel.h.old 要优秀，但也失去了一些乐趣。

####它原本是为了“用来代替enum”而设计的，但事实上，它更适合做的工作是“在全局域或函数局部域里，安全的定义和使用'全局唯一常量'”.

---------------------------

# MAKE_LOGLEVEL(Level, Rank) 和 makelevel(Level)

* MAKE_LOGLEVEL 定义全局唯一"常量"
* makelevel “常量”修饰符

enum被用来定义一组常量，但是一旦一个enum类型被定义，以后它就不能再随意的增加常量了。

<pre>
enum EnumType {AA = 0, BB, CC};
// 被定义了一组常量 AA BB CC
// 但是，如果再想追加定义一组EnumType类型的常量: enum EnumType {DD, EE};, 就不可能实现了.
</pre>

然而，log/loglevel.hpp 巧妙的绕开了这个限制:
<pre>
//比如原先的代码中是这样在全局域里定义日志级别常量的:
enum LogType { 
	TRACE = 0,  DEBUG = 10, 
	INFO  = 20,  WARN = 30, 
	ERROR = 40, FATAL = 50
};
LogType logtype = INFO;

// 如果改用log/loglevel.h的设计可以这样来拆分定义:
// 在全局域或函数局部域中定义6个“全局唯一常量”：
MAKE_LOGLEVEL(TRACE,  0); // TRACE 权重为0
MAKE_LOGLEVEL(DEBUG, 10); // DEBUG 权重为10
MAKE_LOGLEVEL(INFO , 20); // INFO  权重为20
MAKE_LOGLEVEL(WARN , 30); // WARN  权重为30
MAKE_LOGLEVEL(ERROR, 40); // ERROR 权重为40
MAKE_LOGLEVEL(FATAL, 50); // FATAL 权重为50

// 如果未来你想增加几个自定义日志级别，在任何函数域或全局域可以这样写
MAKE_LOGLEVEL(MyLevel1, 25);
MAKE_LOGLEVEL(MyLevel2, 25);    // 针对当前的日志级别设计，权重特意被设置成可以重复的值
//MAKE_LOGLEVEL(MyLevel1, 100); // 错误：
// 1. 如果出现在不同作用域，则编译器在编译器不会报错。
//    但在程序运行时会抛出重复定义常量MyLevel1的异常信息，异常信息包含错误定义所在的文件及行号:
//    比如： Log Exception: Attempts to re-define an existing global constant - 'MyLevel1' [test_loglevel.cpp:27]
// 2. 如果出现在相同作用域，则会直接在编译期报"声明冲突"的错误信息，导致无法通过编译.

// 这样就又增加了两个“全局唯一常量”：MyLevel1， MyLevel2


// 如何使用这些常量呢？这里要付出一点小代价
// LogType logtype = INFO; 之前这样的写法要换成下面这个:
LogType logtype = makelevel(INFO);

// 也就是说，以后凡是出现通过MAKE_LOGLEVEL的定义的"常量"，都必须加上makelevel(常量)来"转义"修饰。
// 凡是之前使用enum定义的常量出现的地方，都得改成makelevel(...)的写法。

// 如果试图给“常量”赋值，则会直接在编译器编译它时报错
// makelevel(INFO) = makelevel(WARN); // 编译报错

// 如果试图修饰一个之前从未定义过的常量，则程序运行时会抛出一个异常信息。
// makelevel(non_exist_const); // non_exist_const 之前从未被定义，会抛异常：
// 异常信息包含，错误所在文件及行号，
// 比如：Log Exception: Attempts to reference a non-existent global constant - 'non_exist_const' [test_loglevel.cpp:27]

</pre>

MAKE_LOGLEVEL() 和 makelevel() 的使用方式，借鉴了std::make_shared<>() 和 std::shared_ptr<>() 的使用方式。

如同std::shared_ptr和std::make_shared的关系一样, makelevel()是“全局唯一常量”修饰函数，MAKE_LOGLEVEL() 是“全局唯一常量”的工厂函数。

---------

# MAKE_LOGLEVEL_INSIDE(Level, Rank)

其实完全只有 MAKE_LOGLEVEL(Level, Rank) 的存在就能正常工作了。

但是对于有强迫症的使用者来说，MAKE_LOGLEVEL是不足的，因为如果MAKE_LOGLEVEL(Level, Rank)如果出现在函数域(局部域)，则编译器会报类似 “variable '...' set but not used [-Wunused-but-set-variable]” 的警告信息。

如果要消除这警告信息，就要在局部域(函数里)，使用MAKE_LOGLEVEL_INSIDE来代替MAKE_LOGLEVEL。他们做的工作是一样的，但是MAKE_LOGLEVEL_INSIDE不能被使用在全局域中，编译器会直接报错，导致编译无法通过。


这里总结下 它们 的使用场景：
<pre>
1. MAKE_LOGLEVEL(Level, Rank) 
	可以在全局域中使用; 也可以在函数里面使用，但是编译器会给出一个警告。
2. MAKE_LOGLEVEL_INSIDE(Level, Rank) 
	只能在函数里面使用（可以在任何函数中使用），在全局域中使用的话，会导致编译通不过。
	/*因为C++不允许在全局域调用函数，只能声明和初始化。*/
</pre>

有关 MAKE_LOGLEVEL 和 MAKE_LOGLEVEL_INSIDE 以及 makelevel 的更详细的补充说明，见本页面的最底部。

---------

### 详细的使用方法，见 src/log/test/test_loglevel.cpp

---------

#LogType

日志类型

LogType伪装成一个enum类型, 可以构造一个被 makelevel()修饰过的"常量"。

<pre>
// 类摘要
class LogType {
public:
	// 构造
	LogType(const Log_LevelBase& loglevel);
	LogType(const LogType& that);

	// 功能函数
	const char* get_name() const; // 获取当前LogType对象的保存的常量名
	// 拷贝赋值
	const LogType& operator=(const LogType& that);

	// 重载比较运算符
	friend bool operator==(const LogType& l, const LogType& r);
	friend bool operator!=(const LogType& l, const LogType& r);
	friend bool operator>(const LogType& l, const LogType& r);
	friend bool operator<(const LogType& l, const LogType& r);
	// 这里要特别注意：
	// == 和 != 只判断常量的名字是否相同;
	// > 和 < 只比较常量的权重大小.
	
	// 支持流输出操作
	friend ostream& operator<< (ostream& os, const LogType& lt);
};
</pre>

-----------

# Log_LevelBase

用户无需关心的 用来定义日志级别的类

<pre>
// 类摘要:
class Log_LevelBase {
public:
	// 构造、析构
	virtual ~Log_LevelBase();
	Log_LevelBase(const char* _name = "NONELEVEL", const int& _rank = -1);
	// 为了能够被放入stl容器，拷贝构造不可缺失
	Log_LevelBase(const Log_LevelBase&);
	Log_LevelBase(Log_LevelBase&&);
	

	// 获取“常量”名
	const char* get_name() const;
	// 获取“常量” 权重
	inline const int get_rank() const;

	// 比较运算符
	friend bool operator== (const Log_LevelBase& l, const Log_LevelBase& r);
	friend bool operator!= (const Log_LevelBase& l, const Log_LevelBase& r);
	friend bool operator> (const Log_LevelBase& l, const Log_LevelBase& r);
	friend bool operator< (const Log_LevelBase& l, const Log_LevelBase& r);
	// 这里要特别注意：(同LogType, 因为LogType就是调用的这里)
	// == 和 != 只判断常量的名字是否相同;
	// > 和 < 只比较常量的权重大小.

	// 支持流输出操作
	friend ostream& operator<< (ostream& os, const Log_LevelBase& ll);
	friend ostream& operator<< (ostream& os, Log_LevelBase&& ll);
};
</pre>

------

# LogLevelManage

用户无需关心的日志级别的管理类，所有日志级别唯一"常量", 都是由它来管理。

<pre>
// 类摘要
class LogLevelManage {
public:
	// 构造, 线程安全的
	LogLevelManage(Log_LevelBase&& llb, const char* filename, const int& linenum);

	// 依据给定的“常量”的名字，来返回相应的"常量". （查找）
	static const Log_LevelBase& 
	get_level(const char* level, const char* filename, const int& linenum);
	// 如果查找不到，则会抛出一个“试图引用一个未定义的常量”的异常信息

	// 什么都不做，只是为了消除某一编译警告
	// 它仅仅被用在MAKE_LOGLEVEL_INSIDE的内部实现中
	void clearwarning(void) {}

};

</pre>



------

# MAKE_LOGLEVEL(Level, Rank)

"全局唯一常量"的工厂函数,它是线程安全的.

底层实现用户无需关心，但这里还是给出定义, 以便帮助想有更多了解的人:

<pre>
#define MAKE_LOGLEVEL(Level, Rank)\
	auto&& __tmp_var_LogLevelManage_##Level \
		=  LogLevelManage(Log_LevelBase(#Level, Rank), __FILE__, __LINE__);

</pre>

其实所谓的“常量”，只不过是被 MAKE_LOGLEVEL 处理过的 类常对象而已。

一旦一个“常量”被定义，则这个"常量"将被交给 LogLevelManage 管理。

-------
# MAKE_LOGLEVEL_INSIDE(Level, Rank)

用户无需关注它的实现，但这里还是给出定义, 以便帮助想有更多了解的人:
<pre>
// MAKE_LOGLEVEL 函数(局部)域版本， 同样也是线程安全的
#define MAKE_LOGLEVEL_INSIDE(Level, Rank)\
	MAKE_LOGLEVEL(Level, Rank)\
	__tmp_var_LogLevelManage_##Level.clearwarning();
</pre>


--------

# makelevel(Level)

"全局唯一常量"修饰符

用户无需关心实现，但这里还是给出定义, 以便帮助想有更多了解的人:

<pre>
#define makelevel(Level) \
	LogLevelManage::get_level(#Level, __FILE__, __LINE__)
</pre>

其实它只是，返回之前交给LogLevelManage管理的“常量”对象的常引用.

--------
