#ifndef STORE_H
#define STORE_H

/**
 * 仓库模板类
 *
 * 特点: 线程安全的单件容器类，满足生产者-消费者模式需求
 * 原型:
 *		template <typename T, typename Container = std::queue<T> >
 *		class Store : public boost::serialization::singleton<Store<T, Container> >;
 * 说明:
 *		1. 类型 T 必须支持完整意义上的缺省构造、拷贝构造和 operator=
 *		2. 基础容器默认为 queue<T, deque<T> > （推荐）
 *		3. 基础容器必须支持: 
 *				1. 元素的访问: front 访问第一个元素
 *				2. 容量: empty, size
 *				3. 修饰符: push pop
 *		4. 公共方法摘要:
 *				1. void push(const T&);     入仓方法
 *				2. void pop(T&); T& pop();  出仓方法
 *				3. bool empty const ();     判空估值, 返回值是不可靠的(没有锁保护), 函数返回时可能值已改变。 
 *				//4. size_t size const ();  估算大小, 返回值是不可靠的(没有锁保护), 函数返回时可能值已改变。
 *				//5. bool full() const;     满仓判断。
 *				6. static void reserve(size_t); 重置仓库最大容量, 线程安全的，但可能会阻塞,仓库最大容量默认值为200000 
 *				7. static void reserve_unsafe(size_t); 重置仓库最大容量, 线程不安全的，不会阻塞。
 *				8. 两个重载版本方法: bool push(const T*); 和 bool pop(T*); 仅当传入参数指针为NULL时返回false
 *				9. 对boost::lockfree::queue 和 boost::lockfree::stack 的特化版本中的pop, 返回值为bool
 *		5. 特别的, empty() 底层实现是判定begin()迭代器和end()迭代器是否相等;
 *
 *		    // !!以下注释内容待进一步测试，size()是否安全不能就此妄下结论!!
 *		    //
 *		    //由于基础容器默认为 queue<T, deque<T> >, 则
 *			//size() 的底层实现是两个deque类型的迭代器相减，而operator-的实现代码如下：
 *			//	
 *			//	return typename _Deque_iterator<_Tp, _Ref, _Ptr>::difference_type
 *			//	(_Deque_iterator<_Tp, _Ref, _Ptr>::_S_buffer_size())
 *			//	* (__x._M_node - __y._M_node - 1) + (__x._M_cur - __x._M_first)
 *			//	+ (__y._M_last - __y._M_cur);
 *
 *		   //事实上，
 *			//	typedef ptrdiff_t	difference_type;
 *			//	(typename _Tp)*		_M_cur, _M_first, _M_last; 
 *			//	(typename _Tp)**	_M_node;
 *		   //即都是指针的减法运算，并不对指针取目标。
 *
 *		   故empty()无需锁保护, 但负面影响是函数返回时可能值已改变;
 *		   好处是, 方法没有锁保护可以使得仓库的I/O吞吐(写)能力不受影响.
 *		   另一个不加锁的的理由是, 使用读写锁(共享互斥量)完全可以解决数据被改变问题,
 *		   但读写锁代价比互斥锁高很多, 并且读写锁更适用于读的次数远大于写的次数,
 *		   故读写锁也不符合设计要求.
 */
////////////////////////////////////////////////////////////////
// 
// Example: 
//
// void foo() {
// 	
//		typedef Store<int> Int_Store;
// 		const Int_Store& ca = Int_Store::get_const_instance();
// 		Int_Store& ma		= Int_Store::get_mutable_instance();
// 
// 		cout << ca.size() << endl;				// 0
// 		ma.push(100);
// 		ma.push(200);
// 		ma.push(300);
// 		//cout << ma.size() << endl;			// 3
// 
// 		int b; 
// 		ma.pop(b);
// 		//cout << ca.size() << endl;			// 2
// 		cout << b << endl;						// 100
// 		int c;
// 		ma.pop(&c);
// 		//cout << ca.size() << endl;			// 1
// 		cout << c << endl;						// 200
// 		ma.pop(c);
// 		//cout << ca.size() << endl;			// 0
// 		cout << c << endl;						// 300
// 		cout << boolalpha << ca.empty() << endl;// ture
// }
//
// 这是老版本的示例，更详细的使用见 ./test/test.cpp
//
////////////////////////////////////////////////////////////////

#define BOOST_ALL_NO_LIB
#include <boost/thread.hpp>
#include <boost/serialization/singleton.hpp>
#include <queue>
//using boost::serialization::singleton;

namespace __store_private_ {
static const size_t STORECAPACITY = 2000000; // default capacity of Store
} // namespace __store_private_

//
// class std_priority_queue
// 对std::priority_queue二次封装, 以便适配 Store
//
template <typename T>
class std_priority_queue : public std::priority_queue<T> {
	public:
		const T& front() const {
			return std::priority_queue<T>::top();
		}
};


// 
// class Store
//
template <typename T, typename Container = std::queue<T> >
class Store : public boost::serialization::singleton<Store<T, Container> > {
	public:
		void push(const T& element) {
			mutex_t::scoped_lock lock(_store_lock);
			while (full()) {
				_cond_full.wait(_store_lock);
			}
			_store.push(element);
			_cond_empty.notify_one();
		}
		void push(T&& element) {
			mutex_t::scoped_lock lock(_store_lock);
			while (full()) {
				_cond_full.wait(_store_lock);
			}
			_store.push(std::forward<T>(element));
			_cond_empty.notify_one();
		}
		bool push(const T* element_ptr) {
			if (element_ptr) {
				//push(*element_ptr);
				push(std::move(*element_ptr));
				return true;
			}
			else {
				return false;
			}
		}

		void pop(T& element) {
			mutex_t::scoped_lock lock(_store_lock);
			while (empty()) {
				_cond_empty.wait(_store_lock);
			}
			element = _store.front();
			_store.pop();
			_cond_full.notify_one();
		}
		T& pop() {
			mutex_t::scoped_lock lock(_store_lock);
			while (empty()) {
				_cond_empty.wait(_store_lock);
			}
			T&& element = _store.front();
			_store.pop();
			_cond_full.notify_one();
			return element;
		}
		bool pop(T* element_ptr) {
			if (element_ptr) {
				pop(*element_ptr);
				return true;
			}
			else {
				return false;
			}
		}

		bool empty() const {
			return _store.empty();
		}

		// 改变容量大小. 线程安全的，但可能会阻塞.
		static void reserve(size_t capacity) {
			mutex_t::scoped_lock lock(_mutex_cap);
			reserve_unsafe(capacity);
		}
		// 改变容量大小. 线程不安全的，不会阻塞.
		static void reserve_unsafe(size_t capacity) {
			_capacity = capacity;
		}
	private:
		// size() - not thread-safe
		size_t size() const {  
			return _store.size();
		}
		bool full() const {
			return size() >= _capacity;
		}
	protected:
		Store(void) {}
		Store(const Store&) {}
	private:
		typedef boost::mutex            mutex_t;
		Container                       _store;
		mutex_t                         _store_lock;
		boost::condition_variable_any   _cond_full;     // full condition variable
		boost::condition_variable_any   _cond_empty;    // empty condition variable
		static size_t                   _capacity;      // capacity of Store
		static mutex_t                  _mutex_cap;     // for reserve()
};
template <typename T, typename Container>
size_t Store<T, Container>::_capacity = __store_private_::STORECAPACITY;
template <typename T, typename Container>
typename Store<T, Container>::mutex_t Store<T, Container>::_mutex_cap;



//////////
// note //
//////////
//
// lockfree::queue 和 lock::stack 的特化版本的 Store 
// 在 Android 平台遇到了些问题:
// https://github.com/DD-L/lproxy/issues/166
// 其他平台目前还未发现同样的问题
//
//
// 所以如果非必须, 不建议使用对 boost::lockfree::queue
// 和 boost::lockfree::stack 的偏特化版本的 Store
//
// 推荐裸用 boost::lockfree::queue 和 boost::lockfree::stack
//
//////////
// note //
//////////

// class Store 对 boost::lockfree::queue 偏特化版本
// defaults to boost::lockfree::fixed_sized<false>  , Dynamic Size
#include <boost/lockfree/queue.hpp>
template <typename T>
class Store<T, boost::lockfree::queue<T> > : 
        //public boost::serialization::singleton<
        //            Store<T, boost::lockfree::queue<T> > >,
		public boost::lockfree::queue<T> {
public:
	//using boost::lockfree::queue<T>::push;
	//using boost::lockfree::queue<T>::pop;
    typedef boost::lockfree::queue<T> BASETYPE;

    static Store& get_mutable_instance(void) {
        static Store __store;
        return __store;
    }

    static const Store& get_const_instance(void) {
        return Store::get_mutable_instance();
    }

	bool push(const T* element_ptr) {
		if (element_ptr) {
			return this->push(*element_ptr);
		}
		else {
			return false;
		}
	}
    bool push(const T& element) {
        return BASETYPE::push(element);
    }

	bool pop(T* element_ptr) {
		if (element_ptr) {
			return this->pop(*element_ptr);
		}
		else {
			return false;
		}
	}
    bool pop(T& element) {
        return BASETYPE::pop(element);
    }

	// 继承父类的reserve
	//void reserve(size_t);
	//void reserve_unsafe(size_t);
protected:
	//Store(size_t _capacity = __store_private_::STORECAPACITY) :
	Store(size_t _capacity = 0) :
		boost::lockfree::queue<T>(_capacity) {}
	Store(const Store&) {}
private:
	// 屏蔽下面接口
	size_t size() const { return 0; }  
	bool full()	  const { return false; }
};


// class Store 对 boost::lockfree::stack 偏特化版本
// defaults to boost::lockfree::fixed_sized<false>  , Dynamic Size
#include <boost/lockfree/stack.hpp>
template <typename T>
class Store<T, boost::lockfree::stack<T> > : 
        //public boost::serialization::singleton<
        //            Store<T, boost::lockfree::stack<T> > >,
		public boost::lockfree::stack<T> {
public:
    //using boost::lockfree::stack<T>::push;
    //using boost::lockfree::stack<T>::pop;
    typedef boost::lockfree::stack<T> BASETYPE;

    static Store& get_mutable_instance(void) {
        static Store __store;
        return __store;
    }

    static const Store& get_const_instance(void) {
        return Store::get_mutable_instance();
    }

    bool push(const T* element_ptr) {
        if (element_ptr) {
            return this->push(*element_ptr);
        }
        else {
            return false;
        }
    }
    bool push(const T& element) {
        return BASETYPE::push(element);
    }

    bool pop(T* element_ptr) {
        if (element_ptr) {
            return this->pop(*element_ptr);
        }
        else {
            return false;
        }
    }
    bool pop(T& element) {
        return BASETYPE::pop(element);
    }

    // 继承父类的reserve
    //void reserve(size_t);
    //void reserve_unsafe(size_t);
protected:
    //Store(size_t _capacity = __store_private_::STORECAPACITY) :
    Store(size_t _capacity = 0) :
        boost::lockfree::stack<T>(_capacity) {}
    Store(const Store&) {}
private:
    // 屏蔽下面接口
    size_t size() const { return 0; }
    bool full()	  const { return false; }
};
#endif // STORE_H
