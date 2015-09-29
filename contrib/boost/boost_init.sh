#! /bin/bash

#
# Usage:
#	./boost_init.sh [ -force ]
#		-force: Forced initialization
#


THISDOCDIR=`pwd`
ROOT=$THISDOCDIR/../../


BOOST_PACKAGE=boost_1_57_0.7z
BOOST_SRC=./boost_1_57_0
SUCCESS_FLAG=$THISDOCDIR/$BOOST_SRC/boost/success_flag

_7ZR_TOOL_DIR=$ROOT/tools/7zip/
_7ZR_TOOL=$_7ZR_TOOL_DIR/bin/7zr

# show help
show_help ()
{
	echo -e "Usage:\n\t./boost_init.sh [ option ]\n"
	echo "option:"
	echo -e "  -h, --help:\tshow help"
	echo -e "  -force:\tForced initialization"
}

# check boost
check_boost () 
{
	if [ -f $SUCCESS_FLAG ];then
		echo "info: checking $SUCCESS_FLAG ... ok"
		exit 0
	else
		echo "$SUCCESS_FLAG is missing..."
	fi
}



PARA=$1
if [ x$PARA = x"-h" -o x$PARA = x"--help" ]; then
	show_help
	exit 0
fi
if [ ! x$PARA = x"-force" ]; then
	# check boost
	check_boost
fi


# check 7z tools

# 7z 7za 7zr
UNPACK_TOOL="unknown"
for tool in 7z 7za 7zr
do
	command -v $tool >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		UNPACK_TOOL=$tool
	fi
done

if [ $UNPACK_TOOL = "unknown" ];then
	# not found '7z 7za 7zr'
	echo -e "\nwarning: These tools were not found in OS: 7z / 7za / 7zr, then, try to make one..."
	
	cd $_7ZR_TOOL_DIR
	make check
	if [ $? -eq 0 ]; then
		UNPACK_TOOL=$_7ZR_TOOL
	else
		make
		if [ $? -eq 0 ]; then
			UNPACK_TOOL=$_7ZR_TOOL
		else
			cd $THISDOCDIR
			echo -e "\nErr: '$_7ZR_TOOL' installation failed, please extract $THISDOCDIR/$BOOST_PACKAGE to $THISDOCDIR/ by yourself."
			exit 1
		fi
	fi
	cd $THISDOCDIR
fi

# unpack boost.7z
rm -rf $BOOST_SRC 
echo "Extracting $BOOST_PACKAGE"
$UNPACK_TOOL x $BOOST_PACKAGE  
if [ $? -eq 0 ]; then
	touch $SUCCESS_FLAG
fi
# check boost
ret=`check_boost`
if [ x"$ret" = "$SUCCESS_FLAG is missing..." ]; then
	echo "Err: checking $THISDOCDIR/$BOOST_SRC/boost ... no!"
	exit 1
fi
exit 0
