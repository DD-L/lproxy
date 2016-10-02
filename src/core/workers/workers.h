#ifndef WORKER_H
#define WORKER_H

/**
 *	Workers 抽象工作者类
 *
 *	1. 特点: 对工人集合进行了封装; 一个工人对应一个工作线程, 不同工种由其唯一
 *	的线程池对象进行管理; 对不同工种的执行流程提供了继承接口; 不同工种由其子类
 *	实现(一个子类对应一个工种)。
 *	2. 原型: 
 *			template<typename T> 
 *			class Workers : public boost::serialization::singleton<T>;
 *	3. 方法摘要:
 *			1. void start(const count_t& n, bool return_immediately = false);
 *				Workers开工方法. 
 *				参数一为要启用的工作线程数量. 
 *				注意, 当n小于或等于已启用的工人数，则将不会有新的工人被创建;
 *				参数二为是否立即返回, 当为false时，该函数会一直等待工人被要求
 *				中断退出后才返回, 否则一直阻塞等待.
 *			2. void interrupt_all(bool return_immediately = false);
 *				中断所有工人的工作.
 *				中断点被设置在了单个工作流程执行完毕的地方, 确保数据被处理完毕.
 *				参数为是否立即返回, 当为false时, 此函数会一直等待所有工作线程
 *				中断退出以后才返回, 否则一直阻塞等待
 *			3. inline int workers_count() const;
 *				获取当前工作线程数量.
 *			4. count_t add_workers(const count_t& n);
 *				增加工作线程数量方法. 返回调整后的数量, 内置锁保护.
 *				参数为要增加的工作线程数量。
 *			5. count_t sub_workers(const count_t& n, 
 *			                    bool return_immediately = false);
 *				减少工作线程数量方法. 返回调整后的数量, 内置锁保护.
 *				参数一为将要减少工作线程的数量; 
 *				参数二为是否立即返回, 当为false 时, 此函数会一直等待要被中断
 *				工作线程中断退出以后才会返回, 否则一直阻塞等待。
 *				裁员会中断工作线程, 但会保证单个工作流程执行结束. 
 *				*****由于开发进度要求，此方法尚未完全实现，留作遗留，
 *				待下个开发版本实现. ******
 *			6. count_t reset_count(const count_t& n, 
 *			                    bool return_immediately = false);
 *				调整工作线程数量方法，返回调整后的数量，内置锁保护。
 *				参数一为要将工作线程的数量调整后的值; 
 *				参数二为是否立即返回, 仅在当前工作线程的数量比参数一n大时才有效, 
 *				当为参数二的值为false 时, 此函数会一直等待多余工作线程中断退出
 *				以后才返回, 否则一直阻塞等待。
 *				裁员会中断工作线程, 但会保证单个工作流程执行结束. 
 *				此方法依赖上两个函数, ******故留作遗留.******
 *			7.	私有虚函数介绍:
 *				1. 进入流程循环之前的处理函数
 *					virtual void prepare();
 *				2. 一次工作流程
 *					virtual void process(void) = 0;
 *				3. 对于工作线程中断后的响应函数
 *					virtual void interruption_respond();
 *			8. 其他
 *				static void run(Workers& workers); [private] 线程执行函数
 *	4. 直接依赖:
 *				1. boost 线程库、serialization单件等
 */


///////////////////////////////
// See examples logworkers.h //
///////////////////////////////


#define BOOST_ALL_NO_LIB
#include <boost/thread.hpp>
#include <boost/serialization/singleton.hpp>

//using boost::serialization::singleton;
//using namespace boost;
template<typename T>
class Workers : public boost::serialization::singleton<T> {
	public:
		typedef unsigned int	count_t;
		typedef boost::mutex	mutex_t;
	public:
		// 工人开工方法. 
        // 注意, 如果n小于或等于已有的工人数，则将不会创建新的工人.
		void start(const count_t& n, bool return_immediately = false);
		// 中断所有工人的工作. 中断点被设置在了单个工作流程执行完毕的地方
		void interrupt_all(bool return_immediately = false);
		
		// 工人数量
		inline int workers_count() const;
		// 增加工人数量，返回调整后的数量
		count_t add_workers(const count_t& n);

		/* 以下两个方法留作遗留, 尚未完全实现
		// 裁员，返回裁员后的人数.
		// 如果裁员必须保证工人的一个生产周期完成才可以裁掉
		count_t sub_workers(const count_t& n, bool return_immediately = false);
		// 调整工人数量, 返回调整后的数量
		count_t reset_count(const count_t& n, bool return_immediately = false);
		*/
		Workers& operator=(const Workers&) = delete;
		virtual ~Workers();
	private:
		// 进入流程循环之前的处理函数
		virtual void prepare();
		// 一次工作流程
		virtual void process(void) = 0;
		// 对于工作线程中断后的响应函数
		virtual void interruption_respond();
		// 线程函数
		static void run(Workers& workers);
	private:
        boost::thread_group     workers;
		// 因为 thread_group 内部维护的thread list对外不可见, 
		// 所以这里要再声明一个	thread list, 以便对每个工作线程进行管理.
		// 裁员时凸显 thread list 的重要性.
        // std::list<boost::thread*>	threads; 
		mutex_t                 mu; // 调整工人数量时使用的锁
};

#include <iostream>
template<typename T>
void Workers<T>::start(const count_t& n, bool return_immediately/* = false*/) {
	for (count_t i = workers_count(); i < n; ++i) {
		//threads.push_back(workers.create(boost::bind(run, boost::ref(*this)))));
		workers.create_thread(boost::bind(run, boost::ref(*this)));
	}
	if (! return_immediately) {
		workers.join_all();		
	}
	else {
		return;
	}
}

template<typename T>
void Workers<T>::interrupt_all(bool return_immediately/* = false*/) {
	// interrupt all the working thread
	workers.interrupt_all();
	if (! return_immediately) {
		workers.join_all();		
	}
	else {
		return;
	}
}

template<typename T>
inline int Workers<T>::workers_count() const {
	return workers.size();
}


template<typename T>
void Workers<T>::run(Workers& workers) 
try {
	// prepare, before loop
	workers.prepare();
	//process loop
	while (true) {
		workers.process();
		// interruption point
        boost::this_thread::interruption_point();
	}
}
catch (boost::thread_interrupted&) {
	// interrupt exception handling
	workers.interruption_respond();
}

#include <typeinfo.h>
template<typename T>
/*virtual*/void Workers<T>::prepare() {
	std::cout << "Now start a Workers thread(id:" 
		<< boost::this_thread::get_id() 
		<< ", RTTI:\"" << typeid(T).name() << "\")!" << std::endl;
}
template<typename T>
/*virtual*/void Workers<T>::interruption_respond() {
	std::cout << "Workers thread(id:" << boost::this_thread::get_id() 
		<< ", RTTI:\"" << typeid(T).name() 
		<< "\") is interrupted." << std::endl;
}

template<typename T>
typename Workers<T>::count_t Workers<T>::add_workers(const count_t& n) {
	mutex_t::scoped_lock lock(mu);	
	for (count_t i = 0; i < n; ++i) { 
		workers.add_thread(new boost::thread(run, boost::ref(*this))); 
        // 不会泄露，workers的析构会delete它
        // 参见 <boost/thread/detail/thread_group.hpp> 
        // 中的thread_group::~thread_group()
		/*
		thread* new_woker = new thread(run, boost::ref(*this)); 
		workers.add_thread(new_worker);
		threads.push_back(new_worker);
		*/
	}
	return workers.size(); // 返回增加后的个数
}

/* 此方法未实现，在以后的开发版本中实现*/
//template<typename T>
//typename Workers<T>::count_t Workers<T>::sub_workers(const count_t& n) {
//	mutex_t::scoped_lock lock(mu);	
//	for (count_t c = 0, auto it = threads.begin(); 
//			it != threads.end(); ++it) {
//		// 从工人组里移除
//		workers.remove_thread(&(*it));
//		// 发出中断指令 
//		it->interrupt();
//		// 断言此工作线程被要求中断
//		assert(it->interruption_requested());
//		// 从 threads list 中移除
//		threads.erase(it);
//		// 遗留问题:
//		// 此处内存有泄露，要选择在线程退出后再 delete it
//		// 而且由于是在循环内部erase, 可能会多执行一次++it
//
//		if (++c >= n) break;
//	}
//	return workers.size();
//}

/* 由于sub_workers()未实现，且此方法依赖sub_workers()，
 * 所以此方法也未实现. 以后开发版本在实现
 */
//template<typename T>
//typename Workers<T>::count_t Workers<T>::reset_count(const count_t& n) {} 

template<typename T>
Workers<T>::~Workers() {
	// interrupt all the working thread & wait
	this->interrupt_all(/*false*/);
}

#endif // WORKER_H

