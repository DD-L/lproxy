# logrotate demo

demo.cpp

```cpp
#include <logrotate/logrotate.h>
#include <cstring> // for memset
#include <cstdio>  // for getchar

int main() {
    std::ofstream log;
    Logrotate lrt;

	// 追加打开 "./example.log" 文件， 如果一旦 该文件 超过 50 byte 就开始执行 分片压缩 
    bool ret = lrt.bind(log, "./example.log", 50/*50 byte*/);
    std::cout << "current pos = " << log.tellp() << std::endl;

    char *buf = new char[1024];
    if (ret) {
		// 进入 分片压缩 循环， 这里只是演示， 所以只设置 10 次循环
        for (int loop = 0; loop < 10; ++loop) {
			// 尝试 转储, 如 文件大小 未超过设定值 50 byte 就什么也不做。
            lrt.action();


			/////////////////////////
            std::cout << "current pos = " << log.tellp() << std::endl;
            std::cout << "Press Enter to continue." << std::endl;
            getchar();

			// 向 ./example.log 文件灌送 内容，以模拟 日志文件 不断增长 的情况 
            std::ifstream in("./1.txt");
            if (! in)
                std::cout << "open" << std::endl;
        
            do {
				memset(buf, 0, 1024);
                in.getline(buf, 1024);
				// 向 ./example.log 灌内容， 模拟 现实生产环境。
                log << buf << std::flush;
            } while(in.fail());
            
        }
    }
    else { 
        std::cout << "open file failed.\n";
    }
    delete[] buf;

    return 0;
}
```


./1.txt 样例: [here](../../src/core/logrotate/demo/1.txt)

### 程序执行情况

程序执行时，基本上，每敲一次 <Enter>, 另外一个终端就 `ls` 一下，以观察 分片 压缩情况。

```shell
$ ./demo.exe
current pos = 0
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-34-22.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-34-39.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-34-46.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-34-58.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-35-07.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-35-25.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-35-38.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-35-43.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
open file:[./example.log]
open file:[./example.log.2016-04-05_12-35-53.gz]
write content: [<open file './example.log', mode 'rb' at 0xffdff288>]
write completed...
current pos = 0
Press Enter to continue.
<Enter>
```

日志转储情况：

```
$ ls
1.txt  demo.exe  demo.static  example.log  logrotate.py
$ ls
1.txt     demo.static  example.log.2016-04-05_12-34-22.gz
demo.exe  example.log  logrotate.py
$
$
$ ls
1.txt     demo.static  example.log.2016-04-05_12-34-22.gz  logrotate.py
demo.exe  example.log  example.log.2016-04-05_12-34-39.gz
$ ls
1.txt        example.log                         example.log.2016-04-05_12-34-46.gz
demo.exe     example.log.2016-04-05_12-34-22.gz  logrotate.py
demo.static  example.log.2016-04-05_12-34-39.gz
$ ls
1.txt        example.log                         example.log.2016-04-05_12-34-46.gz
demo.exe     example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-34-58.gz
demo.static  example.log.2016-04-05_12-34-39.gz  logrotate.py
$
$ ls
1.txt        example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-07.gz
demo.exe     example.log.2016-04-05_12-34-39.gz  logrotate.py
demo.static  example.log.2016-04-05_12-34-46.gz
example.log  example.log.2016-04-05_12-34-58.gz
$
$ ls
1.txt        example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-07.gz
demo.exe     example.log.2016-04-05_12-34-39.gz  example.log.2016-04-05_12-35-25.gz
demo.static  example.log.2016-04-05_12-34-46.gz  logrotate.py
example.log  example.log.2016-04-05_12-34-58.gz
$ ls
1.txt        example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-07.gz
demo.exe     example.log.2016-04-05_12-34-39.gz  example.log.2016-04-05_12-35-25.gz
demo.static  example.log.2016-04-05_12-34-46.gz  example.log.2016-04-05_12-35-38.gz
example.log  example.log.2016-04-05_12-34-58.gz  logrotate.py
$ ls
1.txt                               example.log.2016-04-05_12-34-58.gz
demo.exe                            example.log.2016-04-05_12-35-07.gz
demo.static                         example.log.2016-04-05_12-35-25.gz
example.log                         example.log.2016-04-05_12-35-38.gz
example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-43.gz
example.log.2016-04-05_12-34-39.gz  logrotate.py
example.log.2016-04-05_12-34-46.gz
$ ls
1.txt                               example.log.2016-04-05_12-34-58.gz
demo.exe                            example.log.2016-04-05_12-35-07.gz
demo.static                         example.log.2016-04-05_12-35-25.gz
example.log                         example.log.2016-04-05_12-35-38.gz
example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-43.gz
example.log.2016-04-05_12-34-39.gz  example.log.2016-04-05_12-35-53.gz
example.log.2016-04-05_12-34-46.gz  logrotate.py
$ ls
1.txt                               example.log.2016-04-05_12-34-58.gz
demo.exe                            example.log.2016-04-05_12-35-07.gz
demo.static                         example.log.2016-04-05_12-35-25.gz
example.log                         example.log.2016-04-05_12-35-38.gz
example.log.2016-04-05_12-34-22.gz  example.log.2016-04-05_12-35-43.gz
example.log.2016-04-05_12-34-39.gz  example.log.2016-04-05_12-35-53.gz
example.log.2016-04-05_12-34-46.gz  logrotate.py

$ ls -alh example.log
-rw-rw-r--+ 1 User None 54 4月   5 12:35 example.log

```

#### 转储文件格式

日志压缩分片后的文件名的格式是这样的：

```
日志文件名.年-月-日_时-分-秒.gz
```

如果一秒内会分片压缩很多次，则会自适应的生成如下格式的转储文件, 例如：

```
日志文件名.年-月-日_时-分-秒.gz
日志文件名.年-月-日_时-分-秒_01.gz
日志文件名.年-月-日_时-分-秒_02.gz
日志文件名.年-月-日_时-分-秒_03.gz
...
日志文件名.年-月-日_时-分-秒_100.gz
...
```

所以不必担心日志急速膨胀，转储文件会被覆盖问题。