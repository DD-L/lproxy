src/static_analysis/cppcheck/readme.md

----------------------------------

static analysis
===============
### 代码静态检查



####tools:

	cppcheck
	cppcheck-htmlreport

####source:

	$ROOT/tools/static_analysis_tool/cppcheck/cppcheck-1.69.tar.bz2
	
	<$ROOT为项目根目录，下同>
####manual:

	$ROOT/tools/static_analysis_tool/cppcheck/manual.pdf
	
####install:
	1. 手动安装
		(1) make
			cd $ROOT/tools/static_analysis_tool/cppcheck
			tar jxvf cppcheck-1.69.tar.bz2
			cd cppcheck-1.69
			make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
		(2) install
			cd $ROOT/tools/static_analysis_tool/cppcheck
			mkdir -p ./bin/cfg/
			cp -a ./cppcheck ./bin/
			cp -a ./htmlreport/cppcheck-htmlreport ./bin/
			cp -a ./cfg/std.cfg ./bin/cfg/
			chmod +x ./bin/cppcheck
		(3) check
			1. 文件 ./bin/cppcheck 必须有执行权限
			2. 文件 ./bin/cfg/std.cfg 必须存在
			3. 文件 ./bin/cppcheck-htmlreport 的执行：
				1. 依赖python运行环境，2.7.x 或 更高版本
				2. 依赖python模块Pygments ( http://pygments.org/ )
						如果缺失Pygments, 请执行 pip install Pygments 安装;
						或到 https://pypi.python.org/pypi/Pygments 下载安装
					
						Pygments是一个语法高亮工具，可用于例如论坛、wiki和其他方面的 Web 应用；
						提供命令行工具和开发包，可输出包括 HTML, RTF, LaTex and ANSI 序列等格式。
	
	2. 自动化脚本安装(推荐)
		1. 方式一：
			cd $ROOT/tools/static_analysis_tool/cppcheck; make
		2. 方式二： (墙裂推荐)
			cd $ROOT/src/static_analysis/cppcheck; make init

####uninstall

	1. 手动卸载
		cd $ROOT/tools/static_analysis_tool/cppcheck
		rm -rf ./cppcheck-1.69
		rm -rf ./bin
	2. 自动化脚本卸载
		cd $ROOT/tools/static_analysis_tool/cppcheck
		make uninstall

####usage:

	1. cppcheck 简单使用示例
		1. cppcheck
			cp $ROOT/tools/static_analysis_tool/cppcheck/bin/cfg/std.cfg ./
			$ROOT/tools/static_analysis_tool/cppcheck/bin/cppcheck --xml --std=c++11 --template=gcc --enable=all --force -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
			rm ./std.cfg
		2. cppcheck-htmlreport (python tool，依赖python运行环境，本机v2.7.x)
			#pip install Pygments #依赖Pygments模块, 参见 install|手动安装|check
			$ROOT/tools/static_analysis_tool/cppcheck/bin/cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR
	
	2. 自动化脚本 
		躶用cppcheck太麻烦，于是编写了静态检查自动化脚本 Makefile, cppcheck_report.sh。
		（注意严格的代码静态检查会很耗时, 大部分时间都是在扫描的库）
		
		注：以下涉及到的命令，在没有特别说明的情况下，都是在 $ROOT/src/static_analysis/cppcheck 路径下完成的

		1. 配置脚本（一般不需要手动配置）
			1. 修改cppcheck_report.sh中的 XX 变量.(一般不需修改)
			
				XX=g++
				如果你使用其它g++编译器(比如交叉编译器)，需在此修改；否则没有必要修改XX.

			2. 修改cppcheck_report.sh脚本文件中的 INCLUDEFILES 变量.(一般不需修改)
			
				如果 cppcheck_report.sh 没有得到正确的$XX编译器的include搜索路径列表，需在此修改；
				否则没有必要修改INCLUDEFILES.
				
				手动获取编译器include搜索路径列表方法:
					$ touch ./test.cpp
					$ g++ -v ./test.cpp -o a.out
					$ rm ./test.cpp ./a.out
					在输出结果中:
						#include "..."
						#include <...>
					的下面就是g++程序的头文件搜索列表.

			3. 如果想缩短扫描时间，需要修改Makefile中的$(CPPCHECK_OPT_EX)变量定义。
				--max-configs=12 是 cppcheck 默认配置，如果过大，它将可能会更多的扫描标准库和boost库；值越大，扫描的越慢；
				
				强制完整扫描，添加 --force 选项即可，它还会消除 id=toomanyconfigs的告警，
				但是会非常耗时，绝大部分扫描是没有必要的。
	
				具体参数介绍，参看下面的 后面的附录 cppcheck 部分。
			
			4. 脚本cppcheck_report.sh的执行依赖python运行环境, 如果缺失python, 请先安装python(推荐版本>=v2.7.x)

		2. 使用示例如下：
			1. 初始化cppcheck工具
			
				make init

				如果不是之前卸载了cppcheck，初始化的工作，在一台主机上仅需执行一次即可。
	
				它会完成cppcheck的编译、安装和运行环境的检查的工作。
				cppcheck的安装的位置，参考上面介绍的“手动安装”部分的描述； 
				cppcheck的清理和卸载方法，参考上面的“uninstall”部分的描述。

				如果重复执行初始化(make init)，它会检查cppcheck是否已经安装并检查运行环境. 如果没有通过检查，
				则它会重新再做一次编译安装和运行环境检查的工作。

			2. 生成报告
				1. make # 或者 make all 
					会在当前目录生成项目所有源码的静态检查报告，报告在xxx_report/xxx_report目录内
				
				2. make 源码目录
					生成单个报告				
	
					“源码目录”是相对 $ROOT/src/ 的相对路径
	
					比如：make core/log
					只会在当前目录生成 core/log 目录的静态检查报告:core_report/log_report，
					脚本会对“源码目录”的每一层目录都加上_report后缀。其它项目模块类推。

					再如 make core, 只会在当前目录生成 core 目录的静态检查报告:core_report, 
					core_report包含core源码目录所有源文件的报告，但是不包含 $ROOT/src/core/boost_built的扫描。

			3. 报告清理
				1. make all.clean
					会删除项目所有源码的静态检查报告，并删除 ./std.cfg ./gcc_include_cpp_path;
						* ./std.cfg:               cppcheck程序所需
						* ./gcc_include_cpp_path： 自动化脚本cppcheck_report.sh所需
					如果当前路径存在./core_report或./gui_report目录, 也会将 ./core_report
					和 ./gui_report 一并删除
				
				2. make 源码目录.clean
					单个清理
					
					“源码目录”是相对 $ROOT/src/ 的相对路径		
	
					例如：make core/log.clean
					只会删除 core/log 模块的静态检查报告，其它项目模块类推

附 自动化脚本 cppcheck_report.sh的help:
<pre>
Usage: ./cppcheck_report.sh [ init | $src_dir [$cppcheck_options_ex] ]
        (1) init: Initialize tool - 'cppcheck', including compilation, installation, checking.
        (2) $src_dir is the path relative to directory - '$ROOT/src/'.
        (3) $cppcheck_options_ex is the options of the cppcheck, This script has built-in
           cppcheck parameters: '--xml --std=c++11 --template=gcc --enable=all -I...'
           you can use '$cppcheck_options_ex' to configure cppcheck other additional options.
</pre>
注意，如果$cppcheck_options_ex中有cppcheck的-i选项(ignore)，那么-i接收的参数也必须是一个相对路径（相对./cppcheck_report.sh的路径），这应该是cppcheck的一个bug, 如果给它绝对路径，-i将不起作用。

bug描述：cppcheck 的-i选项仅当 其接收的路径为相对路径，并且$src_dir也是相对路径 的情况下时，-i才会生效

这也是为什么，自动化脚本将$src_dir也限定为相对路径的原因。

---------

附 cppcheck的help
<pre>
Cppcheck - A tool for static C/C++ code analysis

Syntax:
    cppcheck [OPTIONS] [files or paths]

If a directory is given instead of a filename, *.cpp, *.cxx, *.cc, *.c++, *.c,
*.tpp, and *.txx files are checked recursively from the given directory.

Options:
    --append=<file>      This allows you to provide information about functions
                         by providing an implementation for them.
    --check-config       Check cppcheck configuration. The normal code
                         analysis is disabled by this flag.
    --check-library      Show information messages when library files have
                         incomplete info.
    --dump               Dump xml data for each translation unit. The dump
                         files have the extension .dump and contain ast,
                         tokenlist, symboldatabase, valueflow.
    -D<ID>               Define preprocessor symbol. Unless --max-configs or
                         --force is used, Cppcheck will only check the given
                         configuration when -D is used.
                         Example: '-DDEBUG=1 -D__cplusplus'.
    -U<ID>               Undefine preprocessor symbol. Use -U to explicitly
                         hide certain #ifdef <ID> code paths from checking.
                         Example: '-UDEBUG'
    --enable=<id>        Enable additional checks. The available ids are:
                          * all
                                  Enable all checks. It is recommended to only
                                  use --enable=all when the whole program is
                                  scanned, because this enables unusedFunction.
                          * warning
                                  Enable warning messages
                          * style
                                  Enable all coding style checks. All messages
                                  with the severities 'style', 'performance' and
                                  'portability' are enabled.
                          * performance
                                  Enable performance messages
                          * portability
                                  Enable portability messages
                          * information
                                  Enable information messages
                          * unusedFunction
                                  Check for unused functions. It is recommend
                                  to only enable this when the whole program is
                                  scanned.
                          * missingInclude
                                  Warn if there are missing includes. For
                                  detailed information, use '--check-config'.
                         Several ids can be given if you separate them with
                         commas. See also --std
    --error-exitcode=<n> If errors are found, integer [n] is returned instead of
                         the default '0'. '1' is returned
                         if arguments are not valid or if no input files are
                         provided. Note that your operating system can modify
                         this value, e.g. '256' can become '0'.
    --errorlist          Print a list of all the error messages in XML format.
    --exitcode-suppressions=<file>
                         Used when certain messages should be displayed but
                         should not cause a non-zero exitcode.
    --file-list=<file>   Specify the files to check in a text file. Add one
                         filename per line. When file is '-,' the file list will
                         be read from standard input.
    -f, --force          Force checking of all configurations in files. If used
                         together with '--max-configs=', the last option is the
                         one that is effective.
    -h, --help           Print this help.
    -I <dir>             Give path to search for include files. Give several -I
                         parameters to give several paths. First given path is
                         searched for contained header files first. If paths are
                         relative to source files, this is not needed.
    --includes-file=<file>
                         Specify directory paths to search for included header
                         files in a text file. Add one include path per line.
                         First given path is searched for contained header
                         files first. If paths are relative to source files,
                         this is not needed.
    --config-exclude=<dir>
                         Path (prefix) to be excluded from configuration checking.
                         Preprocessor configurations defined in headers (but not sources)
                         matching the prefix will not be considered for evaluation
                         of configuration alternatives
    --config-excludes-file=<file>
                         A file that contains a list of config-excludes
    --include=<file>
                         Force inclusion of a file before the checked file. Can
                         be used for example when checking the Linux kernel,
                         where autoconf.h needs to be included for every file
                         compiled. Works the same way as the GCC -include
                         option.
    -i <dir or file>     Give a source file or source file directory to exclude
                         from the check. This applies only to source files so
                         header files included by source files are not matched.
                         Directory name is matched to all parts of the path.
    --inconclusive       Allow that Cppcheck reports even though the analysis is
                         inconclusive.
                         There are false positives with this option. Each result
                         must be carefully investigated before you know if it is
                         good or bad.
    --inline-suppr       Enable inline suppressions. Use them by placing one or
                         more comments, like: '// cppcheck-suppress warningId'
                         on the lines before the warning to suppress.
    -j <jobs>            Start [jobs] threads to do the checking simultaneously.
    -l <load>            Specifies that no new threads should be started if there
                         are other threads running and the load average is at least
                         load (ignored on non UNIX-like systems)
    --language=<language>, -x <language>
                         Forces cppcheck to check all files as the given
                         language. Valid values are: c, c++
    --library=<cfg>
                         Load file <cfg> that contains information about types
                         and functions. With such information Cppcheck
                         understands your your code better and therefore you
                         get better results. The std.cfg file that is
                         distributed with Cppcheck is loaded automatically.
                         For more information about library files, read the
                         manual.
    --max-configs=<limit>
                         Maximum number of configurations to check in a file
                         before skipping it. Default is '12'. If used together
                         with '--force', the last option is the one that is
                         effective.
    --platform=<type>    Specifies platform specific types and sizes. The
                         available platforms are:
                          * unix32
                                 32 bit unix variant
                          * unix64
                                 64 bit unix variant
                          * win32A
                                 32 bit Windows ASCII character encoding
                          * win32W
                                 32 bit Windows UNICODE character encoding
                          * win64
                                 64 bit Windows
                          * native
                                 Unspecified platform. Type sizes of host system
                                 are assumed, but no further assumptions.
    -q, --quiet          Only print error messages.
    -rp, --relative-paths
    -rp=<paths>, --relative-paths=<paths>
                         Use relative paths in output. When given, <paths> are
                         used as base. You can separate multiple paths by ';'.
                         Otherwise path where source files are searched is used.
                         We use string comparison to create relative paths, so
                         using e.g. ~ for home folder does not work. It is
                         currently only possible to apply the base paths to
                         files that are on a lower level in the directory tree.
    --report-progress    Report progress messages while checking a file.
    --rule=<rule>        Match regular expression.
    --rule-file=<file>   Use given rule file. For more information, see:
                         https://sourceforge.net/projects/cppcheck/files/Articles/
    --std=<id>           Set standard.
                         The available options are:
                          * posix
                                 POSIX compatible code
                          * c89
                                 C code is C89 compatible
                          * c99
                                 C code is C99 compatible
                          * c11
                                 C code is C11 compatible (default)
                          * c++03
                                 C++ code is C++03 compatible
                          * c++11
                                 C++ code is C++11 compatible (default)
                         More than one --std can be used:
                           'cppcheck --std=c99 --std=posix file.c'
    --suppress=<spec>    Suppress warnings that match <spec>. The format of
                         <spec> is:
                         [error id]:[filename]:[line]
                         The [filename] and [line] are optional. If [error id]
                         is a wildcard '*', all error ids match.
    --suppressions-list=<file>
                         Suppress warnings listed in the file. Each suppression
                         is in the same format as <spec> above.
    --template='<text>'  Format the error messages. E.g.
                         '{file}:{line},{severity},{id},{message}' or
                         '{file}({line}):({severity}) {message}' or
                         '{callstack} {message}'
                         Pre-defined templates: gcc, vs, edit.
    -v, --verbose        Output more detailed error information.
    --version            Print out version number.
    --xml                Write results in xml format to error stream (stderr).
    --xml-version=<version>
                         Select the XML file version. Currently versions 1 and
                         2 are available. The default version is 1.
Example usage:
  # Recursively check the current folder. Print the progress on the screen and
  # write errors to a file:
  cppcheck . 2> err.txt

  # Recursively check ../myproject/ and don't print progress:
  cppcheck --quiet ../myproject/

  # Check test.cpp, enable all checks:
  cppcheck --enable=all --inconclusive --std=posix test.cpp

  # Check f.cpp and search include files from inc1/ and inc2/:
  cppcheck -I inc1/ -I inc2/ f.cpp

For more information:
    http://cppcheck.sourceforge.net/manual.pdf
</pre>