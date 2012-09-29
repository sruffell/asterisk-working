#!/bin/sh
if [ "$1" == "tags" ]; then
	ctags -R .
elif [ "$1" == "cscope" ]; then
	find `pwd -P` -name "*.[ch]" > cscope.files
	cscope -icscope.files -bqk
fi
exit $?
