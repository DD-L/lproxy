#! /bin/bash

#
# cppcheck_report [init | $src_dir [ $cppcheck_options_ex ] ]
#	init 初始化cppcheck工具。 
#		只要不是卸载了工具，初始化一次即可, init 会自动安装工具
#	$src_dir 是相对 $SRCROOT 的路径
#	$cppcheck_options_ex 是cppchek的选项，此脚本已经内置了一些
#		选项: '--xml --std=c++11 --template=gcc --enable=all -I...'
#		如果想添加其他的选项，设置此参数即可
#  
#

XX=g++
THISDOCDIR=$(pwd)
ROOT=$THISDOCDIR/../../../
SRCROOT=$ROOT/src/
BOOST_INCLUDE="$ROOT/contrib/boost_1_57_0/"

CPPCHECK_TOOL_DIR=$ROOT/tools/static_analysis_tool/cppcheck/
STD_CFG_FILE=$CPPCHECK_TOOL_DIR/bin/cfg/std.cfg
CPPCHECK_CMD=$CPPCHECK_TOOL_DIR/bin/cppcheck
CPPCHECK_HTMLREPORT_CMD=$CPPCHECK_TOOL_DIR/bin/cppcheck-htmlreport


# check parameter
PARA=$1
# 参数不可为空
if [ x"$PARA" = x'' ]
then
	echo -e "\nErr: parameter is NULL ($0)\n"
	echo -e "Usage: $0 [ init | \$src_dir [\$cppcheck_options_ex] ]"
	echo -e "\t(1) init: Initialize tool - 'cppcheck', including compilation, installation, checking."
	echo -e "\t(2) \$src_dir is the path relative to directory - '$SRCROOT'."
	echo -e "\t(3) \$cppcheck_options_ex is the options of the cppcheck, This script has built-in"
	echo -e "\t   cppcheck parameters: '--xml --std=c++11 --template=gcc --enable=all -I...'"
	echo -e "\t   you can use '\$cppcheck_options_ex' to configure cppcheck other additional options. "
	exit -1
else
	# 参数不可以是绝对路径
	if [[ $PARA =~ ^/ ]]; then
		echo -e "\nErr: \$src_dir='$PARA' - parameter is an absolute path."
		echo -e "     \$src_dir must be a path that relatives to directory - '$SRCROOT'.\n"
		exit -1
	fi
fi

#init tools
if [ x$PARA = x"init" ];then
	cd $CPPCHECK_TOOL_DIR
	make check
	if [ $? -eq 0 ];then
		echo -e "\ncppcheck: Initialization has been completed.\n"
		cd $THISDOCDIR
		exit 0
	else # not found tool
		make 
		if [ $? -eq 0 ]; then
			cd $THISDOCDIR
			echo -e "\ncppcheck: Initialization has been completed.\n"
			exit 0
		else
			cd $THISDOCDIR
			exit 1
		fi
	fi
fi

# set $cppcheck_options_ex
CPPCHECK_OPTIONS_EX=""
if [ $# -gt 1 ];then
	for para_c in $(seq 2 $# )
	do
		CPPCHECK_OPTIONS_EX="$CPPCHECK_OPTIONS_EX ${!para_c}"
	done
fi

#check TOOLS
cd $CPPCHECK_TOOL_DIR
make check
if [ $? -ne 0 ];then
	cd $THISDOCDIR
	echo -e "\ncppcheck Err: tools is missing, please run 'make init' to reinstall tools\n"
	exit 1
fi
cd $THISDOCDIR

# setting INCLUDEFILES
INCLUDEFILES=""
TMPCPPFILE="./tmp_cpplibpath.cpp"
INCLUDE_CPP_PATHS_FILE="./gcc_include_cpp_path"
if [ ! -f $INCLUDE_CPP_PATHS_FILE ];
then
	# get include path
	echo -e "#include<iostream>\nint main() { return 0; }" > $TMPCPPFILE

	$XX -v $TMPCPPFILE -o a.out.tmp 2>&1 | tee tmp_XX > /dev/null
	rm -rf $TMPCPPFILE a.out.tmp

	echo "$XX include search list(Standard):"
	(sed -n '/^#include /,/^GNU /p' tmp_XX | grep "/" | tee $INCLUDE_CPP_PATHS_FILE) && rm tmp_XX
fi
while read line
do
	INCLUDEFILES="$INCLUDEFILES -I $line/ "
done < $INCLUDE_CPP_PATHS_FILE
INCLUDEFILES="$INCLUDEFILES -I $BOOST_INCLUDE -I $SRCROOT"


#INCLUDEFILES="-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++/ \
#	-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++/i686-pc-cygwin/ \
#	-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++/backward/ \
#	-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/ \
#	-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include-fixed/ \
#	-I /usr/include/ \
#	-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/../../../../include/w32api/ \
#	-I $BOOST_INCLUDE -I $SRCROOT"



# config src_dir
# SRC_DIR 最好是相对路径, 否则会引起cppcheck 的-i选项会失效，
# 并且-i参数接收的也只能是相对路径(相对$ROOT/src), 这是cppcheck的一个bug
SRC_DIR=../../$PARA
SRC_FULL_PATH=$SRCROOT/$PARA

# config report_dir
NORMPATH=`python -c "import os.path; print(os.path.normpath('$PARA'))"` # 去除点路径
# 计算出正常路径后，对每一层目录都加上"_report"后缀
REPORT_DIR=$THISDOCDIR/${NORMPATH//\//_report\/}"_report"
[ -d $REPORT_DIR ] || mkdir -p $REPORT_DIR

# check std.cfg
if [ ! -f $THISDOCDIR/std.cfg ]
then
	cp $STD_CFG_FILE $THISDOCDIR/
fi


if [ -d $SRC_DIR ]
then
	# run cppcheck
	$CPPCHECK_CMD --xml --std=c++11 --template=gcc --enable=all $INCLUDEFILES $CPPCHECK_OPTIONS_EX $SRC_DIR 2> $REPORT_DIR/err.xml

	# run cppcheck-htmlreport
	#$CPPCHECK_HTMLREPORT_CMD --file=$REPORT_DIR/err.xml --report-dir=$REPORT_DIR --source-dir=$SRC_DIR
	# 由于$SRC_DIR是相对路径, --source-dir 不必给值，如果给也是给当前路径 ./ , 即: --source-dir=.
	$CPPCHECK_HTMLREPORT_CMD --file=$REPORT_DIR/err.xml --report-dir=$REPORT_DIR
	
	exit 0
else # error
	echo "Err: source-dir='$SRC_DIR' directory does not exist."
	exit -1
fi
