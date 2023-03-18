#include "defines.h"
/*     Bed a menu-driven multi dataformat binary editor for Linux            */
/*     Copyright (C) 1998 Jaap Korthals Altes <binaryeditor@gmx.com>       */
/*                                                                           */
/*     This program is free software; you can redistribute it and/or modify  */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     This program is distributed in the hope that it will be useful,       */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with this program; if not, write to the Free Software           */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/* Fri Dec  8 22:14:27 2000                                                  */
#include <stdlib.h>
#include <alloca.h>
#include <time.h>
#include <stdint.h>
#include "editor.h"
#include "macros.h"
#include "main.h"
#include "system.h"
#include "screen.h"
#include "config.h"
#ifdef _WIN32
#include "win.h"
#endif
extern int ggetch(WINDOW *w) ;
int Editor::therepeat(int nr) {
	int *keys,maxkeys;
	int nrkeys=0;
	maxkeys=80;
	keys=myallocar(int ,maxkeys+1);
	keys[nrkeys]=ggetch(editscreen);
#ifndef PDCURSES
	keys[++nrkeys]=ggetch(editscreen);

//Alt-e,n: 27 101 110
#ifdef USE_KEYPAD
	while(!(keys[nrkeys-1]==27&&keys[nrkeys]==182)) {
#else
	while(!(keys[nrkeys-1]==27&&keys[nrkeys]==46)) {

#endif
#else
//Alt-e,n: 421 110
	while(keys[nrkeys]) {
#endif
		if(nrkeys>=maxkeys) { 
			int *tmp=keys;
			maxkeys*=2;
			keys=myallocar(int ,maxkeys+1);
			memmove(keys,tmp,(nrkeys+1)*sizeof(int));
			myfree(tmp);
			}
		keys[++nrkeys]=ggetch(editscreen);
		}
#ifndef PDCURSES
	nrkeys--;
#endif
	if(nrkeys==0) {
		myfree(keys);
		output("no keys given");
		return -1;
		}
	message("Repeat %d times",nr);
	for(int i=0;i<nr;i++) 
		for(int j=0;j<nrkeys;j++)
			playkeys.add(keys[j]);
//	edits[editfocus]->processkey();
	myfree(keys);
	return 0;
	}

extern int mytmpname(char *buf,int len,const char *name) ;
Keystrokes *recordkeysptr=NULL;
#define recordkeys (*recordkeysptr)
static int menupos=-1;
void setoffrecording(void) {
	if(isrecording==1) {
		showstatestate(" ",1,menupos);
		}
	isrecording=0;
}

int Editor::record(void) {
	if(isrecording==1) {
		setoffrecording() ;
		if((*(recordkeys.inp))>3) {
			(*(recordkeys.inp))-=3; /* more general?*/
			isrecording=2;
			output("End recording");
			erefresh();
			return saverecord();
			}
		else {
			recordkeys.flush();
			output("End recording, nothing saved");
			return 0;
			}
		}
	if(isrecording) {
		switch(getkeyinput("Overwrite previously recorded ? (y/n)")) {
			case 'y':;
			case 'Y': break;
			case 'n':;
			case 'N': ;
			default: message("Cancelled");return -1; 
				};
		}
	if(menupos<0)
		menupos=showstatestate("*",1)-1;
	else
		showstatestate("*",1,menupos);
	if(!recordkeysptr)	{
const char endname[]="bedrecord-XXXXXX";
		char *name=(char *)malloc(BEDSMAXPATH);
		mytmpname(name,BEDSMAXPATH,endname);
		int len=strlen(name)+1;
		name=(char *)realloc(name,len);
		recordkeysptr=new Keystrokes(name);
		}
	else 
		recordkeys.flush();
//#ifdef _WIN32
#ifdef __CYGWIN__
char tmp[MAX_PATH];
	winpath(recordkeys.getname(),tmp); 
#else
const char *const tmp=recordkeys.getname();
#endif
	message("Recording to %s, press Control-t to end",tmp); 
	isrecording=1;
	return 0;
	}

Editor *getactive(void);

extern int	endrecording(void) ;
#if 0
int Editor::playrecord(void) {
	if(!recordkeysptr||!recordkeys.pending()) {
		message("No keys recorded");
		return -1;
		}
	if(isrecording==1) {
		switch(getkeyinput("You are recording. Do you want an endless loop? (y/n)")) {
			case 'y': case 'Y':
				break;
			case 'n': case 'N': ;
			default: (*(recordkeys.inp))-=3;
				};
		};
	playkeys.fill(recordkeys);	
	while(playkeys.pending())
		getactive()->processkeyonce();
	return 0;
	}
#else
int Editor::playrecord(void) {
	if(!recordkeysptr||!recordkeys.pending()) {
		message("No keys recorded");
		return -1;
		}
	int loop=0;
	if(isrecording==1) {
		switch(getkeyinput("Recording is already on. Do you want an endless loop? (y/n)")) {
			case 'y': case 'Y': loop=1; /*the same as the endless recursion, without segmentation violation*/ 
			case 'n': case 'N': ;
	//		default: (*(recordkeys.inp))-=3;
			default: (*(recordkeys.inp))-=4;
				};
		};
	
do  {
	playkeys.fill(recordkeys);	
	while(playkeys.pending())
		getactive()->processkeyonce();
		/*
			if(getactive()->processkeyonce()<0) {
				playkeys.flush();
				return -1;
			}
			*/
	} while(loop);
	return 0;
	}
#endif


       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

/*
0: regstring 11
20: bedversion 10
38: datum 8
46: aantal keystrokes

the keystrokes
end 9
*/
//

#ifdef USE_PREAD 
#ifdef _WIN32
#define LPWRITE mypwrite64
BEDS_SIZE_T mypwrite64(FILEHANDLE outputhandle,const char *buf,BEDS_SIZE_T nr,OFFTYPE offset) ;
#endif
#define offwrite(handle,off,str, len) (LPWRITE(handle,str,len,off)==len)
#else
#ifdef _WIN32
extern OFFTYPE setfilepos(FILEHANDLE file,OFFTYPE offset) ;
#define	offwrite(handle,off,str, len)  ((setfilepos(handle,off) == off)?writetofile(handle,str,len)==len:0)
#else
#define	offwrite(handle,off,str, len) ((lseek(handle,off,SEEK_SET)==off)?write(handle,str,len)==len:0)
#endif
#endif

#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define returnmessage(x...) {message( ## x);beep();erefresh();return -1;} VARMACRO
#else
#define returnmessage(...) {message(__VA_ARGS__);beep();erefresh();return -1;} 
#endif

char macrosign[]="\276\320bed macro";
char macroend[]="\276\320bed end";
const int macrolen=sizeof(macrosign)-1;
const int macroendlen=sizeof(macroend)-1;
extern const int startmacrodata;
const int startmacrodata=50;

int Editor::playmacro(char *buf,int end) {
	if(memcmp(buf,macrosign,macrolen)) {
		returnmessage("not a bed macro");
		}
	int nrkeys;
	if(memcmp(buf+end-macroendlen,macroend,macroendlen)) {
		nrkeys=*((int *)(buf+46));
		if(nrkeys<3||((nrkeys*(int)sizeof(Strokekeytype))>(end-startmacrodata))|| (getkeyinput("End is missing, continue?")|32)!='y')
			returnmessage("No end cancelled");
		nrkeys-=2;		
		}
	else 
		nrkeys=(end-startmacrodata-macroendlen)/sizeof(Strokekeytype);
	if(nrkeys<1) {
		message("No keys recorded");return -1;
		}
	playkeys.fill((Strokekeytype *)(buf+startmacrodata),nrkeys);
	while(playkeys.pending())
		getactive()->processkeyonce();
	return 0;
	}
int Editor::playmacrofile(const char *name) {
	int handle;
	if((handle=open(name, O_RDONLY BINARY_FLAG))<0) {
		message("Can't open %s: %s",name,strerror(errno));
		wrefresh(editscreen);
		return -1;
		}
       off_t end;
	end=lseek(handle, 0L, SEEK_END);
	if(end<macrolen) {
		returnmessage("not a bed macro");
		}	
	char *buf=myallocar(char,end);
	lseek(handle, 0L, SEEK_SET);
	ssize_t res;
	if((res=read(handle,buf,end))!=end) {
		message("read(%s...)=%d !=%d",name,res,end); 
		}
	else
		playmacro(buf,end);
	myfree(buf);
	return 0;
	}


extern void                     mychdir(const char *path);
extern char	*mygetcwd(char *name,int len) ;


char macrodirectory[BEDSMAXPATH]=".";	
int Editor::playkeysfile(void) {
	char buf[BEDSMAXPATH];
	char name[BEDSMAXPATH];
	char cwdold[BEDSMAXPATH];
	const char *ant="*.bedk";
	strcpyn(buf,ant,BEDSMAXPATH);
	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(macrodirectory);
	do {
		ant=getfile("Play key strokes",buf) ;
		editup();
		mygetcwd(macrodirectory,BEDSMAXPATH);
		if(!ant) {
			mychdir(cwdold);
			return -1;
			}
		if(!arealpath(ant,name))
				continue;
		mychdir(cwdold);
		} while( playmacrofile(name)<0);
	return 0;
	}
/*	
int writemacroheader(FILEHANDLE handle) {
	char *empty=(char *)alloca(startmacrodata);
	memset(empty,0xAF,startmacrodata);
	offwrite(handle,0,empty,startmacrodata);
	if(!offwrite(handle,0,macrosign, macrolen)) {
		return -1;
		}
	const char vers[]=VERSION;
	offwrite(handle,16,vers, minnum(sizeof(vers),14));
	offwrite(handle,30,(char *)&pid, sizeof(pid));
	uint64_t  tim=0LL;
	time((time_t *)&tim);
	offwrite(handle,38,(char *)&tim,8);
	return 0;
	}
*/
	
ssize_t getmacroheader(char  *macroheader) {
	memset(macroheader+macrolen,0xAF,startmacrodata-macrolen);
	memcpy(macroheader,macrosign, macrolen) ;
	const char vers[]=VERSION;
	memcpy(macroheader+16,vers, minnum(sizeof(vers),14));
	int32_t uitpid=(int32_t)pid;
	memcpy(macroheader+30,(char *)&uitpid, 4);
	uint64_t  tim=0LL;
	time((time_t *)&tim);
	memcpy(macroheader+38,(char *)&tim,8);
	return startmacrodata;
	}
inline int writemacroheader(FILEHANDLE handle) {
	char *macroheader=(char *)alloca(startmacrodata);
	getmacroheader(macroheader);
	return offwrite(handle,0,macroheader, startmacrodata);
	}

/*
int writemacroheader(FILEHANDLE handle) {
}
*/

int Editor::saverecord(void) {
	if(!recordkeysptr||!recordkeys.pending()) {
		message("No keys recorded");
		return -1;
		}
	
	char buf[BEDSMAXPATH];
	char cwdold[BEDSMAXPATH];
	const char *ant="*.bedk";
	FILEHANDLE handle;
	strcpyn(buf,ant,BEDSMAXPATH);
	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(macrodirectory);

	setoffrecording();
	do {
		ant=getfile("Save key strokes",buf) ;
		editup();
		if(!ant) {
			mygetcwd(macrodirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return -1;
			}
		} while((handle=openfile(ant))==MY_INVALID_HANDLE_VALUE);
	mygetcwd(macrodirectory,BEDSMAXPATH);
	mychdir(cwdold);
	uint32_t nrkeys=*(recordkeys.inp)-recordkeys.out;
	ssize_t len=sizeof(Strokekeytype)*nrkeys;

	if(writemacroheader(handle)<0) {
		message("write macroheader failed");
		closehandle(handle);
		return -1;
		}
	offwrite(handle,46,(char *)&nrkeys,4);
	setfilepos(handle, startmacrodata);
	if(writetofile(handle,recordkeys.buf+recordkeys.out,len)!=len) {
		message("Error writing to %s",ant);
		closehandle(handle);
		return -1;
		}
	writetofile(handle,macroend,macroendlen);
	closehandle(handle);
//	setoffrecording();
	message("Keys strokes saved to %s",ant);
	return 0;
	}


int isrecording=0;

Strokekeytype endreco[]={20  };
int endrecolen=sizeof(endreco)/sizeof(Strokekeytype);
int addtorecordkeys(int key) {
	recordkeys.add( key);
	int in=recordkeys.pending();
	if(in>endrecolen&&!memcmp(endreco,recordkeys.buf+*(recordkeys.inp)-endrecolen,sizeof(endreco))) {
		 setoffrecording() ;
		if((*(recordkeys.inp))>endrecolen) {
			(*(recordkeys.inp))-=endrecolen;
			isrecording=2;
			}
		edits[editfocus]->output("End recording");
		edits[editfocus]->erefresh();
		return KEYSTROKE_ERROR;
		}
	return key;
	}
extern int isrecording;
#include "keystrokes.h"
int	endrecording(void) {
	int ret=0;
	if(isrecording==1) {
		if((*(recordkeysptr->inp))>2) {
			isrecording=2;
			int ans;
			if((ans=(edits[editfocus]->getkeyinput("Save the keysstrokes you are recording (quit will be included)? (y/n/c)")))==27||(ans|=32)=='c') {
				isrecording=1;
				return -1;
				}
			if(ans=='y') {
		//		(*(recordkeysptr->inp))-=2;
				ret= edits[editfocus]->saverecord();	
				if(ret<0) {
					isrecording=1;
		//			(*(recordkeysptr->inp))+=2;
					return ret;
					}
				}
			}
		}
	setoffrecording();
	if(recordkeysptr)
		delete recordkeysptr;
	recordkeysptr=NULL;
	return  ret;
	}
