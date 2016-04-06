
# ExceptionTemplate

`ExceptionTemplate` 使用示例

```cpp
#include <except/except.h>

// 如果要定义一个自己的异常类 MyException 很容易：

struct __myexception {
	static std::string name() {
	        return "MyException";
	}
};
typedef ExceptionTemplate<__myexception>    MyException;


// 可以像这样来使用 MyException
int main()
try {
	throw(MyException("This is a test"));
	return 0;
}
catch (const MyException& me) {
	std::cout << me.what() << std::endl; // MyException Exception: This is a test
}
catch (const std::exception& e) {
	// 如果没有上面的 catch (const MyException& me) 语句，将会被 std::exception 捕获
	std::cout << e.what() << std::endl;
}

```