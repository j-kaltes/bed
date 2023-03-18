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
#include "desturbe.h"
#include <stdarg.h>
#include "editor.h"
#include "inout.h"
#include "readconfig.h"
#include "main.h"
#include "screen.h"
#include "offtypes.h"
#include "system.h"

//#define LOGMESSAGE
#ifdef LOGMESSAGE
extern char *mytmpname(char *buf,int len,const char *name) ;
const int maxlogname=256;
char logname[maxlogname];
void rmlogtemp(void) ;
int logmessup(void) {
	if(mytmpname(logname,maxlogname,"bedmessagesXXXXXX")) {
		int handle=open(logname, O_WRONLY|O_CREAT, 0600);
		if(handle>=0)
			atexit(rmlogtemp);
		return handle;
		}
	else
		return -1;
	}
const int loghandle=logmessup();
void rmlogtemp(void) {
	if(loghandle>-1) {
		close(loghandle);
		unlink(logname);
		}
	}
#endif
extern char messagebuf[];
extern const int maxmessagebuf;
const int maxmessagebuf=200;
char messagebuf[maxmessagebuf]="";
 void Editor::clearmessage(void) { messagebuf[0]=0; if(nomessage) textovermessage();else fileinfo(); }
 int Editor::output(const char info[]) {
	strcpyn(messagebuf,info,maxmessagebuf);
	return realoutput(messagebuf);
	}
 int Editor::realoutput(const char info[]) {
#ifdef LOGMESSAGE
	if(loghandle!=-1) {
		const int maxtmp=1024;
		char tmp[maxtmp];
		int len=snprintf(tmp,maxtmp, "%s:" OFFPRINT " %s\n",mem.filename,filepos,info);
		write(loghandle,tmp,len);
		}
#endif
	INCDEST();
	editmove(MESSAGELINE,0);
	COMMANDON;
	overline() ;
	editmove(MESSAGELINE,0);
	waddnstr(editscreen,(char *)info,Screenwidth);
	COMMANDOFF;
	ismessage=1;
	wrefresh(editscreen);
	DECDEST();
	return 0;
	}


int globaloutput(char *str) { return edits[editfocus]->output(str); }
int globalmessage(const char *format, ...) {
	va_list ap;
	const int len=255;
	char str[len];
	va_start(ap,format);
	vsnprintf( str,len, format,  ap);
	va_end(ap);
	return globaloutput(str);
	}
extern char showfileinfo;
 int Editor::showmessage(void) {
	if(edits[editfocus]==this) {
		if(messagebuf[0]!=0&&!showfileinfo) {
			realoutput(messagebuf);	
			}
		else {
			fileinfo();
			showfileinfo=0;
			}
		messagebuf[0]=0;
		}
	else 
		fileinfo();
	return 0;
	}
#include "screenpart.h"
 int Editor::showposition(void) {
	if(nomessage)
		return 0;
	INCDEST();
	COMMANDON;
	int start=Screenwidth-maxnumstr-1; 
	int bytes=parts[mode]->bytes;
	wmove(editscreen,MESSAGELINE,start); 
	waddch(editscreen, ' ');
	wprintw(editscreen,numstr ,((editpos/bytes)*bytes)+filepos);
	COMMANDOFF;
	DECDEST();
	return 0;
	}
	
 int Editor::message(const char *format ...) {
	va_list ap;
	char str[255];
	va_start(ap,format);
	vsnprintf( str,255, format,  ap);
	va_end(ap);
	return output(str);
	}


int Editor::getinput(const char question[],char *answer) {
	answer[0]='\0';
	if(getanswer(question,answer,80)<0) {
		beep();return -1;
		}
	return 0;
	}
int Editor::inputget(char *answer,const char *format,...) {
	char question[255];
	va_list ap;
	va_start(ap,format);
	vsnprintf( question, 255,format,  ap);
	va_end(ap);
	answer[0]='\0';
	if(getanswer(question,answer,255)<0) {
		beep();return -1;
		}
	return 0;
	}

 int Editor::getkeyinput(const char *format ...) {
	int key;
	va_list ap;
	char str[255];
	va_start(ap,format);
	vsnprintf( str,255, format,  ap);
	va_end(ap);
	while(1) {
		realoutput(str);
		key= getch();
		if(key!=RESIZE) {
			return key;
			}
		resized();
		rewriteall();
		}
	}

#define kgetch() ggetch(stdscr)
extern int ggetch(WINDOW *w) ;


extern bool mykbhit(void) ;
int getkeys(unsigned char *keys,int maxkeys) {
	int i;
	keys[0]=kgetch();
	for(i=1;i<maxkeys;i++) {
		keys[i]=(unsigned char)ERR;
		for(int j=0;j<2;j++) {
			if(mykbhit()) {
				 keys[i]=kgetch();
				break;
				}
			usleep(10000);
			}
		if(keys[i]==(unsigned char)ERR) {
			flushinp();
			return i;
			}
		}
	flushinp();
	return i;
	}
