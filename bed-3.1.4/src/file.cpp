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
#ifndef HAVE_SETENV
#include "setenv.h"
#endif
#include "desturbe.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#ifndef HAVE_STPCPY
#include <algorithm> 
#endif
#ifdef HAVE_TERMIOS_H
       #include <termios.h>
#endif
       #include <unistd.h>
#include "editor.h"
#include "main.h"
#include "system.h"
#include "screen.h"
#include "inout.h"
#include "screenpart.h"
#include "config.h"
#include "cursesinit.h"
#include "win.h"
#ifdef USE_WINFILE
#include <windows.h>
#include <winioctl.h>
#if defined(_WIN32) && !defined(DEFAULTBEDRCBIN)
#include <shfolder.h>
//#include <Shlobj.h>
#endif
#include "win.h"
#endif
extern int wantsignal;

#ifndef NOSIGNALS
extern void omnihandler(int signum) ;
#else
#define DONTSUSPEND 
#endif
extern void closefilehandle(FILEHANDLE file);
extern const char NEWFILENAME[];
extern const int newfilenamelen;

extern char messagebuf[];


int Editor::askmenufile(void) {
		char buf[BEDSMAXPATH]="*";
		const char *file;
			while(1) {
				if((file=getfile("Open file",buf))) {
					errno=0;
					if(!newfile(file)){
						if(errno)
							message("Can't open %s: %s",file,strerror(errno));
						else 
						     showmessage();
/*
							message("Can't open %s: zero size file",file);
*/
						wrefresh(editscreen);
						strcpyn(buf,file,BEDSMAXPATH);
						continue;
						}
					else {

//					messagebuf[0]=0;
						return 0;
						}
					}
				else {
					editup();
					return -1;
					}
				}
			}

 int Editor::opendirect(void) {
	return askfile();
	}
int Editor::askfile(void) {
		static char ant[MAXANT];
        //        getinput("Name of file to visit: ",ant);
	if(getanswer("Filename: ",ant,MAXANT,60)<0) {
		message("No filename read");return -1;
		}
	if(!editfile(ant)) {
			const char *file;
			if((file=getfile("Open file",ant))) {
				if(!editfile(file)){
						rewritescreen=1;
						message("Error opening %s",file);
						return -1;
						}
					}
			else {
					editup();
				return -1;
				}
			return 0;
			}
		return ASKFILE;
		};
char *Editor::getfilename(char *buf) {
	if(buf) {
		memcpy(buf,mem.filename,mem.filenamelen+1);
		return buf;
		}
	else
		return mem.filename;
	}

extern OFFTYPE setfilepos(FILEHANDLE file,OFFTYPE offset) ;
extern  FILEHANDLE opencreate(const char *filename);
#ifdef Windows
/*
BOOL WINAPI GetFileInformationByHandle(
  __in   HANDLE hFile,
  __out  LPBY_HANDLE_FILE_INFORMATION lpFileInformation
)
*/
/*
typedef struct _BY_HANDLE_FILE_INFORMATION {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    dwVolumeSerialNumber;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    nNumberOfLinks;
  DWORD    nFileIndexHigh;
  DWORD    nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION;
*/

FILEHANDLE openreading(const char *filename) ;
FILEHANDLE Editor::tmpopenfile(const char *ant) {
	FILEHANDLE handle;
	if((handle=opencreatenew(ant))==MY_INVALID_HANDLE_VALUE) {
		DWORD crerror=GetLastError();
		if(crerror==ERROR_FILE_EXISTS) {
			message("%s exists, overwrite? (y/n)",ant);
			wrefresh(editscreen);
			switch(getch()) {
				case 'y':;
				case 'Y': 
					  break;
				case 'n':;
				case 'N':;
				default:message("Not saved");wrefresh(editscreen); return MY_INVALID_HANDLE_VALUE;

				};
		if((handle=openreading(ant))==MY_INVALID_HANDLE_VALUE) {
			message("Can't open %s for reading",ant);
			wrefresh(editscreen);
			return MY_INVALID_HANDLE_VALUE;
			}
		BY_HANDLE_FILE_INFORMATION FileInfo;

		if(!GetFileInformationByHandle(handle,&FileInfo )) {
			message("Can't get fileinfo for %s",ant);
			wrefresh(editscreen);
			closefilehandle(handle);
			return MY_INVALID_HANDLE_VALUE;
			}




		closefilehandle(handle);
		for(int i=0;i<nredit;i++) {
			if(samelink(&FileInfo,&(edits[i]->mem.FileInfo))) {
				beep();
				message("User error: %s is open.",edits[i]->mem.filename);
				wrefresh(editscreen);
				return MY_INVALID_HANDLE_VALUE;
				}
			}
		if((handle=opencreate(ant))==MY_INVALID_HANDLE_VALUE) {
			message("Can't open %s",ant);
			wrefresh(editscreen);
			return MY_INVALID_HANDLE_VALUE;
			}
			}
		else {
			message("Can't open %s",ant);
			wrefresh(editscreen);
			return MY_INVALID_HANDLE_VALUE;
			}
		}
	  char ch=0;
	  if(writetofile(handle,&ch,1)!=1) {
		  message("write to %s failed",ant);
			closefilehandle(handle);
			return MY_INVALID_HANDLE_VALUE;
			  };
	setfilepos(handle,(OFFTYPE)0L);
	return handle;
		}
#else
FILEHANDLE Editor::tmpopenfile(const char *ant) {
	STATTYPE st;
	FILEHANDLE handle;
	int fileexists=0;
	if(!access(ant,0)) {
		if(STAT(ant,&st))
			memset(&st,'\0',sizeof(st));
		fileexists=1;
		}

	if(fileexists) {
		for(int i=0;i<nredit;i++)
			if(st.st_ino==edits[i]->mem.statdata.st_ino) {
				message("User error: %s is open.",edits[i]->mem.filename);
				wrefresh(editscreen);
		return MY_INVALID_HANDLE_VALUE;
				}
		message("%s exists, overwrite? (y/n)",ant);
		wrefresh(editscreen);
		switch(getch()) {
			case 'y':;
			case 'Y': 
				  break;
			case 'n':;
			case 'N':;
			default:message("Not saved");wrefresh(editscreen); return MY_INVALID_HANDLE_VALUE;

			};
		}

	if((handle=opencreate(ant))==MY_INVALID_HANDLE_VALUE) {
		message("Can't open %s",ant);
		wrefresh(editscreen);
		return MY_INVALID_HANDLE_VALUE;
		}
	  char ch=0;
	  if(writetofile(handle,&ch,1)!=1) {
		  message("write to %s failed",ant);
			closefilehandle(handle);
			return MY_INVALID_HANDLE_VALUE;
			  };
	setfilepos(handle,(OFFTYPE)0L);
	return handle;
		}
#endif
#ifdef x_WIN32
#define TO_REAL
#endif
#if defined(_WIN32)
#if 1
FILEHANDLE Editor::openfile(const char *ant) {
	return tmpopenfile(ant) ;
}
#else
FILEHANDLE Editor::openfile(const char *ant) {
FILEHANDLE handle;
	if((handle=opencreate(ant))==MY_INVALID_HANDLE_VALUE) {
		message("Can't open %s",ant);
		wrefresh(editscreen);
		return MY_INVALID_HANDLE_VALUE;
		}
#if __GNUC__
#warning "Editor::openfile test op openfile"
#endif
	return  handle;
}
#endif
#else
int Editor::openfile(const char *ant) {
	STATTYPE st;
	int handle;
#ifdef  TO_REAL
char namebuf[BEDSMAXPATH];
//		arealpath(ant,namebuf);
	expandfilename(namebuf,ant);
#else
#define namebuf ant
#endif
	if((handle=LOPEN(namebuf,O_WRONLY|O_CREAT BINARY_FLAG,0666))<0) {
		message("Can't open %s: %s",namebuf,strerror(errno));
		wrefresh(editscreen);
		return -1;
		}
	if(FSTAT(handle,&st)==-1) {
		output("fstat failed, file not saved");
		wrefresh(editscreen);
		return -1;
		}
	if(st.st_size!=0) {
		for(int i=0;i<nredit;i++)
			if(st.st_ino==edits[i]->mem.statdata.st_ino) {
				message("User error: %s is open.",edits[i]->mem.filename);
				wrefresh(editscreen);
				LCLOSE(handle);
				return -1;
				}
		message("Nonzero %s exists, overwrite? (y/n)",namebuf);
		wrefresh(editscreen);
		switch(getch()) {
			case 'y':;
			case 'Y': if(FTRUNCATE(handle,0)<0) {
					message("Can't truncate %s: %s",namebuf,strerror(errno));
					wrefresh(editscreen);
					return -1;
					}
				break;
			case 'n':;
			case 'N':;
			default:message("Not saved");wrefresh(editscreen);LCLOSE(handle);return -1;
			};
		}
	return handle;
#ifndef TO_REAL
#undef namebuf
#endif
		}

#endif
int Editor::endedit(void) {
	if(nredit<2) {
		if(!argfile())
			return exitall();
		}

	if(closefile()<0)
		return( -1);
	return ::endedit(this);
	}	
extern Editor **deathcells;
extern int maxdeath,gossip;
extern Editor *selectioneditor;
int Editor::closethis(void) {
	if(endedit()<0)
		return -1;
	else {
		INCDEST();
		if(gossip>=maxdeath) {
			Editor **tmp=deathcells;
			maxdeath*=2;
			deathcells=myallocarpr(Editor **,Editor *,maxdeath);
			memmove(deathcells,tmp,gossip*sizeof(Editor*));
			}
		if(selectioneditor==this)
			selectioneditor=NULL;
		deathcells[gossip++]=this;
		DECDEST();
		}
	return 0;
	}
int Editor::quit(void) {
	return exitall();
	}
int Editor::closefile(void) {
	int i;
	memorize();
	if(!modified) {
		return 0;
		}
	startnonactive=minnum(startnonactive,nredit);
	for(i=startactive;i<startnonactive;i++) 
		if(this==edits[i])
			break;
	if(i==startnonactive) {
		startactive=editfocus;
		startnonactive=startactive+1;
		place(0);
		}
	wrefresh(editscreen);
	while(1) {
		message("Save changes %s before leaving? (y/n/c)",mem.filename);
		switch(getch()) {
			case 'y':;
			case 'Y': if(save())
					return -1;
					else
						return 0;
			case 'n':;
			case 'N': return 0;
			case 'c':;
			case 'C':
				clearmessage();
					return -1;
#define firstescape 27
#define secondescape 254
			case firstescape: 
				if(getch()==secondescape) {
					clearmessage();
					wrefresh(editscreen);
					return -1;
					};
				flushinp();
				wrefresh(editscreen);
			default: beep();
			};
		}; 
	return 0;
	}

int Editor::saveaser(const char *ant) {
	if(!ant) {
		return 1;
		}
	else {

	if(mem.isnewfile()) {
		FILEHANDLE oldhandle,handle;
		memorize();
		if((	handle=tmpopenfile(ant))== MY_INVALID_HANDLE_VALUE)		{
			beep();
			return -1;
			}
		char filebuf[BEDSMAXPATH];
		if(!arealpath(ant,filebuf))  {
			beep();
			closefilehandle(handle);
			return -1;
			}
		INCDEST();
		oldhandle=mem.filehandle;
		char *oldfilename=mem.filename;
		mem.filehandle=handle;
		int len=strlen(filebuf)+1;
		mem.filename=myallocar( char,len);
		memcpy(mem.filename,filebuf,len);
		if(int err=mem.saveall()) {
			myfree(mem.filename);
			mem.filename=oldfilename;
			mem.filehandle=oldhandle;	
			closefilehandle(handle);

if(err==1) message("readonly because of previous error");
else
#ifdef _WIN32
		if(err==WINERROR&&mem.errors) {
			const int meslen=200;
			char mes[meslen];
			if(winerror(mes,meslen,mem.errors))
				message("%s: %s",filebuf,mes);
			else
				message("Error writing to %s",filebuf);
			}
#else
		if(err>0)
			message("Error writing to %s %s",filebuf,strerror(errno));
#endif
		else
			message("Error writing to %s",filebuf);
	DECDEST();
			return -1;
			}
		message("Saved all to %s",mem.filename);
mem.filenamelen=len-1;
		modified=0;
		closefilehandle(oldhandle);
	DECDEST();
		return 0;
		}
	else {
		 FILEHANDLE handle;
		if((	handle=openfile(ant))==MY_INVALID_HANDLE_VALUE)		{
			beep();
			return -1;
			}
	INCDEST();
	errno=0;
			if(mem.saveto(handle)!=0) {
					closefilehandle(handle);
#ifdef _WIN32
			
			showerror("%s: %s",ant);
					
#else
					message("Problems writing to %s: %s",ant,strerror(errno));
#endif
					beep();
					wrefresh(editscreen);
	DECDEST();
					return -1;

					}
			closefilehandle(handle);
	DECDEST();
			message("Saved all to %s",ant);
			return 0;
			}
		return -1;
		}
	}
int Editor::saveas(void) {
	char buf[BEDSMAXPATH];
	const char *ant="*";
	memorize();
	do {
		strcpyn(buf,ant,BEDSMAXPATH);
		ant=getfile("Save as",buf) ;
		editup();
		if(!ant)
			return -1;
		} while( saveaser(ant)<0);
	return 0;
	}

int Editor::dosave(void) {
	if(mem.isnewfile())
		return saveas();
	memorize();
	if(!modified) {
		output("Nothing to save");
		return 1;
		}

	while(1) {
		int tou;
		if((tou=mem.touched())>0) 
			message("Error:: %s changed by other process. Add modified screens (%d boundaries)????? (n/y)",mem.filename,mem.BLOCKSIZE);
		else {
			if(tou<0)
				output("Original could be changed by other process. Save changes? (y/n)");
			else
				output("Do you really want to save changes? (y/n)");
			}
		switch(getch()) {
			case 'y':;
			case 'Y': {
					return save();
					};break;
			case 'n':;
			case 'N': 
				flushinp();
				message("Not saved");
				return 0;
			default: flushinp();beep();
			};
		}; 
	return 1;
	}
int Editor::save(void) {
	int ret;
	memorize();
	if(mem.isnewfile()) {
		return saveas();
		}
	INCDEST();
	if((ret=mem.saveall())) {
//		message("Couldn't save file %s: %s",mem.filename, (ret< sys_nerr)?strerror(ret):"?");
#ifdef Windows
switch(ret) {
case 1: message("readonly because of previous error");break;
case WINERROR: 
		if(mem.errors) {
			const int meslen=200;
			char mes[meslen];
			if(winerror(mes,meslen,mem.errors)) {
				output(mes);
				break;
				}
			};
default: 
		message("Can't save %s",mem.filename) ;
	};
#else
if(ret>0)
		message("Couldn't save file %s: %s",mem.filename, strerror(ret));
else
		message("Couldn't save file %s",mem.filename);
#endif
		ret=1;
		}
	else {
	  message("Saved %s",mem.filename);
		modified=0;
		ret=0;
		}
	DECDEST();
	return ret;
	}

int Editor::checkchanged(void) {
	int ret;
	memorize();
	INCDEST();
if(mem.isnewfile())  {
   if(mem.zerochange()==0 ) {
	 modified=0;
	 message("Unmodified");
	 ret=0;
	}
   else {
	message("Modified: Alt-f,s to save, Alt-e,u to undo changes");
	ret=1;
	}
}
else {
	if((ret=mem.touched())>0) {
		if(modified)
			message("File changed by other process. Save(as) will mess up old and new"  );
		else {
			rewrite();
			toscreen();
			message("File changed by other process.");
			}
		ret= 2;
		}
	else {
		int changed=mem.changed();
		if(changed ) { 
			if(changed==-1)
				message("Modified: Alt-f,s to save, Alt-e,u to undo changes");
		else
				message("No modification found. Don't trust it: an error occurred earlier");
			ret= 1;
			}
		else {
			if(ret<0)  {
				message("Unmodified by this process"); 
				}

			else
				message("Unmodified");
			modified=0;
			ret= 0;
			}
		}
}
	DECDEST();
	return ret;
	}

int Editor::saveinode(void) {
	place(0);
	message("Save dangling file, previously called %s? (Y/n)",mem.filename);
	switch(getch()) {
		case 'n':;
		case 'N':;break;
		case 'y':;
		case 'Y': ;
		default:
			while(saveas()<0) {
				beep();
				};
			break;
		}
	return 0;
	}

extern int therewrite(void) ;
extern int thesuspend(bool);


int Editor::shell(void) {
//	kill(pid,SIGSTOP);
//stophandler(SIGSTOP) ;
	
int res= thesuspend(false);
if(res)
	message("Set the SHELL environmental variable to the program you want so start"); 
return res;
	return 0;
}
	
#undef ADDNEWLINE

#define ADDNEWLINE  flen+=addnltoar(formbuf+flen);
/*
#undef ADDNEWLINE

#define ADDNEWLINE formbuf[flen++]=0x0D; formbuf[flen++]=0x0A;
*/

#if defined(__MINGW32__)&&!defined(SIGTSTP)
void kill(pid_t,int) {}
#endif

ssize_t Editor::saveshownname(const char *ant) {
	if(selectpos== OFFTYPEINVALID) {
		message("No selection");
		return -1;
		}
	FILEHANDLE handle;
	memorize();
//#ifdef Windows
#ifdef __CYGWIN__
char namebuf[BEDSMAXPATH];
		winpath(ant,namebuf);
#else
const char *namebuf=ant;
#endif
	if((handle=openfile(namebuf))==MY_INVALID_HANDLE_VALUE) {
		return -2;
		}
INCDEST();
	ScreenPart *datashown=parts[mode];
	int showelsize=datashown->maxres+datashown->apart;
	int bytes=datashown->bytes;
	long items=cols()/bytes;
	if(items<1)
		items=1;
	long blocklen=items*bytes;
	OFFTYPE posf=filepos+(editpos/bytes)*bytes,first,last;
	if(selectpos>posf) {
		first=posf;
		last=selectpos+bytes;
		if(last>filesize())
			last-=bytes;
		lensel=last-first;
		if(lensel%bytes) {
			first-=(bytes	-lensel%bytes);
			if(first<0)
				first+=bytes;
			}
		}
	else {
		first=selectpos;
		last=posf;
		lensel=last-first;
		last+=(bytes-lensel%bytes);
		if(last>filesize())
			last-=bytes;
		}
	lensel=last-first;
	if(lensel<=0) {
		message("No full item in selection");
		DECDEST();
		return -3;
		}
	message("Writing to file %s",namebuf);
	wrefresh(editscreen);
	OFFTYPE blocknr=	lensel/blocklen;
	int rest=lensel%blocklen;
	OFFTYPE endpos=blocknr*blocklen+first;
	Viewbuf binbuf(mem,blocksize()*2);
//	unsigned char binbuf[blocklen];
	unsigned char formbuf[items*showelsize+1];
	OFFTYPE off=first;

	for(off=first;off<endpos;off+=blocklen) {
		if(wantsignal) {
			int olddont=GETDEST();
			NODEST();
			int wassig=wantsignal;
			wantsignal=0;
			kill(pid,wassig);	
			SETDEST(olddont);
			}
//		getmem(off,(char *)binbuf,blocklen);
		int flen=datashown->fromascii(formbuf,(unsigned char *)binbuf.getdata(off,blocklen),blocklen);
ADDNEWLINE
		writetofile(handle,(char *)formbuf,flen);
		}
	if(rest>0) {	
//		getmem(off,(char *)binbuf,rest);
		int flen=datashown->fromascii(formbuf,(unsigned char *)binbuf.getdata(off,blocklen),rest);
ADDNEWLINE
	writetofile(handle,(char *)formbuf,flen);
		}
	ssize_t total= sgetfilesize(handle);

	if(total<0)
		message("getfilesize failed");
	if(!closehandle(handle)){
#ifndef _WIN32
		message("closehandle(handle) failed: %s",strerror(errno));
#else
		showerror("closehandle %s failed: %s",namebuf);
#endif
		DECDEST();
		return -4;
		}
	message("Selection saved in %s",namebuf);
	DECDEST();
	return total;
	}

#undef ADDNEWLINE
/*
 * Als regels niet uitkomen op een eenheid na verwijdering newline en het wel uitkomt wanneer newline in een spatie word veranderd wordt dit gedaan.
 *
*/
#define endmmap {if(nommap) myfree(gegs);else  munmap(gegs, endf+1); close(handle);}
#include <assert.h>
#include "misc.h"
inline BLOCKTYPE endblock(OFFTYPE posf,int bs,int blen) {
const OFFTYPE endpos=posf+blen;
return (endpos-1)/bs+1;
}





int Editor::insertshownpos(const OFFTYPE posf,const char *const gegs, const off_t endf) {
	if(searchdontedit())
		return -1;
memorize();
ScreenPart *datashown=parts[mode];
INCDEST();
const int bytes=datashown->bytes;
const int apart=datashown->apart;
const int maxres=datashown->maxres;
const int maxunit=maxres+apart;
const int blen=((endf+1)/minnum(2,maxunit))*bytes;/*Smallest digit is 1 + 1 space makes 2 */
const int bs=blocksize();
const BLOCKTYPE sbl=posf/bs ,bks=endblock(posf,bs,blen)-sbl;



char *const binbuf=(char *)alignedalloc(bs,bks*bs*sizeof(char));
const int inblock=posf%bs;
mem.getblocks(sbl,binbuf,bks);
unsigned char *const und=(unsigned char *)expandchunk(NULL,blen,0);
memcpy(und,binbuf+inblock,blen);
const int binput= datashown->search2bin(gegs, (unsigned char*)binbuf+inblock, endf); 
assert(binput<=blen);

mem.putblocks(sbl,binbuf,endblock(posf,bs,binput)-sbl);
inundo(posf,und,binput);
alignedfree(binbuf);
modified++;
updated=0;
dofromfile=1;
DECDEST();
return binput;
}

int Editor::insertshown(char *gegs, off_t endf) {
if(searchdontedit())
	return -1;

const int bytes=parts[mode]->bytes;
const OFFTYPE pos=filepos+(editpos/bytes)*bytes;
int ret=insertshownpos(pos,gegs,endf);
topos(pos+endf-1);
return ret;
}
#define TO_REAL
#ifdef Windows

BEDS_SIZE_T readfromfile(FILEHANDLE input,char *buf,BEDS_SIZE_T nr) ;
int Editor::readshownname(const char *filename) {
	if(searchdontedit())
		return -1;

	FILEHANDLE handle;
	int ret=0;
	memorize();

//#if defined(Windows)

#ifdef __CYGWIN__
char ant[BEDSMAXPATH];
		winpath(filename,ant);
#else
const char * const ant=filename;
#endif
	if((handle=openreading(ant))==MY_INVALID_HANDLE_VALUE) {
		message("Can't open %s for reading",ant);
		return -1;
		}

	ScreenPart *datashown=parts[mode];
	int showelsize=datashown->maxres+datashown->apart;
	off_t endf=sgetfilesize(handle);
	char *gegs=myallocar(char,endf+showelsize);
	readfromfile(handle,gegs,endf);
	ret=insertshown(gegs, endf); 
	myfree(gegs); 
	closehandle(handle);
	signalwant(noop,-2);
	return ret;

	}
#else
int Editor::readshownname(const char *ant) {

	if(searchdontedit())
		return -1;
	int handle,ret=0;
	memorize();


#if defined(Windows) && CYGWIN_VERSION_DLL_MAJOR>=1007
char namebuf[BEDSMAXPATH];
	absunixpath(ant,namebuf) 
#else
#define namebuf ant
#endif
	if((handle=open(namebuf,O_RDONLY BINARY_FLAG))<0) {
		message("Can't open %s",namebuf);
		return -1;
		}

	ScreenPart *datashown=parts[mode];
	int showelsize=datashown->maxres+datashown->apart;
       off_t endf;
	endf=lseek(handle, 0L, SEEK_END);
       char *gegs= (char *)mmap(NULL, endf+showelsize,  PROT_READ ,  MAP_SHARED, handle,0);
	int nommap=0;
 	if(gegs==MAP_FAILED) {
		nommap=1;	
		gegs=myallocar(char,endf+showelsize);
		lseek(handle, 0L, SEEK_SET);
		ssize_t res;
		if((res=read(handle,gegs,endf))!=endf) {
			message("Can't read from %s returns %d, expected %d",namebuf,res,endf);
			endmmap;
			return res;
			}
		}
	ret=insertshown(gegs, endf); 
	endmmap;
	signalwant(noop,-2);
	return ret;

#ifndef TO_REAL
#undef namebuf
#endif
	}
#endif

int Editor::readshown(void) {

	if(searchdontedit())
		return -1;

	char buf[BEDSMAXPATH];
	const char *ant="*";
	strcpyn(buf,ant,BEDSMAXPATH);
	memorize();
	do {
		ant=getfile("Insert file in shown datatype",buf) ;
		editup();
		if(!ant) 
			return -1;
		} while( readshownname(ant)<0);
	return 0;
	}


int Editor::saveshown(void) {
	if(selectpos!=OFFTYPEINVALID) {
		const char *ant;
		BEDS_SIZE_T ret;
		char buf[BEDSMAXPATH]="*";
	do {
		again:
		ant=getfile("Save selection in datatype as shown",buf) ;
		editup();
		if(!ant)
			return -1;
		if((ret=saveshownname(ant))==-2) {
			strcpyn(buf,ant,BEDSMAXPATH);
			buf[BEDSMAXPATH-1]='\0';
			goto again;
			}
		} while(ret==-2);
	return ret;
	}
	message("No selection");
	beep();
	return -1;
	}




extern FILEHANDLE opencreate(const char *filename) ;
extern FILEHANDLE openreading(const char *filename) ;
extern void closefilehandle(FILEHANDLE file);
extern BEDS_SIZE_T readfromfile(FILEHANDLE input,char *buf,BEDS_SIZE_T nr);

extern char bedrcbin[];
char bedrcbin[BEDSMAXPATH] {""};
#if !defined(DEFAULTBEDRCBIN)
#ifdef __linux__
const auto DEFAULTBEDRCBIN =R"(~/.bedrc-bin)";
#endif
#endif


int bedrcbinlen=0;
void writesearchgegs(void) {
	const int len =bedrcbinlen;
	char old[len+5];
	memcpy(old,bedrcbin,len);
	memcpy(old+len,".old",5);
	rename(bedrcbin, old);
	FILEHANDLE soutf=opencreate(bedrcbin);
	if(soutf== MY_INVALID_HANDLE_VALUE) {
		fprintf(stderr,"Can't open %s for writing\n",bedrcbin);
		return;
		}
	if(writetofile(soutf,reinterpret_cast<char *>(&Editor::defaultsearch),sizeof(Editor::defaultsearch))<0)
		fprintf(stderr,"Write to %s failed\n",bedrcbin);
	closefilehandle(soutf) ;
	}; 
int readsearchgegs(void) {
if(bedrcbin[0]!=FILENAMESEP[0]) {
#if defined(_WIN32) && !defined(DEFAULTBEDRCBIN)
#ifndef BEDRCBINFILE
const char BEDRCBINFILE[]=R"(bedrc-bin)";
#endif
//	SHGetFolderPathA( NULL, CSIDL_APPDATA ,NULL,SHGFP_TYPE_CURRENT , bedrcbin);
	SHGetFolderPathA( NULL, CSIDL_APPDATA ,NULL,0 , bedrcbin);
	bedrcbinlen=strlen(bedrcbin);
	bedrcbin[bedrcbinlen++]='\\';
#ifdef HAVE_STPCPY
	const char * const endstr=stpcpy(bedrcbin+ bedrcbinlen, BEDRCBINFILE);
#else
	const char * const endstr=std::copy(std::begin(BEDRCBINFILE),std::end(BEDRCBINFILE),bedrcbin+ bedrcbinlen);
#endif
	bedrcbinlen=endstr-bedrcbin;
#else
	expandfilename(bedrcbin,DEFAULTBEDRCBIN);
	bedrcbinlen=strlen(bedrcbin);
#endif
	}
FILEHANDLE sinf=openreading(bedrcbin);
if(sinf== MY_INVALID_HANDLE_VALUE) {
#ifndef NDEBUG
	fprintf(stderr,"Can't open %s for reading\n",bedrcbin);
#endif
	return -1;
}
int len=sizeof(Editor::defaultsearch);
//s/Editor::SearchType/SearchOpts/g
SearchOpts tmp;
int res=readfromfile(sinf,reinterpret_cast<char *>(&tmp),len);
closefilehandle(sinf) ;
if(res==len) {
	Editor::defaultsearch=tmp;
const int maxsearch=1
#ifdef USE_RE2
+1
#endif
;
//typedef SearchOpts::RegexT  RegType;
typedef RegexT  RegType;
#if defined(__clang__) && defined(__ANDROID__)
#warning Editor::defaultsearch assigned via cast, so -Wtautological-constant-compare doesnt apply
#endif
	if(Editor::defaultsearch.RegexReplace>maxsearch)
		Editor::defaultsearch.RegexReplace=RegType::RegGNU;
	if(Editor::defaultsearch.RegexSearch>(maxsearch
	#ifdef USE_HYPERSCAN
	+1
	#endif
	))
	Editor::defaultsearch.RegexSearch=RegType::RegGNU;
	return len;
	}
else {
	fprintf(stderr,"Read from %s failed\n",bedrcbin);
	return -2;
	}
};

//SearchType Editor::defaultsearch = { true,None,true,true,false,false};
//SearchType Editor::defaultsearch = {.procel={true,None,true,true,false,false}};
SearchType Editor::defaultsearch; 

//typedef  struct {uint8_t  Forward=true; RegexT Regex=None;uint8_t Raw=true,Case=true, Align=false, Unit=false; } ProcelT ;
/*
typedef struct SearchTyper{
union {
struct {uint8_t  Forward; RegexT Regex;uint8_t Raw,Case, Align, Unit; };
ProcelT procel;
};
uint8_t List=0, Edge=false, Keep=false,keepAlign=false,keepUnit=false;
RegexT RegexSearch=RegGNU,RegexReplace=RegGNU; 

SearchTyper() {};
} SearchType;

*/
