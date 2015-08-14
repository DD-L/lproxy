
/** 
 *  Store的 push/pop 性能测试 <C代码部分>
 *
 *  <测试线程由UC线程库pthread驱动, 为跨平台, 后改用boost线程库>
 *
 */ 

#include <iostream>
//#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include <sstream>
#include <exception>
#include <assert.h>
#include <boost/timer.hpp>
#include "store.h"

/*
 * 准备测试用的 全局变量，结构体类型 及功能函数
 */ 

// 1. 普通容器 <默认std::queue>
#ifdef _STD_QUEUE
typedef Store<int> IntStore;
#endif

// 2. 容器采用 std_priority_queue (对std::priority_queue的二次封装)
#ifdef _STD_PRIORITY_QUEUE
typedef Store<int, std_priority_queue<int> > IntStore;
#endif

// 3. 容器采用 boost::lockfree::queue
#ifdef _BOOST_LOCKFREE_QUEUE
typedef Store<int, boost::lockfree::queue<int> > IntStore;
#endif

// 4. 容器采用 boost::lockfree::stack
#ifdef _BOOST_LOCKFREE_STACK
typedef Store<int, boost::lockfree::stack<int> > IntStore;
#endif

// buff
IntStore& buff = IntStore::get_mutable_instance();


/** 
 *  公共函数
 */


//pthread_mutex_t g_print_lock = PTHREAD_MUTEX_INITIALIZER;
boost::mutex g_print_lock;
void _print(const std::ostringstream& oss) {
	boost::mutex::scoped_lock lock(g_print_lock);
	//pthread_mutex_lock(&g_print_lock);
	std::cout << oss.str() << std::flush;
	//pthread_mutex_unlock(&g_print_lock);
}

#define print(msg) \
	do {std::ostringstream oss; \
		(oss << msg); _print(oss);} while(0)


/** 
 *  参数类型定义
 */

#include <vector>
// 测试push的线程函数所使用的参数
struct Pusharg {
	int     who;    // id
	int     minNum; // 为对结果统计和分析方便, 使用minNum
	int     maxNum; // 和maxNum, 以保证每次生产的数据不同
	bool    done;   // 当前生产者线程是否完成指定的生产任务
	Pusharg(int me, int min, int max, bool is_done = false) 
		: who(me), minNum(min), 
		  maxNum(max), done(is_done) {}
	// 简单类型无需考虑深拷贝
};

// 测试pop的线程函数所使用的参数
struct Poparg {
	const std::vector<Pusharg*> pa;
	bool is_push_done() const {
		for (auto& i : pa) {
			if (! i->done) return false;
		}
		return true;
	}
};


/** 
 *  计时器定义
 */


// 多线程下的计时器, 必须是懒汉式单件
// 当样本数据量比较大时，可避免重复计时
#include <boost/atomic.hpp>
#include <typeinfo>
template<typename T>
class TTimer {
public:
	static TTimer& get_instance() {
		boost::mutex::scoped_lock lock(_lock);
		if (++_count == 1) {
			_self = new TTimer();
		}
		return *_self;
	}

	static void release() {
		boost::mutex::scoped_lock lock(_lock);
		if (--_count <= 0) {
			delete _self;
			_self = NULL;
			_count = 0;
		}
	}

	static void destroy_all() {
		while (_self) {
			release();
		}
	}

	virtual ~TTimer() { // std::cout - not thread-safe
		std::cout << " [" << typeid(*this).name() 
			<< "] 共耗时: " << _t->elapsed() << 's';
	}
private:
	TTimer() : _t(new boost::timer()) { // std::cout - not thread-safe
		std::cout << " [" << typeid(*this).name() << "] 开始计时";
	}
	TTimer(const TTimer&) {}
private:
	static boost::atomic_int    _count;
	static boost::mutex         _lock;
	boost::timer*               _t;
	static TTimer*              _self;
};
template<typename T>
boost::atomic_int TTimer<T>::_count(0);
template<typename T>
TTimer<T>* TTimer<T>::_self = NULL;
template<typename T>
boost::mutex TTimer<T>::_lock;



//struct Push {};
//struct Pop  {};


/** 
 *  线程函数定义
 *		1. producer 测试push接口
 *		2. customer 测试pop 接口
 */

// test push
//void* producer(void* arg) {
void producer(Pusharg* arg) {
	assert(arg);
	Pusharg& _arg = *((Pusharg*)arg); // pthread痕迹
	assert(_arg.minNum <= _arg.maxNum);
	
	//pthread_mutex_lock(&g_print_lock);	
	g_print_lock.lock();
	std::cout << "\nthread push start @" << _arg.who;
	auto& ttimer = TTimer<struct Push>::get_instance();
	std::cout << "...\n";
	//pthread_mutex_unlock(&g_print_lock);
	g_print_lock.unlock();

	try {
        for (int i = _arg.minNum; i <= _arg.maxNum; ++i) {
			//buff.push(&i);
			buff.push(std::move(i));
			print("[in: " << i << "] ");
		}
	}
	catch (std::exception& e) {
        print("Except: " << e.what() << " @" << _arg.who << '\n');
	}

	//pthread_mutex_lock(&g_print_lock);	
	g_print_lock.lock();
	std::cout << "\nthread push exit @" << _arg.who;
	ttimer.release();
	std::cout << "...\n";
	//pthread_mutex_unlock(&g_print_lock);	
	g_print_lock.unlock();

	_arg.done = true;
	//return NULL;
}

// test pop
//#include <atomic>
//void* customer(void* arg) {
void customer(Poparg* arg) {
	assert(arg);
	Poparg& _arg = *((Poparg*)arg); // pthread 痕迹

	//pthread_mutex_lock(&g_print_lock);	
	g_print_lock.lock();
	//std::cout << "\nthread pop start @" << pthread_self();
	std::cout << "\nthread pop start @" << boost::this_thread::get_id();
	auto& ttimer = TTimer<struct Pop>::get_instance();
	std::cout << "...\n";
	//pthread_mutex_unlock(&g_print_lock);	
	g_print_lock.unlock();

	try {
		
		//******************************************************************//
		//
		// !!!!! 注意此处(while循环体)可能会引发死锁 !!!!!!
		//
		//     因为 Store::pop() 的设计需要, 仓库为空时，pop操作会阻塞；
		// 同时, 当下面if条件里面的表达式 buff.empty() 返回false时, 就不会break,
		// 而当 再次执行到 buff.pop() 时，buff 可能已经为空, 进而引发 pop 阻塞.
		//
		//     解决此死锁的办法之一是进入原子操作。但负面影响是, 屏蔽了buff.pop
		// 内置的锁机制，违反了测试 pop 的初衷，故放弃使用原子操作，保留可能
		// 出现的死锁现象。
		//
		//     当此死锁发生时, 不影响 push 的测试结果，也不会影响到 pop 的数据
		// 输出样本，只会影响到测试 pop 的计时策略，偶尔也会影响push的计时策略。
		// 代价比较小，故保留此测试bug
		//
		//******************************************************************//

		// 引入原子操作。
		// 以防止 buff.empty() 判空为true时，阻塞在pop(), 没能及时退出
		//std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
		int val = -1;
		while (true) {
			//if (! lock_stream.test_and_set()) {
			if (_arg.is_push_done() && buff.empty()) break;
			buff.pop(val); // 当buff为空时，阻塞
			print("[out: " << val << "] ");
			//lock_stream.clear();
			//}
		}
	}
	catch (std::exception& e) {
        //print("Except: " << e.what() << " @" << pthread_self() << "\n");
        print("Except: " << e.what() 
				<< " @" << boost::this_thread::get_id() << '\n');
    }

	//pthread_mutex_lock(&g_print_lock);	
	g_print_lock.lock();
	//std::cout << "\nbuff.empty() = " << std::boolalpha << buff.empty()
	//		<< " @" << pthread_self();
	//std::cout << "\nthread pop exit @" << pthread_self();
	std::cout << "\nbuff.empty() = " << std::boolalpha << buff.empty() 
			<< " @" << boost::this_thread::get_id();
	std::cout << "\nthread pop exit @" << boost::this_thread::get_id();
	//ttimer.release();
	ttimer.destroy_all(); // 因为后面是 _exit()
	std::cout << "...\n" << std::flush;
	//pthread_mutex_unlock(&g_print_lock);	
	g_print_lock.unlock();

	_exit(0);
	//return NULL;
}


/**
 *  功能测试函数
 */ 

// test
void test(const int producer_steps = 100,   // 每个生产者线程生产的数据的个数
          const int producer_count = 7,     // 生产者线程个数
          const int customer_count = 3,     // 消费者线程最大个数
          const size_t capacity = 200001) { // 仓库容量

	// test reserve
	buff.reserve(capacity);

	//pthread_attr_t attr;
	//pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	//pthread_t tid;

	boost::thread_group push_thread_group;
	boost::thread_group pop_thread_group;

	std::vector<Pusharg*> push_arg_set;
	// 最小数据为 0
	for (int i = 0; i < producer_count; ++i) {
		Pusharg* _arg = new Pusharg (
			i,                            // who
			i * producer_steps,           // minNum 
			(i + 1) * producer_steps - 1, // maxNum
			false                         // done
		);
		push_arg_set.push_back(_arg);
		//pthread_create(&tid, &attr, producer, (void*)_arg);
		push_thread_group.create_thread(boost::bind(producer, _arg));
	}


	Poparg pop_arg = {push_arg_set};
	for (int i = 0; i < customer_count; ++i) {
		//pthread_create(&tid, &attr, customer, (void*)&pop_arg);
		pop_thread_group.create_thread(boost::bind(customer, &pop_arg));
	}

	//getchar(); // pthread_join
	push_thread_group.join_all();
	pop_thread_group.join_all();
	
	// 以下为多余代码
	// 因为某个消费者线程函数把所有任务完成后会直接_exit(0)
	for (auto i : pop_arg.pa) delete i;
	//pthread_attr_destroy(&attr);
	// ...
}

int main(int argc, char** argv) {
	if (argc != 4 and argc != 5) {
		print("Usage: " << argv[0] << " producer_steps producer_count"
				" customer_count [capacity]\n");
		print("  producer_steps: 每个生产者线程生产的数据的个数.\n");
		print("  producer_count: 生产者线程个数.\n");
		print("  customer_count: 消费者线程最大个数.\n");
		print("  capacity:       仓库容量. 可选参数, 默认值为200001.\n");
		return -1;
	}
	const int producer_steps = atoi(argv[1]);
	const int producer_count = atoi(argv[2]); 
	const int customer_count = atoi(argv[3]);
	const int capacity = (argc == 5) ? atoi(argv[4]) : 200001;
	assert(producer_steps > 0);
	assert(producer_count > 0);
	assert(customer_count > 0);
	assert(capacity > 0);
	print("producer_steps = " << producer_steps << '\n');
	print("producer_count = " << producer_count << '\n');
	print("customer_count = " << customer_count << '\n');
	print("capacity       = " << capacity       << '\n');
	print("回车键继续...");
	getchar();
	print("\n");
	// 如果参数为(100, 7, 3, 200001)， 则生产的数据最小值为0, 最大值为 1000 * 7 -1
	test(producer_steps, producer_count, customer_count, capacity);
	return 0;
}
