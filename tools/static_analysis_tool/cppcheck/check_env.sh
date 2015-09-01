#! /bin/sh

#check python
command -v python >/dev/null 2>&1 || { echo >&2 "(./bin/cppcheck-htmlreport)Requires python 2.7.x or higher, but it's not installed.  Aborting."; exit 1; }

#check cppcheck
if [ -f './bin/cppcheck' ];then
	chmod +x ./bin/cppcheck
	if [ -x './bin/cppcheck' ];then 
		echo -e "checking ./bin/cppcheck ... ok"
	else
		echo -e "\nUnable to execute permissions to ./bin/cppcheck\n"
		exit 1
	fi
else
	echo "Err: file - ./bin/cppcheck is missing."
	exit 1
fi

#check ./bin/cfg/std.cfg
if [ -f './bin/cfg/std.cfg' ];then
	echo -e "checking ./bin/cfg/std.cfg ... ok"
else
	echo "Err: file - ./bin/cfg/std.cfg is missing."
	exit 1
fi

#check cppcheck-htmlreport
if [ -f './bin/cppcheck-htmlreport' ];then
	echo -e "\npython ./bin/cppcheck-htmlreport"
	python ./bin/cppcheck-htmlreport
	if [ $? -eq 0 ];then
		echo -e "checking ./bin/cppcheck-htmlreport ... ok\n"
		exit 0
	else
		echo "Perhaps it is missing python module - 'pygments', run 'sudo pip install pygments' can install it."
		command -v pip >/dev/null 2>&1 || { echo >&2 "it requires pip, pleaes install 'pip' firstly, and then run 'sudo pip install pygments'."; }
		exit 1
	fi
else
	echo "Err: file - ./bin/cppcheck-htmlreport is missing."
	exit 1
fi

