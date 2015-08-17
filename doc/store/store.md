store/store.h

-----------------------------

#  “多生产者-多消费者” 的模板仓库

####特点: 
线程安全的单件容器类，满足多生产者-多消费者模式需求

####原型：
<pre>
template < typename T, typename Container = std::queue< T > >
class Store : public boost::serialization::singleton< Store< T, Container> >;
</pre>
####类摘要:

<pre>
template < typename T, typename Container = std::queue< T > >
class Store : public singleton< Store< T, Container > > {
public:
	// push
	// 在满仓时阻塞，直到仓库非满
	void push(const T& element);
	void push(T&& element);
	bool push(const T* element_ptr);
	// pop
	// 在空仓时阻塞, 直到仓库非空
	void pop(T& element);
	bool pop(T* element_ptr);

	// 非阻塞push/pop接口，在通用模板中尚未实现, 遗留
	// bool try_push(const T& element);
	// bool try_push(T&& element);
	// bool try_pop(T& element);
	// ...

	// capacity
	bool empty() const；
	static void reserve(size_t capacity)；
	static void reserve_unsafe(size_t capacity)；
	
	// Inherited from boost::serialization::singleton
	// static const T& get_const_instance();
	// static       T& get_mutable_instance();
};

</pre>

### 简单说明
1. 类型 T 必须支持完整意义上的缺省构造、拷贝构造和 operator=
2. 基础容器默认为 queue< T, deque< T > > 
3. 基础容器必须支持: 
	* 元素的访问: front 返回第一个要输出的元素
	* 容量: empty, size
	* 修饰符: push pop
4. 继承自 boost::serialization::singleton

#### 对 boost::lockfree::queue 偏特化版本
<pre>
template < typename T >
class Store< T, boost::lockfree::queue< T > > : 
		public singleton< Store< T, boost::lockfree::queue< T > > >,
		public boost::lockfree::queue< T >;
</pre>
可以使用 boost::lockfree::queue 的所有接口。

Note: 如果仓库元素想存放自定义类型MyClass, 则在使用boost::lockfree::queue/stack时，要用指针MyClass* 代替，这是boost::lockfree::queue/stack要求的， 即 T = MyClass*。

#### 对 boost::lockfree::stack 偏特化版本
<pre>
template < typename T >
class Store< T, boost::lockfree::stack< T > > : 
		public singleton< Store< T, boost::lockfree::stack< T > > >,
		public boost::lockfree::stack< T >; 
</pre>
可以使用 boost::lockfree::queue 的所有接口

#### 对std::priority_queue的封装，以便适配 Store
为实现泛型，对std::priority_queue::top()改名为front()
<pre>
template < typename T >
class std_priority_queue : public std::priority_queue< T > {
	public:
		const T& front() const {
			return std::priority_queue<T>::top();
		}
};
</pre>