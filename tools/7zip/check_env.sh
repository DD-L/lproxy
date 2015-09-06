#! /bin/bash

THISDOCDIR=`pwd`
TOOL=$THISDOCDIR/bin/7zr

#check 7zr

if [ -f "$TOOL" ]; then
	chmod +x $TOOL
	if [ -x "$TOOL" ]; then
		echo $TOOL --help
		$TOOL --help
		if [ $? -eq 0 ]; then
			echo -e "checking ./bin/7zr ... ok"
		else
			echo -e "checking ./bin/7zr ... no!"
			exit 1
		fi
	else
		echo -e "\nUnable to execute permissions to $TOOL\n"
		exit 1
	fi
else
	echo "Err: file - $TOOL is missing."
	exit 1
fi

# ok
exit 0

