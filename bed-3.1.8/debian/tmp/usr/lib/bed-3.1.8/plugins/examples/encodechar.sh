#!/bin/bash
if test $# -ne 2
then
	echo usage $0 FROM TO 
	echo for example: $0 ebcdic ascii
	echo 'makes a plugin source which converts between character sets FROM and TO'
	echo 'to header files are then needed: FROM2TO.h and TO2FROM.h (ebcdic2ascii.h'
	echo 'and ascii2ebcdic.h) FROM2TO.h should contain a character array FROM2TO'
	echo 'which has at position i the conversion from i to the characterset TO'
	echo etc
	exit 2
fi
from=$1
to=$2
two=2
cat > $from$two$to.cc <<!
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include "../screenpart.h"
#include "../ascii.h"
#include <limits.h>
#include <ctype.h>

static
#include "$from$two$to.h"
static
#include "$to$two$from.h"

#define datadecs\
	chconv($to,$from)\
	chconv($from,$to)
#undef chconv
#define chconv(from,to) \
class  from##to : public Ascii {	\
	private:\
	public:	 from##to ( Editor *ed,int x1=0, int y1=0): Ascii( ed,x1, y1)	{\
	};	\
	declareparent \
	int addkey(unsigned char *bin,int binlen, int key, int pos ) {\
			bin[pos]=::to##2##from[key];\
			return 0;\
			}\
	int form2bin(char *formal, unsigned char *bin, int &len) {	\
		for(int i=0;i<len;i++)\
			bin[i]=::to##2##from[(unsigned char)formal[i]];\
		return len;\
		}\
	ScreenPart *newone(void) {return new from##to(*this);}\
	int bin2formal(unsigned char *bin, char *formal, int len) {	\
		for(int i=0;i<len;i++) {\
			formal[i]=::from##2##to[bin[i]];\
			if(!isel(formal[i]))\
				formal[i]=nonprint;\
			}\
		return 0;	}	\
	};\

datadecs
#undef chconv
#define chconv(from,to) parentdef(Ascii, from##to, #from"2"#to)
datadecs
#undef chconv
#define chconv(from,to) datatypename(from##to, from##to::thename)
#include "../typelist.h"
datadecs
!
echo Generated $from$two$to.cc
