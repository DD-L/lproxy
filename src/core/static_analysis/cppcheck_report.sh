#! /bin/bash

ROOT=$(pwd)/../../../
SRCROOT=$ROOT/src/core/
THISDOCDIR=$ROOT/src/core/static_analysis

STD_CFG_FILE='/opt/cppcheck/cfg/std.cfg'
INCLUDEFILES="-I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++/ -I $ROOT/contrib/boost_1_57_0/"

# check parameter
PARA=$1
if [ x"$PARA" = x'' ]
then
	echo "Err: parameter is NULL ($0)"
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

# run cppcheck
cppcheck --xml --std=c++11 --template=gcc --enable=all $INCLUDEFILES -j 4 $SRC_DIR  2> $REPORT_DIR/err.xml

# run cppcheck-htmlreport
cppcheck-htmlreport --file=$REPORT_DIR/err.xml --report-dir=$REPORT_DIR --source-dir=$SRC_DIR

