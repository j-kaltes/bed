#include "global.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "config.h"
#include "varmacro.h"
#include CURSES_HEADER

//#define DEBUGGING(...) fprintf(stderr,__VA_ARGS__)
#ifndef DEBUGGING
#ifdef OLDGCCVARMACRO
#define DEBUGGING(var...) 
#else
#define DEBUGGING(...) 
#endif
#endif

#define MAXLEN 50
BEDPLUGIN
#define minnum(x,y) ((x)<(y)?(x):(y))
extern Editor *getselectioneditor(void);
#ifdef _WIN32
#define ALIGNMEMORY
#endif
#include "../../src/alignmem.h"
class Editor {
#include "faked.h"
public:
int	searchclip(void) {
	char *ptr;
	int len=getselbuf(	&ptr);
//	int uit=creat("selectie.dat",0777); write(uit,ptr,len); close(uit);
	const OFFTYPE startpos=getfilepos()+geteditpos();
	const OFFTYPE fsize=filesize();
	if(len<=0) {
		message("Nothing in copy buffer");
		beep();
		return -1;
		}
	BLOCKTYPE BLOCKSIZE=blocksize();
	int takelen=minnum(len,BLOCKSIZE);
	char *buf=(char *)alignedalloc(BLOCKSIZE,BLOCKSIZE);
OFFTYPE res=startpos;
	for(;;){
FURTHER:
		res=dosearch(res,ptr,takelen);
		 if(res==OFFTYPEINVALID) {
		 	DEBUGGING("Not found");	
		 	topos(startpos);
			message("Content copy buffer not found");
			beep();
			alignedfree(buf);
			return -1;
		 	}
		DEBUGGING("Found start " OFFPRINT,res);	
		OFFTYPE compstart=res+takelen;
		int inblock=compstart%BLOCKSIZE;
		BLOCKTYPE bl=compstart/BLOCKSIZE;
		int max=minnum(len,fsize-res);

		for(int pos=takelen;pos<max;pos+=(BLOCKSIZE-inblock),inblock=0) {
			int size=minnum(BLOCKSIZE-inblock,max-pos);
			int take=getblock(bl++,buf);
//			getmem(res+pos,buf,size);
			if(take<size||memcmp(buf+inblock,ptr+pos,size)) {
				DEBUGGING("partial match " OFFPRINT" to %d" ,res,pos );	
				if(res>=startpos)
					res++;
				else
					res--;
				goto FURTHER;	
				}
			}
		alignedfree(buf);
		if(max<len) {
			message("Reached end of file matched for %ld bytes at " OFFPRINT,max,res);
			beep();
			return -1;
			}
		else 
			message("Found content copy buffer at " OFFPRINT " (#%ld)",res,len);
		topos(res);
		return 0;
		}
	
	}
int compare(void) {
	Editor *ed;
	message("Compare selection to current file from cursor position ...");
	if((ed=getselectioneditor())) {
		OFFTYPE len,start;
		if((len=ed->getselregion(&start))) {
			BLOCKTYPE BLOCKSIZE=blocksize();
			OFFTYPE curpos=getfilepos()+geteditpos();
			OFFTYPE fsize=filesize(),left=fsize-curpos;
			OFFTYPE end=minnum(len,left);
			for(OFFTYPE pos=0;pos<end;pos+=BLOCKSIZE) {
				char bufhere[BLOCKSIZE],bufthere[BLOCKSIZE];
				int size=minnum(BLOCKSIZE,end-pos);
				ed->getmem(start+pos,bufthere,size);
				getmem(curpos+pos,bufhere,size);
				if(memcmp(bufhere,bufthere,size)) {
					int j=0;
					for(;bufhere[j]==bufthere[j];j++)
						;
					message("Differ at offset " OFFPRINT " in selection, offset " OFFPRINT " in this file",pos+j,curpos+pos+j);
					return -1;
					}
				}
			if(len>left) {
				message("End of file reached in current file " OFFPRINT " bytes equal",fsize-curpos);
				return -2;
				}
			char name[512];
			message("Selection %s (" OFFPRINT ") equal to " OFFPRINT "-" OFFPRINT ".",ed->getfilename(name),start,curpos,curpos+len-1);
			return 0;

			}
		else 
			message("Can't find nonzero selection");
		}
	message("Can only use last made selection");
	return -3;
	}
#ifdef TEST
int testsclip(void) {
	int BLOCKSIZE=blocksize();
	char buf[BLOCKSIZE];
	OFFTYPE size=filesize(),iter;
	int error=0;
	srandom(time(NULL));
	for(iter=(OFFTYPE)0;iter<size;iter+=random()%100) {
		int len= minnum(random()%BLOCKSIZE,size-iter);
		getmem(iter,buf,(OFFTYPE)len);
		putselbuf(buf,len);
		topos((OFFTYPE)0);
		if(searchclip()<0) {
			getkeyinput("Didn't find " OFFPRINT "#%ld <press return>",iter,len);
			error++;
			}
		}
	message("%d errors",error);
	return error;
	}
#endif
};
sethelp(compare,"Compares last made selection with current file at current cursor\nposition for the length of that selection.\nBed itself has no limit on selection size, but some Xwindows programs force\nsuch limits on others. See the help of makeselection.") 
call(compare)


sethelp(searchclip,"Searches in current file for the content of bed's copy buffer.\nIt takes over some search settings (Direction) of Alt-m,s.\nAnd sets others: Case sensitive, Non-Regex, non-aligned, from cursor position, not within Units.\nCurrently it repeatedly searches for the first block of the copy buffer\nand then compares it with the whole copy buffer.") 
call(searchclip)
#ifdef TEST
call(testsclip)
#endif
