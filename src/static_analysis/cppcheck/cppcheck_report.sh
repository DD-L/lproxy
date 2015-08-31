#! /bin/bash

#
# cppcheck_report $src_dir
#  $src_dir 是相对 $SRCROOT 的路径
#

XX=g++
THISDOCDIR=$(pwd)
ROOT=$THISDOCDIR/../../../
SRCROOT=$ROOT/src/

STD_CFG_FILE='/opt/cppcheck/cfg/std.cfg'
BOOST_INCLUDE="$ROOT/contrib/boost_1_57_0/"

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

# check parameter
PARA=$1
if [ x"$PARA" = x'' ]
then
	echo -e "\nErr: parameter is NULL ($0)\n"
	echo -e "Usage: $0 src_dir"
	echo -e "\tsrc_dir is the path relative to directory - '$SRCROOT'.\n"
	exit -1
fi
# config src_dir
SRC_DIR=$SRCROOT/$PARA
# config report_dir
REPORT_DIR=$THISDOCDIR/$PARA"_report"
[ -d $REPORT_DIR ] || mkdir -p $REPORT_DIR

# check std.cfg
if [ ! -f $THISDOCDIR/std.cfg ]
then
	cp $STD_CFG_FILE $THISDOCDIR/
fi

if [ -d $SRC_DIR ]
then
	# run cppcheck
	cppcheck --xml --std=c++11 --template=gcc --enable=all --force $INCLUDEFILES -j 4 $SRC_DIR  2> $REPORT_DIR/err.xml

	# run cppcheck-htmlreport
	cppcheck-htmlreport --file=$REPORT_DIR/err.xml --report-dir=$REPORT_DIR --source-dir=$SRC_DIR
	
	exit 0
else # error
	echo "Err: source-dir='$SRC_DIR' directory does not exist."
	exit -1
fi
