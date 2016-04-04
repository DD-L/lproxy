# language demo

```cpp
#include <language/lang.h>
#include <iostream>
#include <string>
using namespace std;

void test() {
	cout << "=====test=======" << endl;
	Lang::setLang(new LangEN());
	std::cout << Lang("hehe") << std::endl; // hehe
	cout << "====end test====" << endl;
}

int main() {
	{
	Lang::setLang(new LangCN());
	cout << Lang("hehe") << endl;                     // 呵呵
	cout << (Lang("hehe") + "llo") << endl;           // 呵呵llo
	cout << (Lang("hehe") + string("llo")) << endl;   // 呵呵
	cout << (Lang("hehe") + Lang("hello")) << endl;   // 呵呵你好

	test();
	cout << Lang("hehe") << endl;      // hehe
	cout << Lang("hello") << endl;     // hello
	Lang::setLang(new LangCN());     
	cout << Lang("hehe") << endl;      // 呵呵

	}
	return 0;
}
```

`LangEN` 和 `LangCN` 定义

```cpp

struct LangCN : Lang {
	LangCN(void) {
		m["hello"] = "你好";
		m["hehe"] = "呵呵";
		// ...
	}
};
struct LangEN : Lang {
	LangEN(void) {
		m["hello"] = "hello";
		m["hehe"] = "hehe";
		// ...
	}
};
``` 