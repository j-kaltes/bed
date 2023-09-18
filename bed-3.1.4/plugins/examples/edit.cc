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
#include "global.h" 
#include <stdlib.h>
#include <alloca.h>
#include "config.h"
#include CURSES_HEADER
#ifdef PDCURSES
#undef MOUSE_MOVED
#endif

#include <stdio.h>
#include "defines.h"
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#include <string.h>
#include "screenpart.h"
#include "screen.h"
extern void otherscreen(void);
extern void bedscreen(void) ;

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#include <process.h>
#include "../../src/shell.h"
#endif
BEDPLUGIN
class Editor {
public:
#include "faked.h"
iscalled(editdata)
};

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

#ifdef _WIN32
#ifdef PDCURSES
#undef MOUSE_MOVED
#endif

#include <windows.h>
#include "win.h"
#endif
const char *geteditorname(void) {
	static  char *editorname=getenv("EDITOR");
	if(!editorname) {
#ifndef _WIN32
	static char vi[]= "vi";
		editorname=vi; 
#else
		char *windir=(char *)alloca(MAX_PATH);
		GetWindowsDirectory(windir,MAX_PATH);
		int len=strlen(windir);
		windir[len++]='\\';
		strcpy(windir+len,"notepad.exe");
		len+=11;
#ifdef __CYGWIN__
	editorname=(char *)malloc(MAX_PATH);
	absunixpath(windir,editorname);
#else
	editorname=(char *)malloc(len+1);
	strcpy(editorname,windir);
#endif
#endif
		}
	return editorname;
	}
extern int mytmpname(char *buf,int len,const char *name) ;
#define returngui(ret) {SetCurrentDirectory(	curdir); return ret;}
#if 0
int guiprogram(const char *progname) {
char prog[MAX_PATH];
HMODULE mod=GetModuleHandle(NULL);	
if(!(han&&( len=GetModuleFileName(mod, prog,MAX_PATH)))) {
	fprintf(stderr,"Can't get program filename\n");
		return 0;
	}
fprintf(stderr,"%s\n",prog);
for(int i=len;i>=0;i++) {
	if(prog[i]=='\\') {
			prog[i]='\0';
			len=i;
			}
	}
char curdir[MAX_PATH];
int curlen;
curlen=GetCurrentDirectory( MAX_PATH,curdir);
if(guiprogram(prog,progname)) {
		returngui( 1);
		}
if(guiprogram(curdir,progname)) {
		returngui( 1);
		}
GetSystemDirectory(prog,MAX_PATH);
if(guiprogram(prog,progname)) {
		returngui( 1);
		}
GetWindowsDirectory(
    prog,	// address of buffer for Windows directory 
    MAX_PATH 	// size of directory buffer 
   );	
if(guiprogram(prog,progname)) {
		returngui( 1);
		}
return 0;
}
#endif
#ifdef _WIN32
int guiprogram(const char *progname) {
HANDLE han; 
char prog[MAX_PATH],*fpart;
const char *progptr
#ifdef __CYGWIN__
=prog;
 winpath(progname ,prog);
#else
=progname;
#endif
if((han=CreateFile(
		progptr	,
    GENERIC_READ	,	// access (read-write) mode 
    FILE_SHARE_READ	,	// share mode 
    NULL,	// address of security descriptor 
    OPEN_EXISTING,	// how to create 
    FILE_ATTRIBUTE_READONLY,	// file attributes 
    NULL 	// handle of file with attributes to copy  
   )	)==INVALID_HANDLE_VALUE) {
	 if(!SearchPath( NULL,
			progname,	// address of filename 
			".exe",	// address of extension 
			MAX_PATH,	// size, in characters, of buffer 
			prog,	// address of buffer for found filename 
			&fpart 	// address of pointer to file component 
		 )) {
				return 0;
				}
		else {
		if((han=CreateFile(
				prog	,
				GENERIC_READ	,	// access (read-write) mode 
				FILE_SHARE_READ	,	// share mode 
				NULL,	// address of security descriptor 
				OPEN_EXISTING,	// how to create 
				FILE_ATTRIBUTE_READONLY,	// file attributes 
				NULL 	// handle of file with attributes to copy  
			 )	)==INVALID_HANDLE_VALUE) {
			 	return 0;
			}

		}
	}	
unsigned int filepos=220;
int ret=0;
 if(SetFilePointer(han,filepos,NULL,FILE_BEGIN)==filepos) {
 		char get;
		DWORD len=1,back;
 		if(ReadFile(han,&get,len,&back,NULL)) {
			if(get==2) {
				ret=1;
				}
			
			}
 		}
		CloseHandle(han);
		return ret;
	}
#endif
/*
#include <termios.h>
extern int startterminal;
extern struct termios  starttermios; 
*/
 

int  Editor::editdata(void) {
	static const char *editorname= geteditorname();
#ifdef __CYGWIN__
	char tmpname[512];
#else
	static char com[512];
	static const int ednamelen=(strcpy(com,editorname),strlen(editorname));
        char *tmpname=&com[ednamelen+1];
	com[ednamelen]=' ';
#endif

#ifdef OLDTMP
#ifdef __CYGWIN__
	char tmpnamel[]="/tmp/bed-tmp-XXXXXX";
	mktemp(tmpnamel);
// cygwin_conv_to_posix_path(tmpnamel,tmpname);

#else
	strcpy(tmpname,"/tmp/bed-tmp-XXXXXX");
#ifdef HAS_MKSTEMP
	mkstemp(tmpname);
#else
	mktemp(tmpname);
#endif
#endif
#else
if(mytmpname(tmpname,256,"bedtmp-XXXXXX")<0) {
	message("Failed to create to create temporary file");
	return -1;
	}
#endif	

	int bytes=activemode()->bytes;
	OFFTYPE first, posf=getfilepos()+(geteditpos()/bytes)*bytes;
	const int lensel=getselregion(&first);
	int ret;
	if((ret=saveshownname(tmpname)) <0) {
		beep();
		return ret;
		}
#ifdef _WIN32
if(!guiprogram(editorname)) 
#endif

otherscreen();

int editfailed=0;
#ifndef __CYGWIN__
	if(system(com)==-1) {
		editfailed=1;
		message("%s %s",strerror(errno),com);
		}
#else
char winname[MAX_PATH];
winpath(tmpname ,winname);

/*
PROCESS_INFORMATION procinfo;
STARTUPINFO startup;
ZeroMemory(&startup,sizeof(startup));
startup.cb=sizeof(startup);

//GetStartupInfo(&startup);
Starts within rxvt everytime a new dos console and I don't know
how to change that
if(! CreateProcess(

      NULL,	// address of module name 
     com,	// address of command line 
    NULL,	// address of process security attributes 
    NULL,	// address of thread security attributes 
    FALSE,	// new process inherits handles 
CREATE_NO_WINDOW,	// creation flags 
    NULL,	// address of new environment block 
    NULL,	// address of current directory name 
    &startup,	// address of STARTUPINFO 
    &procinfo 	// address of PROCESS_INFORMATION  
   )) {
		message("%d,CreateProcess %s failed",GetLastError(),com);
	editfailed=1;	
	}
else {
	CloseHandle(procinfo.hThread);
 WaitForSingleObject(procinfo.hProcess,INFINITE);
	CloseHandle(procinfo.hProcess);
	}
*/
editfailed=process(editorname,winname);

//editfailed=spawnlp(_P_WAIT, editorname, editorname,tmpname, NULL);
/*Gives problems with dos programs make bed */
	
#endif
//reset_prog_mode();
bedscreen();
	topos(first);
	if(!editfailed&&readshownname(tmpname) <0) {
		topos(posf);
#ifdef PDCURSES
rewrite();
#endif
		return -1;
		}
	topos(posf);
	remove(tmpname);
#ifdef PDCURSES
rewrite();
#endif
if(!editfailed)
		message("Changes inserted");
	return 0;
	}

call(editdata)

