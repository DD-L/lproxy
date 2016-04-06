log/loglevel.old.h

------------------------------


### **此源码虽还未彻底完成就已被遗弃，因为有更高效更安全的实现，但作为一种新的尝试方法，保留了下来，以便日后查阅。*

### 新的实现文档在这里 [here](./loglevel.md)

---------------------------

# MAKE_LOGLEVEL(Level, Rank) 和 makelevel(Level)
#### ---  `enum` 的增强版实现

enum被用来定义一组常量，但是一旦一个enum类型被定义，以后它就不能再随意的增加常量了。

```cpp
enum EnumType {AA = 0, BB, CC};
// 被定义了一组常量 AA BB CC
// 但是，如果再想追加定义EnumType类型的常量 DD EE，就不可能实现了
```

然而，log/loglevel.h 中巧妙的绕开了这个限制:

```cpp
//比如原先的代码中是这样定义日志级别常量的:
enum LogType { 
	TRACE = 0,  DEBUG = 10, 
	INFO  = 20,  WARN = 30, 
	ERROR = 40, FATAL = 50
};
LogType logtype = INFO;

// 如果改用log/loglevel.h的设计可以这样来拆分定义:
MAKE_LOGLEVEL(TRACE,  0); // TRACE 权重为0
MAKE_LOGLEVEL(DEBUG, 10); // DEBUG 权重为10
MAKE_LOGLEVEL(INFO , 20); // INFO  权重为20
MAKE_LOGLEVEL(WARN , 30); // WARN  权重为30
MAKE_LOGLEVEL(ERROR, 40); // ERROR 权重为40
MAKE_LOGLEVEL(FATAL, 50); // FATAL 权重为50

// 如果未来你想增加几个自定义日志级别，在任何函数域或全局域可以这样写
MAKE_LOGLEVEL(MyLevel1, 25);
MAKE_LOGLEVEL(MyLevel2, 25);   // 针对当前的日志级别设计，权重特意被设置成可以重复的值
//MAKE_LOGLEVEL(MyLevel1, 25); // 错误，重复定义常量MyLevel1，无法通过编译

// 这样就又增加了两个常量MyLevel1， MyLevel2

// 如何使用这些常量呢？这里要付出一点小代价
// LogType logtype = INFO; 之前这样的写法要换成下面这个:
LogType logtype = makelevel(INFO);

// 也就是说，以后凡是出现通过MAKE_LOGLEVEL的定义的"常量"，都必须加上makelevel(常量)来"转义"；
// 凡是之前使用enum定义的常量出现的地方，都得改成makelevel(...)的写法。
```

MAKE_LOGLEVEL() 和 makelevel() 的使用方式，借鉴了std::make_shared<>() 和 std::shared_ptr<>() 的使用方式。

如同std::make_shared和std::shared_ptr的关系一样, MAKE_LOGLEVEL() 是一个makelevel()的工厂(宏)函数。

log/loglevel.h 实现了可以动态的无限的增加enum常量字段。

---------

# MAKE_LOGLEVEL_INSIDE(Level, Rank)

其实完全只有 MAKE_LOGLEVEL(Level, Rank) 的存在就能正常工作了。

但是对于有强迫症的使用者来说，MAKE_LOGLEVEL是不足的，因为如果MAKE_LOGLEVEL(Level, Rank)如果出现在函数域(局部域)，则编译器会报类似 “variable '...' set but not used [-Wunused-but-set-variable]” 的警告信息。

如果要消除这警告信息，就要在局部域(函数里)，使用MAKE_LOGLEVEL_INSIDE来代替MAKE_LOGLEVEL。他们的工作是一样的，但是MAKE_LOGLEVEL_INSIDE不能被使用在全局域中，编译器会直接报错，导致编译无法通过。


这里总结下 它们 的使用场景：
<pre>
1. MAKE_LOGLEVEL(Level, Rank) 
	可以在全局域中使用; 也可以在函数里面使用，但是编译器会给出一个警告。
2. MAKE_LOGLEVEL_INSIDE(Level, Rank) 
	只能在函数里面使用（可以在任何函数中使用），在全局域中使用的话，会导致编译通不过。
	因为C++不允许在全局域调用函数，只能声明和初始化。
</pre>

有关 MAKE_LOGLEVEL 和 MAKE_LOGLEVEL_INSIDE 以及 makelevel 的更详细的补充说明，见本页面的最底部。

---------

# LogType

日志类型

LogType伪装成一个enum类型, 可以构造成被 makelevel()处理过的"常量"。

```cpp
// 类摘要
class LogType {
public:
	// 构造
	LogType(const Log_LevelBase& loglevel);
	LogType(std::shared_ptr< Log_LevelBase > loglevel);
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

	// 什么都不做，只是为了消除编译的某一警告。
	// 它仅仅被用在MAKE_LOGLEVEL_INSIDE的内部实现中
	void clearwarning(void);
};
```

-----------

# Log_LevelBase

用户无需关心的定义日志级别的基类

```cpp
// 类摘要:
class Log_LevelBase {
public:
	// 获取“常量”名, 默认值为"NONELEVEL"
	virtual const char* get_name() const { return "NONELEVEL"; };
	// 获取“常量” 权重, 默认值为-1
	virtual int get_rank() const { return -1; };

	Log_LevelBase() {}
	virtual ~Log_LevelBase() {}

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
	friend ostream& operator<< (ostream& os, std::shared_ptr< Log_LevelBase > llp);
	friend ostream& operator<< (ostream& os, std::shared_ptr< Log_LevelBase const > cllp);
	

};
```

------

# LogLevelManage

用户无需关心的日志级别的管理类，所有日志级别唯一"常量", 都是由它来管理。

```cpp
// 类摘要
class LogLevelManage {
public:
	// 构造
	LogLevelManage(std::shared_ptr< Log_LevelBase > llp);
	// 获取内部管理的引用 (这个接口可能是多余的;并且破坏了类的封装。慎用)
	static const std::set< std::shared_ptr< Log_LevelBase > >& levels(void);
	// 依据给定的“常量”的名字，来返回相应的"常量". （查找）
	static const Log_LevelBase& get_level(const char* level);
	// 依据给定的“常量”的名字，来返回相应的"常量"的智能指针. （查找）
	static const std::shared_ptr< Log_LevelBase > get_level_ptr(const char* level);

};
```



------

# MAKE_LOGLEVEL(Level, Rank)

"常量"的工厂(宏)函数.

底层实现用户无需关心，但这里还是给出定义, 以便帮助想有更多了解的人:

```cpp
#define MAKE_LOGLEVEL(Level, Rank)\
	class Log_##Level : public Log_LevelBase {\
	public: \
		Log_##Level(const char* levelname, int logrank) :\
				name(levelname), rank(logrank) {assert(levelname);}\
		virtual const char* get_name() const {\
			return name.c_str();\
		};\
		virtual int get_rank() const { return rank;}\
	private: string name; int rank;\
	}; \
	auto __tmp_var_LogLevelManage_##Level \
		=  LogLevelManage(std::make_shared< Log_##Level >(#Level, Rank));
```

其实所谓的“常量”，只不过是被 MAKE_LOGLEVEL 处理过的 类对象而已。

如果在同一个域中定义相同的"常量"，是不被编译器允许的，它可能会报类重定义的错误。这保证了在同一个域中，"常量"名的唯一性。

一旦一个“常量”被定义，则这个常量将被交给 LogLevelManage 管理。

-------
# MAKE_LOGLEVEL_INSIDE(Level, Rank)

用户无需关注它的实现，但这里还是给出定义, 以便帮助想有更多了解的人:

```cpp
// MAKE_LOGLEVEL 函数(局部)域版本
#define MAKE_LOGLEVEL_INSIDE(Level, Rank)\
	MAKE_LOGLEVEL(Level, Rank)\
	__tmp_var_LogLevelManage_##Level.clearwarning();
```


--------

# makelevel(Level)

"常量"修饰符

用户无需关心实现，但这里还是给出定义, 以便帮助想有更多了解的人:

```cpp
#define makelevel(Level) LogLevelManage::get_level_ptr(#Level)
```

其实它只是，返回之前交给LogLevelManage管理的“常量”的智能指针常对象.

--------
