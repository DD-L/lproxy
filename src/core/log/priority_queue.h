#ifndef L_PRIORITY_QUEUE_H 
#define L_PRIORITY_QUEUE_H


/**
 * std::priority_queue的堆排序算法不符合日志库需求
 * 这里特别为日志库定制的mini版的优先队列
 *		log_tools::priority_queue
 *		已实现为泛型模板, 以便其它用途
 *
 *		比如：
 *		针对当前日志库设计
 *
 *		优先成员指针mp 这样定义:
 *		LogType LogVal::* mp = &LogVal::log_type;
 *		优先因子vfactor这样定义:
 *		LogType f[2] = {makelevel(FATAL), makelevel(ERROR)}; 
 *		vector<LogType> vfactor(f, f +2);
 *
 *		若日志插入顺序是：
 *		{1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}
 *		则底层容器存储的顺序和插入的顺序相同：
 *      {1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}
 *      则日志弹出的顺序则是:
 *		{4, FATAL} {5, FATAL} {3, ERROR} {6, ERROR} {1, INFO} {2, WARN}
 *
 *
 *		同样的, 如果优先因子vfactor在定义时只有一个元素: FATAL
 *		则日志弹出的顺序为:
 *		{4, FATAL} {5, FATAL} {1, INFO} {2, WARN} {3, ERROR} {6, ERROR}
 *
 *		如果优先因子为空, 则日志弹出顺序和容器的元素存储顺序相同:
 *		{1, INFO} {2, WARN} {3, ERROR} {4, FATAL} {5, FATAL} {6, ERROR}
 *
 *
 *		
 *		关于 底层容器的说明
 *
 *		    底层容器如果像std::priority_queue一样选用std::vector,
 *		则必须在插入时就已经做好规定的排序，才能降低push/pop综合成本,
 *		但这增加了push的时间复杂度,而push通常和使用者的主业务密切相关。
 *
 *		    所以底层容器改用std::list. 有相关资料显示，与vector相比,
 *		list在尾部插入简单类型数据，只有大概到了10,000 ~ 100,000级别时，
 *		才会显现出差别; 而遍历元素出现劣势的情况则大概到了10,000,000级别.
 *		在日志模型中这足够使用了, 并且list从中间移除元素效率远大于其他容器
 *
 *
 *
 *      推荐:
 *
 *		    推荐在使用时, 采用一级指针类型作为容器元素, 这样可以提高效率。 
 *
 */ 

#include <list>
#include <set>

namespace log_tools {

template <typename value_type, typename PriorityFactor>
class priority_queue {
public:
	// init/settings
	static void settings(
			PriorityFactor value_type::* __value_mp, // value_type类成员指针
			const std::vector<PriorityFactor>& __vfactor) {// 优先因子(们)
		// 判断__vfactor元素是否唯一
		std::set<PriorityFactor> vf_test(__vfactor.begin(), __vfactor.end());
		assert(__vfactor.size() == vf_test.size());
		// 如果这里断言失败，说明__vfactor元素有重复

		value_type_mp    = __value_mp;
		vfactor          = __vfactor;
		is_settings      = true;
	}

	// push
	void push(const value_type& __x) {
		c.push_back(__x);
	}
	void push(value_type&& __x) {
		c.push_back(std::forward<value_type>(__x));	
	}

	//
	// top/front
	// 
	////////////////////
	//
	// 数学题(查找算法分析)
	// 已知：对列容器c, c.size() = N, vfactor为优先因子
	//讨论并对比下列两个方法中的两个for叠加轮循次数：
	//
	//方法一：for(vfactor) { for(c) if (cond) return; }
	//方法二：for(c) { for(vfactor) if (cond) return; }
	//
	//分析如下:
	//
	//如果vfactor只有一个元素{FATAL}, 则方法一的两个for叠加轮循次数较少:
	//   1. c中有FATAL的情况:
	//		  方法一可能出现的叠加轮循次数 {2, 3, ..., i, i+1, ..., N+1}
	//		  方法二可能出现的叠加轮循次数 {2, 4, ..., i, i+2, ..., 2N}
	//   2. c中无FATAL的情况:
	//        方法一叠加轮循次数为常量N+1
	//        方法二叠加轮循次数是常量2N
	//  对比两种方法，方法一在最好情况附近和最坏情况附近表现都优于方法二，
	//  而且方法一的查询增长率为+1, 方法二的增长率为+2. 此时方法一完胜方法二.
	//
	//如果vfactor中两个元素{FATAL, ERROR}, 则针对容器共有以下3种情况:
	//	(1). 有FATAL: 
	//	 (根据vfactor元素的排列顺序可知, FATAL优先级大于ERROR, 一旦出现FATAL, 
	//	  整个轮循就结束了, 故此时无需考虑队列中有没有ERROR)
	//	   则
	//	  方法一可能出现的叠加轮循次数:{2, 3, 4, ..., i, i+1, ..., N+1}
	//	  方法二可能出现的叠加轮循次数:{2, 5, 8, ..., i, i+3, ..., 3N-1}
	//	(2). 没有FATAL
	//	   (2.1) 有ERROR:
	//		 则
	//		方法一可能出现的叠加轮循次数: {N+2, N+3, ..., i, i+1, ..., 2N+1}
	//		方法二可能出现的叠加轮循次数: {3, 6, 9, ..., i, i+3, ..., 3N}
	//	   (2.2) 没有ERROR:
	//			方法一叠加轮循次数为常量2N+1
	//			方法二叠加轮循次数为常量3N
	//	
	//	对比两种方法, 
	//	情况(1)中, 不管最好情况附近和最坏情况附近的对比，方法一完胜
	//	情况(2.1)中，方法二在最好情况附近占据明显优势，而方法一在最坏情况附近
	//	占据明显优势，基本上是打个平手。
	//	情况(2.2)中, 方法一完胜
	//	而方法一的查询增长率为+1, 而方法二的查询增长率为+3. 
	//	此时方法一整体占优势
	//
	//(vfactor中有>=3个元素的情况不再一一列举，结果大致和上面相似)
	//
	//	综合考虑优劣情况，front函数内部元素查找算法采用方法一
	//
	////////////////////
	value_type& top() {
		assert(is_settings);		
		if (! vfactor.empty()) {
			// Use method 1 / find
			for (auto& factor : vfactor) {
				for (auto it = c.begin(); it != c.end(); ++it) {
					if ((*it).*value_type_mp == factor) {
						max_value = it;
						return *it;
					}
				}
			}
		}
		// not fount
		max_value = c.begin();
		return *max_value;
	}
	const value_type& top() const {
		return const_cast<priority_queue*>(this)->front();
	}
	// just for Store
	value_type& front() { return top(); }
	const value_type& front() const { return top(); }

	// pop
	void pop() { 
		c.erase(max_value);
	}
	
	// empty/size
	bool empty() const { return c.empty(); }
	size_t size() const { return c.size(); }

public:
	//  constructor/destructor
	priority_queue(void) : max_value(c.begin()) {}
	virtual ~priority_queue() {}
	// forbid copy
	priority_queue(const priority_queue&) = delete;
private:
	std::list<value_type>                    c;        // 底层容器
	typename std::list<value_type>::iterator max_value;// 
	static std::vector<PriorityFactor>       vfactor;  // 优先因子
	// value_type的成员指针
	static PriorityFactor value_type::*      value_type_mp; 
	static bool                              is_settings;

}; // log_tools::priority_queue

} // namespace log_tools




namespace log_tools {

// 优先因子容器，规定优先的因素
template <typename value_type, typename PriorityFactor>
typename std::vector<PriorityFactor> 
priority_queue<value_type, PriorityFactor>::vfactor;
// 成员指针,默认指向value_type的第一个成员
template <typename value_type, typename PriorityFactor>
PriorityFactor value_type::* 
priority_queue<value_type, PriorityFactor>::value_type_mp = NULL; 
// settings flag
template <typename value_type, typename PriorityFactor>
bool priority_queue<value_type, PriorityFactor>::is_settings = false;

} // namespace log_tools




/////////////////////////////////////////////////////////////////////////
//
// log_tools::priority_queue<value_type*, PriorityFactor> 的偏特化版本
//
//     以便log_tools::priority_queue容器能够容纳一级指针类型的元素
//
/////////////////////////////////////////////////////////////////////////

namespace log_tools {

template <typename value_type, typename PriorityFactor>
class priority_queue<value_type*, PriorityFactor> {

	// 'value_type*' should not be a multilevel pointer type
	static_assert(!std::is_pointer<value_type>::value, 
			"'value_type' cannot be a pointer type");

public:
	// init/settings
	static void settings(
			PriorityFactor value_type::* __value_mp, // value_type类成员指针
			const std::vector<PriorityFactor>& __vfactor) {// 优先因子(们)
		// 判断__vfactor元素是否唯一
		std::set<PriorityFactor> vf_test(__vfactor.begin(), __vfactor.end());
		assert(__vfactor.size() == vf_test.size());
		// 如果这里断言失败，说明__vfactor元素有重复

		value_type_mp    = __value_mp;
		vfactor          = __vfactor;
		is_settings      = true;
	}

	// push
	/*
	void push(const value_type& __x) {
		c.push_back(__x);
	}
	void push(value_type&& __x) {
		c.push_back(std::forward<value_type>(__x));	
	}
	*/
	void push(value_type* __x) {
		c.push_back(__x);
	}

	value_type* top() {
		assert(is_settings);		
		if (! vfactor.empty()) {
			// Use method 1 / find
			for (auto& factor : vfactor) {
				for (auto it = c.begin(); it != c.end(); ++it) {
					if ((*it)->*value_type_mp == factor) {
						max_value = it;
						return *it;
					}
				}
			}
		}
		// not fount
		max_value = c.begin();
		return *max_value;
	}
	const value_type* top() const {
		return const_cast<priority_queue*>(this)->front();
	}
	// just for Store
	value_type* front() { return top(); }
	const value_type* front() const { return top(); }

	// pop
	void pop() { 
		c.erase(max_value);
	}
	
	// empty/size
	bool empty() const { return c.empty(); }
	size_t size() const { return c.size(); }

public:
	//  constructor/destructor
	priority_queue(void) : max_value(c.begin()) {}
	virtual ~priority_queue() {}
	// forbid copy
	priority_queue(const priority_queue&) = delete;
private:
	std::list<value_type*>                    c;        // 底层容器
	typename std::list<value_type*>::iterator max_value;// 
	static std::vector<PriorityFactor>        vfactor;  // 优先因子
	// value_type的成员指针
	static PriorityFactor value_type::*       value_type_mp; 
	static bool                               is_settings;

}; // log_tools::priority_queue<value_type*, PriorityFactor>

} // namespace log_tools



namespace log_tools {

// 优先因子容器，规定优先的因素
template <typename value_type, typename PriorityFactor>
typename std::vector<PriorityFactor> 
priority_queue<value_type*, PriorityFactor>::vfactor;
// 成员指针,默认指向value_type的第一个成员
template <typename value_type, typename PriorityFactor>
PriorityFactor value_type::* 
priority_queue<value_type*, PriorityFactor>::value_type_mp = NULL; 
// settings flag
template <typename value_type, typename PriorityFactor>
bool priority_queue<value_type*, PriorityFactor>::is_settings = false;

} // namespace log_tools

#endif // L_PRIORITY_QUEUE_H
