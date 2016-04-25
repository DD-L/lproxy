### `/path/to/lproxy/Makefile` “伪目标”说明

| 伪目标             | 作用       |
|--------------------|------------|
| `all`              | do nothing |
| `init`             | 依次执行伪目标 `check` `boost` `cryptopp`|
| `init.force`       | 依次执行伪目标 `check` `boost.force` `cryptopp`|
| `check`            | 检查编译环境：1.系统是否安装`dos2unix`; 2. 是否支持 C++11|
| `boost`            | 释放 boost 库源码 （lproxy 是以 boost 源码嵌入的方式完成编译的）|
| `boost.force`      | 强制释放 boost 库源码|
| `cryptopp`         | 下载并编译 `cryptopp` 静态库|
| `lss`              | 编译 lss, 执行 `cd /path/to/lproxy/src/core/lss; $(MAKE) -f Makefile`|
| `lss.cygwin`       | 在 Cygwin 环境下编译 lss，执行 `cd /path/to/lproxy/src/core/lss; $(MAKE) -f Makefile.Cygwin`|
| `lss.clean`        | 执行 `cd /path/to/lproxy/src/core/lss; $(MAKE) clean` |
| `install`          | 会将编译好的 lss 二进制程序及配置文件拷贝到 `/path/to/lproxy/bin` 目录下|
| `uninstall`        | 删除安装， 会执行 `$(RM) /path/to/lproxy/bin` |
| `boost_build.clean`| 执行 `cd src/core/boost_build; $(MAKE) clean` |
| `except.clean`     | 执行 `cd src/core/except; $(MAKE) clean` |
| `language.clean`   | 执行 `cd src/core/language; $(MAKE) clean` |
| `store.clean`      | 执行 `cd src/core/store; $(MAKE) clean` |
| `crypto.clean`     | 执行 `cd src/core/crypto; $(MAKE) clean` |
| `log.clean`        | 执行 `cd src/core/log; $(MAKE) clean` |
| `python.clean`     | 执行 `cd src/core/python; $(MAKE) clean` |
| `logrotate.clean`  | 执行 `cd src/core/logrotate; $(MAKE) clean` |
| `src.clean`        | 清理 src 中的二进制文件，依次执行伪目标 `boost_build.clean` `except.clean` `language.clean` `store.clean` `crypto.clean` `log.clean` `python.clean` `logrotate.clean` `lss.clean` |
| `contrib.clean`    | 依次清除 “先前释放的 boost 库源码”，“cryptocpp 源码及其静态库”|
| `all.clean`        | 依次执行伪目标 `src.clean` `contrib.clean` |
| `clean`            | do nothing |