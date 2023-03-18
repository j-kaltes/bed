#ifndef VIEWBUF_H
#define VIEWBUF_H 1
#ifdef WINALLOC
#include <windows.h>
#include "win.h"
#else
#include <stdlib.h>
#endif
//#include "memory.h"
#include "editor.h"
class Editor;
class Viewbuf {
private:
Memory &mem;
OFFTYPE offset,endoffset;
int start,  shown, blnr,minbl;
char *str,*tmpbuf;
public:
Viewbuf(Editor &e,int size=0);
Viewbuf(Memory &m,int size=0):mem(m), offset(OFFTYPEINVALID),endoffset(0),start(0),shown(0),blnr(0),minbl((m.blocksize()?(size-1)/m.blocksize():0)+2),str(NULL),tmpbuf(NULL){ }
char *view(void) {return str+start;}
void setstart(int st) { start=st;}
int getstart(void) {return start;}
int getshown(void) { return shown;}
int setshown(int sh) { return shown=sh;}
char *buf(void) {return str;}
int getnr(void) {return blnr;}
OFFTYPE getendoffset(void) {return endoffset;}
OFFTYPE getoffset(void) {return offset;}
void free(void) {
if(str)
	alignedfree(str);
str=NULL;
}

OFFTYPE fill(OFFTYPE pos,int len) ;
OFFTYPE retain(OFFTYPE pos) ;
char *getdata(OFFTYPE pos,int len) ;
~Viewbuf() {
free();
if(tmpbuf)
	::free(tmpbuf);
} 


};
#endif
