#include <stdio.h>
#include <w32api/windef.h>
#include <sys/cygwin.h>
#include "win.h"
int main() {
const char *  const in=CONFDIR;
char tmp[MAX_PATH];
char out[MAX_PATH];
winpath(in ,tmp);
int over=MAX_PATH;
for(char *ptr,*inptr=tmp,*outptr=out;(ptr= (char *)memccpy(outptr,inptr,'\\',over));) {
	ptrdiff_t pos=	ptr-outptr;
	*ptr++='\\';
	outptr=ptr;
	inptr+=pos;
	over-=pos;
	over--;
	}
/*
fputs("#define CONFDIRWIN ",stdout);
fputs(out,stdout);
*/
fprintf(stdout,"#define CONFDIRWIN \"%s\"\n",out);
return 0;
}
