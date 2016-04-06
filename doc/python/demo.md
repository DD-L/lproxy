# demo

#### C++ 代码部分

./demo.cpp

```cpp
#include <python/getpyfunc.h>
#include <iostream>

int main(int argc, char* argv[]) {
	// 注意文件路径
	GetPyFunc foo("./test.py", "foo");
	{
		boost::python::object ret = foo(1);
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	{
		boost::python::object ret = foo(2, "qwerty");
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	{
		boost::python::object ret = foo();
		std::cout << boost::python::extract<int>(ret) << std::endl;
	}
	
	return 0;
}

```

#### python 代码部分

./test.py

```python
def foo(a = 5, b = 'haha'):
    print(b)
    return a + 100
```

#### 程序输出

```
$ ./bin/demo.exe
haha
101
qwerty
102
haha
105
```

 