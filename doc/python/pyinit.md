python/pyinit.h

-----


# pyinit

```cpp
class pyinit : boost::noncopyable;
```
用来初始化 python 环境的类，使用者无需关心。

### 类摘要

```cpp
class pyinit : boost::noncopyable {
	public:
		pyinit(int initsigs = 1);
		~pyinit() {
			// Py_Finalize();
		}
		bool isInitialized();
		const char* version();
		static void err_print();
};
```