program_options/program_options.h

---------------

# program_options

```cpp
class program_options;
```
命令行选项工具。

POSIX C 中有个工具函数 `int getopt(int argc,char * const argv[ ],const char * optstring);` 用来分析命令行参数，很实用，但可惜无法跨平台。

Boost.program_options 库可以做得非常优秀，但因为 Boost.program_options库太大了，而且必须事先编译好 Boost.program_options 库才能使用。

所以这里实现了一个简易的 `program_options`，应对简单的命令行参数处理足够了，使用时直接 `#include "program_options.h"` 即可，不用事先编译好它。


该工具实现的 `help` 信息格式大致是这样的：

```shell
[title]

Usage: [usage]

Options:
  [option]  [message]
  [option]  [message]
            [message]
  [...]

Examples:
  [example]
  [example]
  [...]
```

* 一共有 4 个区块： `title` 区块， `Usage` 区块， `Options` 区块 和 `Examples` 区块；
* 其中 `title`区块 和  `Examples`区块 是可选的区块，如果不设置，则不会出现；
* 用 `[]` 括起来的内容都是变量； 
* `message` 中允许换行，而保持对齐。

### program_options 类摘要

```cpp
class program_options {
public:
    class parameter_error : public std::exception {
    public:
        parameter_error(const std::string& option) noexcept 
            : m_msg("Exception: program_options::parameter_error: " + option) {}
        virtual ~parameter_error (void) noexcept {}
        virtual const char* what() const noexcept {
            return m_msg.c_str();
        }
    private:
        std::string m_msg;
    };
public:
    explicit program_options(const std::string& usage);
    void add_option(const std::string& options, const std::string& message);
    void example(const std::string& message);
    const std::string show_help(const std::string& title = "") const;
    void store(int argc, char** argv);
    const std::vector<std::string> get_argv(void) const;
    bool count(const std::string& option) const;
    bool empty(void) const;
    std::string get(const std::string& option) const;
};
```

* 构造函数

	```cpp
	explicit program_options(const std::string& usage);
	``` 
	构造时就已经决定了 usage 的输出内容

* 添加选项

	```cpp
	void add_option(const std::string& options, const std::string& message);
	```
	可重复调用该方法。在帮助信息显示时，`option` 前面会留有有两个空格， `options` 和 `message` 之间会留有两个空格，`message` 如果一行显示不完，则允许折行，折行方法是在 `message` 中添加 `\n`. 

* 添加示例

	```cpp
	void example(const std::string& message);
	```
	这是一个可选方法，如果未调用该方法，则帮助信息中不会出现`Examples`区块。可重复调用该方法。每个 `example` 前面留有两个空格。

* 显示帮助信息

	```cpp
	const std::string show_help(const std::string& title = "") const;
	```
	`tilte` 是可选值，如果未设置，则不会出现`tilte`区块。一般 title 用来显示描述信息。

* 存储 `main` 函数参数

	```cpp
	void store(int argc, char** argv);
	```
* 返回存储的 agrv 
	
	```cpp
	const std::vector<std::string> get_argv(void) const;
	```
* 检查给定的选项是否出现在已经存储的 argv 中
	
	```cpp
	bool count(const std::string& option) const;
	```
* 对已经储存的选项判空
	
	```cpp
	bool empty(void) const;
	```
	它的实现方式很简单 `return argc == 1;`

* 获取选项值

	```cpp
	std::string get(const std::string& option) const;
	```
	
	获取选项后面跟的值，以空格为分界线。注意该方法返回 `std::string`, 如果你想得到数字类型，需要自己转换。如果在存储的选项中，如果 get 不到值，会抛出一个 `program_options::parameter_error ` 类型的异常。

### demo

使用示例见 [demo](./demo)