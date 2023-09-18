#!/bin/sh

function compile () {

macrofile=$1
functionname=$2
filebase=$3
bufname=macrobuffer$functionname

echo generating $filebase.cc 
bin2byte $macrofile "static $bufname" $filebase.cc
cat  >> $filebase.cc <<!

#include "global.h"
BEDPLUGIN


class Editor {
public:
int playmacro(char *buf,int len);
int $functionname(void) {
	return playmacro((char *)$bufname,sizeof($bufname)-1);
	}
};
call($functionname)
!
make $filebase.plug
}


if test $# = 1
then
	compile $1.bedk $1 $1
else
	if test $# = 3
	then
	compile $1 $2 $3
	else
		echo Use $0 base
		echo From macro in file base.bedk makes a plugin called base.plug
		echo with function base
		echo or
		echo Use $0 macrofile functionname base
		echo From macro in file macrofile makes a plugin called base.plug
		echo with function functionname
	fi
fi

