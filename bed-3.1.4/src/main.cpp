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
#include "offtypes.h"
#include <alloca.h>

#include <signal.h>
#ifndef NOSIGNALS

#else

#define DONTKILL
#define DONTINTCURSES 1
#endif
#ifdef _WIN32
#include <windows.h>
#include "win.h"

#if defined(__MINGW32__)&&!defined(SIGTSTP)
#include "signalnums.h"
#endif

#endif
#undef ERR
#include <features.h>
#include "editor.h"
#include "main.h"
#include "screen.h"
#include "readconfig.h"
#include "dialog.h"
#include "system.h"
#include "trace.h"
#include "cursesinit.h"
#include "keyproc.h"
#include "myalloc.h"
#include "config.h"
#include <math.h>

#ifdef HAVE_SYS_IOCTL_H 
#include <sys/ioctl.h>
#endif
#include <stdlib.h>
#include <sys/time.h>
     #include <sys/types.h>

            #include <dirent.h>
#include <sys/stat.h>
#ifdef FORCEPOSIXDEFAULTLOCALE
       #include <locale.h>
#endif

#define REFRESH 12
extern char messagebuf[];
extern int resetcolors(void) ;
int thelines,thecols;
pid_t pid;
#ifdef GETOTHERTERM
pid_t *pidden=NULL;
int piddenlen=0;
#endif

#ifdef HAS_SIGSETJMP 
sigjmp_buf screenjmp;
sigjmp_buf resizejmp;
#else
jmp_buf screenjmp;
jmp_buf resizejmp;
#endif
int redistribute=1;
int menuon=1;
int OldMainScreenwidth=thecols;
Editor *getactive(void) {
	return edits[editfocus];
	}

extern int	endrecording(void) ;

#ifdef USETHREADS
sem_t desturbesem;
#endif
#ifdef GETOTHERTERM
void relive(void) ;
#endif
#if  defined(_WIN32)
extern CONSOLE_SCREEN_BUFFER_INFO original_screenbuffer;

int Editor::consoleresize(void) {
#if defined(NCURSES_VERSION) 
#if  defined(_WIN32)

	if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
#endif
#endif
	{
	const char resizemess[]="Now you can resize the console";
	const int resizemesslen=sizeof(resizemess);
	const int width=resizemesslen+4;
	int res=0;

		resizeflag=1;

		SetConsoleScreenBufferSize(consoleoutputhandle,original_screenbuffer.dwSize);
			begindialog(8,width,3);
			mvwaddstr(takescr,2,2,resizemess);
			  oke(6,width/2-2);
			  enddialog(res,res);
			  GetConsoleScreenBufferInfo(consoleoutputhandle, &original_screenbuffer);
#ifdef PDCURSES
			resized();
#else
		rewrite();
#endif
	}
	return 0;
}
#endif

int exitall(void) {
	INCDEST();
	for(int filenr=(nredit-1);filenr>=0;filenr--) {
		if(edits[filenr]->closefile()<0) {
			editfocus=filenr;
			redistribute=1;
			DECDEST();
			edits[editfocus]->message("Cancelled!");
			return -1;
			}
		}
	DECDEST();
	if(endrecording()<0) {
		edits[editfocus]->message("Cancelled!");
		return -1;
		}
		
	for(int filenr=(nredit-1);filenr>=0;filenr--) {
		delete edits[filenr];
		}
	SHOWCURSOR;
#ifdef PDCURSES
	wattrset(stdscr,0);
	move(LINES-1,0);
	addstr("\n");
	refresh();
#else
	if(!isendwin())
		endwin();
#endif
#ifdef GETOTHERTERM
	relive();
#endif
	ENDDEST();
#if  defined(_WIN32)
	#if defined(NCURSES_VERSION) 
	if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
	#endif
	SetConsoleScreenBufferSize(consoleoutputhandle,original_screenbuffer.dwSize);

#endif
	putchar('\n');
	exit(0);
	}
int endedit(Editor *ed) {
	Editor *now,*vorig;
	int i;
	INCDEST();
	nredit--;
	for(i=nredit,vorig=NULL;i>=0&&edits[i]!=ed;i--) {
			now=edits[i];
			edits[i]=vorig;
			vorig=now;
			}
	startnonactive=nredit;
	edits[i]=vorig;

	redistribute=1;
	setactives();
	DECDEST();
	return 0;
	}

extern void putmenus(void) ;
int rewriteall(void) {
		putmenus();
		if((thelines-menuon)>=2) {
			if(editfocus==startactive) {
				while(((thelines-menuon)/(startnonactive-startactive))<2)
					startnonactive--;
				}
			else  
				while(((thelines-menuon)/(startnonactive-startactive))<2)
					startactive++;

			if(editfocus<startactive||editfocus>=startnonactive)  {
					startactive=editfocus;
					startnonactive=editfocus+1;
					}

			for(int i=startactive;i<startnonactive;i++)
					if(edits[i]->place(i-startactive)<0)
						return -1;
			redistribute=0;
			return 0;
			}
		return -1;
		}

void setactives(void) {
	if(editfocus>(nredit-1))
		editfocus=nredit-1;
	if(startactive>editfocus) 
		startactive=editfocus;
	if(startnonactive<(startactive+1))
		startnonactive=startactive+1;
	if(startnonactive>nredit)
		startnonactive=nredit;
	}

#ifdef CURSES
#define keyscr stdscr
#undef getch

#ifndef NEWTERM
#define minimumcols 60
#define minimumlines 15
void scrsize(void) {
	struct winsize win;
   if (ioctl(0, TIOCGWINSZ, &win) == 0) {
        if (win.ws_row != 0) {
            thelines = win.ws_row;
        }
        if (win.ws_col != 0) {
            thecols = win.ws_col;
        }
	if(thecols<0)
		thecols=80;
	else {
		if(thecols<minimumcols)
			thecols=minimumcols;
		else {
			if(thecols>COLS)
				thecols=COLS;
			}
		}
	if(thelines<0)
		thelines=25;
	else {
		if(thelines<minimumlines)
			thelines=minimumlines;
		else {
			if(thelines>LINES)
				thelines=LINES;
			}
		}

	}
}
#endif
extern char **toexecute;
extern int executemax,executeiter;
extern char **toplugin;
extern int pluginmax,pluginiter;
extern char mess[];
extern int dynlink(const char *name) ;
extern int str_call_once(char *name) ;
#if defined(NCURSES_VERSION) 
#if defined (_WIN32)
inline int InConsole(void) { return consoleoutputhandle!=INVALID_HANDLE_VALUE;}
#endif
extern int ncursesdelay;
int ncursesdelay=1;
#endif

extern int readsearchgegs(void) ;
void setconfig(void) {
		Editor::menustructs();
		readconfig();
#if defined(NCURSES_VERSION) 
#if defined (_WIN32)
if(char *ndelaystr=getenv("NCURSESDELAY")) {
	int get=atoi(ndelaystr);
	if(get>0)
		ncursesdelay=	get;
	}
#ifdef CMDNCURWAIT
else if(InConsole())
	ncursesdelay=5;
#endif
#endif
#endif
		if(toplugin) {
			for(int i=0;i<pluginiter;i++) {
				mess[0]='\0';
				if(dynlink(toplugin[i])<0) {
					fprintf(stderr,"%s\n",mess);
					}
				}
			free(toplugin);
			}

		if(toexecute) {
			for(int i=0;i<executeiter;i++) {
				if(str_call_once(toexecute[i]) <0) {
					fprintf(stderr,"Can't execute %s, no such procedure exists\n",toexecute[i]);
					}
				}
			free(toexecute);
			}

readsearchgegs() ;
	}
//void terminate(int err,void *nothing) ;
nocurses_init curses_init_curses;

const char *getfile(const char *prompt,const char *ask) {
	const char *file=ask;
			while(1) {
				if(!(file=takefile(prompt,ask))) {
					return NULL;
					}
				if(file!=ask)  {
					return file;
					}
				resized();
				rewriteall();
				}
			}
#else //CURSES
#ifdef CONIO
void    reset_screen(void) {
        gotoxy(1,MainScreenheight);
        clreol();
        }
struct text_info texi;


#endif //CONIO
#endif //CURSES
//typedef enum {HEX,ASCII,DEC } Type;

	/*
inline void putnumber(long addr,int maxnum,int numbase) {
	for(int i=maxnum-1;i>=0;i--)
		addch(hexel((addr/(1<<(i*4)))%(16)));
	}
	*/

int wantsignal=0;

#include "macros.h"

#if defined(PDCURSES) && defined(_WIN32)
#ifndef SIGUSR1
#define SIGUSR1 SIG_GET
#endif
CALLBACK BOOL ctrlhandler(DWORD sig) {
#ifndef DONTKILL
	if(!ISPDDEST()) 
		kill(pid,SIGUSR1);
	else
#endif
		wantsignal=SIGUSR1;
	return TRUE;
	}
#endif
//#define UNBLOCKSIGNALS
//

#if !defined(NOSIGNALS) && !defined(NOUNBLOCKSIGNALS)
void unblock(int signum) {
 	sigset_t mask;
       sigemptyset(&mask);
        sigaddset(&mask,signum);
         sigprocmask(SIG_UNBLOCK, &mask, NULL);
	}
#else
#define unblock(x)
void omnihandler(int signum) {
	wantsignal=0;
	signal(signum,omnihandler);
	playkeys.flush();
	if(ISDEST()) {
		wantsignal=signum;
		return;
		}
#ifdef USETHREADS
#ifdef HAS_GETVALUE
	int val;
	sem_getvalue(&(edits[editfocus]->sema),&val); 
	if(val<=0) {
		wantsignal=signum;
		return;
		}
#else
	if(sem_trywait (&(edits[editfocus]->sema))!=0) { /*Don't longjump if need for wait*/
		wantsignal=signum;
		return;
		}
#endif
	GSTOPUSELSEEK((&(edits[editfocus]->mem)));
#endif

	unblock(signum);
	siglongjmp(screenjmp,signum);
	};
//sigset_t blockallmask,oldmask;


#endif
#ifndef NOSIGNALS
void realtermhandler(int signum) {
	signal(signum,realtermhandler);
	if(ISDEST()) {
		wantsignal=signum;
		return;
		}
//   	sigprocmask (SIG_BLOCK, &blockallmask, NULL);
	waitinput=0;
	INCDEST();
	GSTOPUSELSEEK(&(edits[editfocus]->mem));
	unblock(signum);
	siglongjmp(screenjmp,signum);
	}
	


#ifndef DONTINTCURSES

#ifdef SIGWINCH
void resizehandler(int signum) {
	signal(SIGWINCH,resizehandler);

	if(waitinput) {
/*		ungetch(REFRESH);
		refresh();
		*/



	GSTOPUSELSEEK(&(edits[editfocus]->mem));
		unblock(signum);
		siglongjmp(resizejmp,signum);
		}
	resizeflag=1;
	};
#endif
#endif
extern void (*origstophandler)(int);
void (*origstophandler)(int) =NULL;
void (*origconthandler)(int) =NULL;
#define ONESHOT

extern int thesuspend(bool);
void stophandler(int signum) {
	unblock(signum);
	thesuspend(true);
	signal(signum,stophandler);
	}

extern int therewrite(void) ;
extern int nasuspend(void) ;
extern void conthandler(int signum) ;
void conthandler(int signum) {
#ifdef DEBUGFLAG
//fprintf(stderr,"conthandler\n");
#endif
#ifdef GETOTHERTERM
	for(int i=0;i<piddenlen;i++)
		if(pidden[i]!=pid) 
			kill(pidden[i],SIGCONT);
#endif			
 nasuspend();
//	resizeflag=1;
	if(waitinput) {
		GSTOPUSELSEEK(&(edits[editfocus]->mem));
		unblock(signum);
		signal(signum,conthandler);
		signal(SIGTSTP,stophandler);
		siglongjmp(resizejmp,signum);
		}
	if(origconthandler) {
		origconthandler(signum);
		}
	signal(signum,conthandler);
	signal(SIGTSTP,stophandler);
	unblock(signum);
	}	
//	kill(pid,SIGWINCH);

void quithandler(int signum) {
	ungetch(28);
	signal(SIGQUIT,quithandler);
	};
#endif
int margc,miter=1;

//char *const *margv;
char **margv;





extern const char NEWFILENAME[];
extern const int newfilenamelen;

extern int newfilesize;
static Editor *newempty(void ) ;
	Editor *emptyfile(void) {
		Editor *ed;
		static int nr=1;
		int oldnredit=nredit;
if((ed=newempty())) {
	char *zeros=myallocar(char,newfilesize);
	memset(zeros,0,newfilesize);
	int ret=ed->mem.putpart(0,newfilesize,zeros);
	myfree(zeros);
	if(ret!=newfilesize) {
		delete ed;
		nredit--;
		return NULL;
		}
	}
	if(oldnredit!=nredit) {
		int len=(int)ceil(log(nr+10000)/log(10))+newfilenamelen+1;
		ed->mem.filename=myallocar( char,len);
		memcpy(ed->mem.filename,NEWFILENAME, newfilenamelen);
		ed->mem.filenamelen=snprintf(ed->mem.filename+newfilenamelen,len-newfilenamelen,"%d",nr++)+newfilenamelen;
		}

	if(addlast()>-1)
		redistribute=1;
	return ed;
	}
Editor *newfile(void) {
	Editor *ed=emptyfile();
	ed->toscreen();
	return ed;
	}
Editor *editin(Editor *ed) {
	if(ed->mem.error()||!ed->mem.size()) {
		return NULL;
		}
	if(nredit>=(maxedit-1))  {
		int oldmaxedit __attribute__ ((unused)) =maxedit ;
		maxedit*=2;
		edits=myrealloc(Editor **,edits,Editor *,maxedit,oldmaxedit);
		}
	edits[nredit]=ed;
	editfocus=nredit;
	redistribute=1;
	nredit++;
	setactives();
	newscherm=1;
	return edits[editfocus];
	}

#ifdef _WIN32
extern char * expanddisk(const char * const name,char *const filename) ;
inline void winexpandall(const char *const in, char * const out) {
	if(in[0]=='\\')
		expanddisk(in,out);
	else
	 	GetFullPathName(in,BEDSMAXPATH,out,NULL);
	}	
#endif



Editor *neweditor(const char *const name) {
if(nredit>=(maxedit-1))  {
	int oldmaxedit __attribute__ ((unused))=maxedit;
	maxedit*=2;
	edits=myrealloc(Editor **,edits,Editor *,maxedit,oldmaxedit);
	}
return (edits[nredit]= new Editor(name));
}


Editor *neweddid( void) {
	editfocus=nredit;
	redistribute=1;
	nredit++;
	setactives();
	newscherm=1;
	return edits[editfocus];
}
Editor *newempty(void ) {
	neweditor(NULL);
	return neweddid();
	}









Editor *newfile(const char *file ) {
	char absolu[BEDSMAXPATH];
#ifdef _WIN32
	winexpandall(file, absolu);
#else
	if(!arealpath(file,absolu))
		return NULL;
#endif
	file=absolu;
#ifndef _WIN32
#ifdef NOSTAT
	bool nostat=false;
#endif
	STATTYPE st;
	if(STAT(absolu,&st)) 
#ifndef NOSTAT
return NULL:
#else
	nostat=true;
#endif
#endif
	for(int i=0;i<nredit;i++)
#ifdef _WIN32
		if(samefilename(edits[i]->mem.filename,absolu)) 
#else
if(
#ifdef NOSTAT
	
		nostat?samefilename(edits[i]->mem.filename,absolu):
#endif


		st.st_ino==edits[i]->mem.statdata.st_ino) 

#endif
			{
			editfocus=i;
			if(editfocus<startactive||editfocus>=startnonactive) {
					redistribute=1;
					}
			newscherm=1;
			edits[i]->shouldreinit=1;
			edits[i]->message("Link to %s is already opened",absolu);
			return edits[i];
			}

Editor *ed=neweditor(file);
if(!ed)
	return NULL;
const char zerofile[]="Why edit zero sized files?";
	if(edits[nredit]->mem.error()|| (!edits[nredit]->mem.size()&&!(memcpy(messagebuf,zerofile,sizeof(zerofile)),errno=0))) {
		delete edits[nredit];
		return NULL;
		}

#ifdef _WIN32
if(edits[nredit]->mem.FileInfo.nNumberOfLinks) {
	for(int i=0;i<nredit;i++)
		if(edits[i]->mem.FileInfo.nNumberOfLinks&&samelink(&(edits[nredit]->mem.FileInfo),&(edits[i]->mem.FileInfo))) {
			delete edits[nredit];
			editfocus=i;
			if(editfocus<startactive||editfocus>=startnonactive) {
					redistribute=1;
					}
			newscherm=1;
			edits[i]->shouldreinit=1;
			edits[i]->message("Link to %s is already opened",absolu);
//			wrefresh(edits[i]->editscreen);
//			snprintf(messagebuf,maxmessagebuf,"Link to %s is already opened",absolu);
			return edits[i];
			}
	}
#endif

	return neweddid();
	}

int addlast(void) {
	if(editfocus>startnonactive) {
		Editor *tmp=edits[editfocus];
		for(int i=editfocus;i>startnonactive;i--)
			edits[i]=edits[i-1];
		editfocus=startnonactive++;
		edits[editfocus]=tmp;
		return 0;
		}
	if(editfocus<startactive) {
			startactive--;
			if(editfocus==startactive) {
				return 0;
				}
			Editor *tmp=edits[editfocus];
			for(int i=editfocus;i<startactive;i++)
				edits[i]=edits[i+1];
			editfocus=startactive;
			edits[startactive]=tmp;
			return 0;
			}
	if(editfocus==startnonactive) {
			if(nredit>startnonactive)
				startnonactive++;
			else
				editfocus=nredit-1;
			return 0;
			}
	return -1;
	}

Editor *editfile(const char *file) {
	Editor *ed=newfile(file);
	if(!ed)
		return NULL;
	if(addlast()>-1) {
		ed->toscreen();
		return ed;
		}
	return ed;
	}
Editor *argfile(void) {
	Editor *ed;
	while(miter<margc) {
		char abpath[PATH_MAX];
		expandfilename(abpath, margv[miter++]);
		if((ed=newfile(abpath)))
			return ed;
		}
	return NULL;
	}


extern void dynlink(void);
#ifdef HAS_SETNEWHANDLER
#ifdef HAVE_NEW
#include <new>
	using std::new_handler;
	using std::set_new_handler;
#else
#include <new.h>
#endif

int memoryexceeded=0;
void mynewhandler(void) {
	memoryexceeded=1;
	int j,i;
	if(takescr)
		delwin( takescr);
	for(i=0;i<nredit;i++) {
		if((!edits[i]->modified)&&(!edits[i]->changed)) {
			delete edits[i];
			edits[i]=NULL;
			}
		else {
			if(edits[i]->selbuf) {
				myfree( edits[i]->selbuf);
				}
			edits[i]->selbuf=NULL;
			edits[i]->delundo() ;
			edits[i]->initundo() ;
			}
		}
	for(i=0,j=1;i<nredit;i++) {
		if(!edits[i]) {
			j=maxnum(j,i+1);
			for(;j<nredit&&!edits[j];j++) 
				;
			if(j<nredit)
				edits[i]=edits[j];
			else  
				break;
			}
		}
	nredit=i;
	startnonactive=nredit;
	char newer[]="Memory exceeded!!!";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	write(STDERR_FILENO,newer,sizeof(newer));
#pragma GCC diagnostic pop
#ifndef NOSIGNALS
	raise(SIGSTOP);
#endif
	}
#endif
#define symsize 80

#ifdef GETOTHERTERM
SCREEN *thescreen=NULL;
void relive(void) {
	int i;
	if(!isendwin())
		endwin();
	if(thescreen) {
		for(i=(piddenlen-1);i>=0;--i)
			if(pidden[i]!=pid)
				kill(pidden[i],18);
		for(i=(piddenlen-1);i>=0;--i)
			if(pidden[i]!=pid)
				kill(pidden[i],28);
		
		piddenlen=0;
		if(pidden) {
			myfree(pidden);
			pidden=NULL;
			}

#ifdef HASDELSCREEN
		delscreen(thescreen);
#endif
		thescreen=NULL;
		}
	}
extern int startterminal;

#ifdef DEBUGON
extern int errormessage(char *format ...) ;
int errormessage(char *format ...) {
	va_list ap;
FILE *errorfp=fopen("/tmp/bederrors","a");
if(!errorfp)
	errorfp=stderr;
	va_start(ap,format);
   int ret=vfprintf(errorfp,  format,  ap);
	va_end(ap);
	fclose(errorfp);
return ret;
	}
#else

#ifndef OLDGCCVARMACRO
#define errormessage(commando,...)  
#else
#define errormessage(commando,args...)   
#endif

#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-overflow"
void getotherterm(void ) {
const int waittime=2;
static time_t nexttime=0;
time_t currenttime=time(NULL);
if(currenttime>nexttime) 
{
nexttime=currenttime+waittime;
	DIR *dir;
	uid_t curuid;
	struct dirent *ent;
	struct stat st;
	char **ttys;
	char *owns;
	pid_t **pids;
	int lasttty=0,len,ttymax=100;
	short *pidlens,maxpids=30;
	char sym[symsize],buf[256];
	char ttynames[][10]= {
		"/dev/tty",
		"/dev/pts"
		};
	int ttynamenr=2,ttynamelen=8;
startterminal=0;
	curuid=getuid();
      dir= opendir("/proc");
     if(chdir("/proc")==-1)
		perror("getotherterm cd /proc");

     	ttys=(char **)alloca(ttymax*sizeof(char *));
	pids=(pid_t **) alloca(ttymax*sizeof(pid_t*));
	owns=(char *) alloca(ttymax*sizeof(char));
	memset(owns,0,ttymax);
	pidlens=(short *) alloca(ttymax*sizeof(short));
      while((ent=     readdir(dir))) {
	      if(!stat(ent->d_name,&st)) {
			if(st.st_uid==curuid&&(ent->d_name[0])>='0'&&(ent->d_name[0])<='9') {
				int i,j,k;
				FILE *fp;
				sprintf(buf,"%s/stat",ent->d_name);
				if((fp=fopen(buf,"r"))) {
					char state[10];

					int pid,ppid,pgrp,ses,tty,tpgid;
					char com[50];
					if(!fgets(sym,symsize,fp)) {
						fprintf(stderr,"fgets from %s failed\n",buf);

						}
					sscanf(sym,"%d%s%s%d%d%d%d%d",&pid,com,state,&ppid,&pgrp,&ses,&tty,&tpgid);
					fclose(fp);
					for(i=0;i<2;i++) {
						sprintf(buf,"%s/fd/%d",ent->d_name,i);
					       if((len=readlink(buf, sym, symsize))>0) {
							sym[len]='\0';
				for(k=0;k<ttynamenr;k++) {
					if(!strncmp(ttynames[k],sym,ttynamelen)) {
							for(j=0;j<lasttty;j++) {
								if(!strncmp(sym,ttys[j],len)) {
									if(state[0]!='T') {
										if((!pidlens[j])||(pids[j][pidlens[j]-1]!=pid))
											pids[j][pidlens[j]++]=pid;
										}
									break;
									}
								}
							if(j==lasttty) {
			
								ttys[j]=	(char *)alloca(len+1);
								memcpy(ttys[j],sym,len);
								ttys[j][len]='\0';
								pids[j]=(pid_t *)alloca(maxpids*sizeof(pid_t));
								if(state[0]!='T') {
									pids[j][0]=pid;
									pidlens[j]=1;
									}
								else
									pidlens[j]=0;
								lasttty++;
								}
							if(pgrp==tpgid) 
								owns[j]=1;
							break;
							}
						}
							}
						}

					}

				}
			}
	}
closedir(dir);
{

struct stat st;
	//		int in,uit,err;
			FILE *in,*uit,*err;
SCREEN *scr;

int take;
for(take=lasttty-1;take>=0;take--) {
/*
	int i;
	for(i=0;i<pidlens[take]&&pids[take][i]!=pid;i++) 
		;
	if(i<pidlens[take])
		continue;
*/
	if(owns[take]&&(!stat(ttys[take],&st)))  {
		if(st.st_uid==curuid)
			break;
		}
	}
if(take<0) 
	while(1) {
		NODEST();
		pause();
		}
relive();
pidden= myallocar(pid_t,pidlens[take]) ;
memmove(pidden,pids[take],pidlens[take]*sizeof(pid_t));
piddenlen=pidlens[take];

errormessage("%d: kill -19",pid);

for(int i=0;i<piddenlen;i++) {
	if(pidden[i]!=pid) {
		kill(pidden[i],19);
		errormessage(" %d",pidden[i]);
		}
	}
errormessage("\n");
//atexit(relive);
			in=fopen(ttys[take],"r");
        		uit=fopen(ttys[take],"w");
			err=fopen(ttys[take],"w");

			dup2(fileno(in),STDIN_FILENO);
			dup2(fileno(uit),STDOUT_FILENO);
			dup2(fileno(err),STDERR_FILENO);
 scr=newterm(NULL, stdout,stdin);
// scr=newterm(NULL, uit,in);
 
thescreen=scr;
set_term(scr);

		clearok(stdscr,FALSE);
		clearok(curscr,FALSE);
		resetcolors() ;
		curses_init_curses.setscreen();
		curses_init_curses.reset();
		OldMainScreenwidth=MainScreenwidth;
}
}
}
#pragma GCC diagnostic pop
#endif //GETOTHERTERM
#if defined(XCURSES)
        char *XCursesProgramName = "xbed";
#endif
int findoutxwindows(void) {
	if(getenv("DISPLAY"))
		return 1;
	else
		return 0;
}
int withinxterm=findoutxwindows();
#ifndef NOSIGNALS
void actionsignal(int sig) {

	switch(sig) {
#ifndef DONTINTCURSES
		case SIGWINCH: 
			signal(SIGWINCH ,resizehandler);
#else
		case SIGUSR1 : ;
#endif
		case SIGINT : {
			signal(SIGINT ,omnihandler);
			redistribute=1;
			curses_init_curses.reset();
			break;
			};
		case SIGHUP:;
#ifdef SIGSYS 
		case SIGSYS : ;
#endif
		case SIGTERM : {
#ifdef GETOTHERTERM
errormessage("actionsignal(%d)\n",sig);
		getotherterm( ) ;
#else
		exit(4);
#endif
//		signal(sig ,realtermhandler);
			break;
			};

		case 0:
		 	{
/*
		 	 newact.sa_handler=realtermhandler;
			newact.sa_mask=blockallmask;
		 	 newact.sa_flags=SA_ONESHOT;
			sigaction(1,&newact,&oldact);
*/
signal(SIGPIPE,SIG_IGN);
#ifndef DONTINTCURSES
			signal(SIGWINCH ,resizehandler);
			signal(SIGINT ,omnihandler);
#else
/*DWORD was;
int inhan=PDC_get_input_fd();
GetConsoleMode(inhan,&was);
fprintf(stderr,"%ld\n",was&ENABLE_PROCESSED_INPUT);
SetConsoleMode(inhan,was|ENABLE_PROCESSED_INPUT);
*/
SetConsoleCtrlHandler(ctrlhandler,TRUE);
#endif
			signal(SIGUSR1 ,omnihandler);

			signal(SIGHUP ,realtermhandler);
			signal(SIGTERM ,realtermhandler);
			signal(SIGQUIT ,quithandler);

		if((origstophandler=signal(SIGTSTP,stophandler))!=SIG_IGN&&origstophandler)
//			origconthandler=signal(SIGCONT,SIG_DFL);
			origconthandler=signal(SIGCONT,conthandler);


			};
			break;
		default:
			fprintf(stderr,"Unknown signal %d\n",sig);
		};
	}
#endif	
Editor **deathcells;
int maxdeath=5,gossip=0;
extern void doargs(void);
extern int fillhelp(void);
 #define _GNU_SOURCE 1
       #define _FILE_OFFSET_BITS 64

#ifndef __MINGW32__
#include <sys/resource.h>
#endif
#ifdef MEMORY_TRACE	
#include <mcheck.h>
#endif
extern int waitforsigwinch(void) ;
#ifdef SEGMENMESSAGE /*I wonder if it always went alright this way */
struct sigaction segmentationact;
#define prognamesize 512
void segm_handler(int signr) {
	pid_t pid= getpid();
	char *cwdpath=get_current_dir_name();
	char *email="binaryeditor@gmx.com";
	char progname[prognamesize+1];
	char procin[15];
	int len;
	sprintf(procin,PROCEXEFILENAMEFORMAT,pid);
       len=readlink(procin, progname, prognamesize);
	progname[len]='\0';
	fprintf(stderr,"\nYou discovered a bug in %s\nType at the commandline:\ngdb %s %s/core\nType at (gdb):\nbt\nand send the output to %s\nAdding some information, about what you where doing\nbefore this bug revealed itself, is very helpfull.\n",progname,progname,cwdpath,email);
	}

int main(int argc, char **argv) 

#ifdef TRYCATCH
try 
#endif
{


    segmentationact.sa_handler = segm_handler;
     sigemptyset(&segmentationact.sa_mask);
     segmentationact.sa_flags = SA_ONESHOT;
     sigaction(SIGSEGV,&segmentationact,NULL);
#else
#ifdef _WIN32
OSVERSIONINFO WindowsVersion;
/*
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
fprintf(stderr,"CtrlHandler %d\n", fdwCtrlType);
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
	omnihandler(SIGINT);
	return true;
	};
return false;
}
*/
#endif
int main(int argc, char **argv) 

#ifdef TRYCATCH
try 
#endif
{
/*
if(!FreeConsole()) {
	fprintf(stderr,"FreeConsole failed\n");
		}

if(!AllocConsole())
	fprintf(stderr,"AllocConsole failed\n");
HANDLE outhandle=GetStdHandle(STD_OUTPUT_HANDLE	);
SetStdHandle(STD_INPUT_HANDLE	,outhandle);
*/
#endif
#ifdef FORCEPOSIXDEFAULTLOCALE
setlocale(LC_ALL, "C");
setlocale(LC_TIME, "GMT");
setenv("LC_ALL","C",1);
#endif
#ifdef MEMORY_TRACE	
#include <mcheck.h>
	mtrace();
#endif
#ifdef _WIN32
WindowsVersion.dwOSVersionInfoSize=sizeof(WindowsVersion);
if(!GetVersionEx(&WindowsVersion))
	WindowsVersion.dwOSVersionInfoSize=0;
const char *winver;
switch( WindowsVersion.dwPlatformId) {
	case VER_PLATFORM_WIN32_WINDOWS: winver="WINDOWS";break;
	case VER_PLATFORM_WIN32_NT	: winver="NT";break;
	default: winver="OTHER";	break;
	};
setenv("WINDOWS_PLATFORM",winver,1);
#endif
	
	margc=argc;
	margv=argv;
	doargs();
	pid=getpid();
	starttrace();
	startdebug();

fillhelp();
	INITDEST();
#ifdef HAS_SETNEWHANDLER
	set_new_handler(mynewhandler);
#endif
/*
extern int searshow(void) ;
searshow();
getchar();
*/

	edits=myallocarpr(Editor **,Editor *,maxedit);
	deathcells=myallocarpr(Editor **,Editor *,maxdeath);
	setconfig();

extern int after_initscr;
	if(!after_initscr) {
		curses_init_curses.curses_init();
		defaultkeys(keyedit);
		}
	resetcolors();
#ifdef XCURSES
	atexit( XCursesExit);
#endif	
	if((thelines-menuon)<2) {
		do {
			beep();
			waitforsigwinch();
			curses_init_curses.resized();
			} while((thelines-menuon)<2) ;
		}
	if(!argfile()) {
		if(!emptyfile())
				exit(5);
		}
	editfocus=0;
	startactive=0;
	startnonactive=nredit;
#ifndef PD24
extern int keyconfig(char *filename=NULL) ;
extern int	dokeyconfig;
	if(dokeyconfig) {
		return keyconfig((char*)((dokeyconfig<margc)?margv[dokeyconfig]:NULL));
		}
#endif
 
	edits[editfocus]->menus();
#ifdef DEBUGFLAG
char pidstr[10];
	 showstatestate(pidstr,sprintf(pidstr,"%ld",pid));
	wrefresh(stdscr );
#endif
#ifndef NOSIGNALS
	actionsignal( mysigsetjmp ( screenjmp,1 ) );
#endif

	int ret;

#if 0
				{			       
struct rlimit rlim;
fputs("RLIMIT_STACK\n",stderr);
	if(getrlimit(RLIMIT_STACK ,&rlim)!=0)
			perror("getrlimit");
	fprintf(stderr,"cur=%llu\n",(unsigned long long)rlim.rlim_cur);
	fprintf(stderr,"max=%llu\n",(unsigned long long)rlim.rlim_max);
	rlim.rlim_cur*=10LL;
	fprintf(stderr,"cur=%llu\n",(unsigned long long)rlim.rlim_cur);

	if(setrlimit(RLIMIT_STACK ,&rlim)!=0)
			perror("setrlimit");
	if(getrlimit(RLIMIT_STACK ,&rlim)!=0)
			perror("getrlimit");
	fprintf(stderr,"cur=%llu\n",(unsigned long long)rlim.rlim_cur);
	fprintf(stderr,"max=%llu\n",(unsigned long long)rlim.rlim_max);

				};		
#endif

extern void getstarttime() ;
getstarttime() ;

#ifdef _WIN32
//SetConsoleCtrlHandler(CtrlHandler, TRUE);
#endif
	while(1)  {
#ifdef TRYCATCH
try 
#endif
{
		if((ret=edits[editfocus]->fileok())!=0) {
			Editor *ed=edits[editfocus];
			endedit(ed);
			if(ret>0) 
				newfile(ed->mem.filename);
			delete ed;
			if(nredit<1&&!argfile())
				exit(0);
			}
		else {

#ifdef DONTINTCURSES 
			SETPDDEST(1);
#else
			NODEST();
#endif
			Editor *cured=edits[editfocus];
//extern char showfileinfo;
//			showfileinfo=1;
			//
			messagebuf[0]=0;
				 cured->processkey(); 
			while(gossip)		
				delete deathcells[--gossip];
			SETDEST(1);
			}
		}

#ifdef TRYCATCH
	catch(...) {
		fprintf(stderr,"While: Caught exception\07\n");
		}; 
#endif
		};
	
	}
#ifdef TRYCATCH
catch(...) {
	fprintf(stderr,"Main: Caught exception\07\n");
	}
#endif

#if defined(USE_KEYPAD) && !defined(XCURSES)
#ifdef PDCURSES

#define selwgetch(x) wgetch(x)
#else
#define selwgetch(x) wgetch(x)
#endif
int keyavailable=ERR;
int  thekbhit(void)
{
	  if (keyavailable == ERR) {
	 	nodelay(stdscr, TRUE);
		 keyavailable = selwgetch(stdscr);
	 	nodelay(stdscr, FALSE);
	  	}
	    return keyavailable != ERR;
}

int newwgetch(WINDOW *win) {  
	  if (keyavailable == ERR) 

		return selwgetch(win);  
	  else {
		 int ch=keyavailable;
		keyavailable=ERR;
		  return  ch;
		}
	}
#else
#define newwgetch wgetch
#endif

#ifdef USE_KEYPAD
#ifdef XCURSES
#if defined (__cplusplus) || defined (__cplusplus__)
extern "C"
   {
#endif
#define thekbhit XCurses_kbhit
extern int XCurses_kbhit(void);

#if defined (__cplusplus) || defined (__cplusplus__)
}
#endif
#else
extern int  thekbhit(void);
#endif
static int keybuf[10],keyiter=0,keymax=0;
bool realkbhit(void) {
	if(keyiter<keymax)
		return !0;
	return thekbhit();
	}
#endif
extern int newwgetch(WINDOW *win)  ;
#define oldmywgetch(win) newwgetch(win)
#ifndef USE_KEYPAD
static int twocharsfromone=0;
#endif
int realmywgetch(WINDOW *w) {
	int key;
#ifdef USE_KEYPAD
	if(keyiter<keymax)
		key=keybuf[keyiter++];
	else {	
keypad(w,TRUE);
meta(w,TRUE);
/*#ifdef PDCURSES
PDC_save_key_modifiers(FALSE);
PDC_return_key_modifiers(FALSE);
#endif
*/
static const int maxspecial=3;
static const int ofesc=2;
static const int maxesckey=(256-maxspecial)+(256-ofesc);
if((key=oldmywgetch(w))>=(256-maxspecial) ) {
	keyiter=0;

	if(key<maxesckey) {
		if(key>=320) {
			if(key>406&&key<417)
				keybuf[0]=((unsigned char)(key-359));
			else
				keybuf[0]=((unsigned char)(key-320));
			}

		else
			keybuf[0]=((unsigned char)(key-320-ofesc));
		key=27;
		keymax=1;
		}
	else {
static const int maxmore=254;
	static const int maxbytes=(int)ceil(log((KEY_MAX-maxesckey+1))/log(maxmore));
		int i;
		key-=maxesckey;
		keymax=0;
		for(i=0;i<maxbytes;keymax++,key/=maxmore,i++)
			keybuf[keymax]=(key%maxmore);
		key=0;
		}
	}
else {
	if(key==27) {
//		keybuf[0]=254;keyiter=0;keymax=1;
#ifdef NCURSES_VERSION
		ndelay;
		keyiter=0;keymax=1;
		if((keybuf[0]=oldmywgetch(w))== ERR ) {
			keybuf[0]=secondescape;			
			}
		nocbreak();
		cbreak();
#else
			keybuf[0]=secondescape;keyiter=0;keymax=1;
#endif
		}
	}
	}
#else
	if(twocharsfromone) {
		if(twocharsfromone==27) {
			twocharsfromone=0;
				ndelay;
				if((key=oldmywgetch(w))== ERR )
					key= secondescape ;

				nocbreak();
				cbreak();
			}
		else {
			key=twocharsfromone;
			twocharsfromone=0;
			}
		}
	else {
			key=oldmywgetch(w);
			if(key&0x80) {
				if(key!=ERR) {
					twocharsfromone=key&0x7f;
					waitinput=0;
					key=27;
					}
				}
			else 
				if(key==27)
					twocharsfromone=27;
		}
#endif

return key;
}
#define KBNODELAY
#ifndef USE_KEYPAD
int realkbhit(void) {
	int ch;
if(twocharsfromone)
	return 1;
#ifdef KBNODELAY
	nodelay(stdscr, TRUE);
#else
	ndelay;
#endif
	if((ch=getch())!=ERR) {
		ungetch(ch);
		}
	else
		ch=0;
#ifdef KBNODELAY
	nodelay(stdscr, FALSE);
#else
	nocbreak();
	cbreak();
#endif
	return ch;
	}
#endif
int mykbhit(void) {  
	int key;
	if((key=playkeys.peek())!=KEYSTROKE_ERROR) {
		if(key==INVALID_KEY) {
			playkeys.get();
			return 0;
			}
		else
			return !0;
		}
	if((key=realkbhit()))
		return !0;
	if(isrecording==1) {
//		if(recordkeys.last()!=INVALID_KEY) {
				addtorecordkeys(INVALID_KEY);
//				}
		}
	return 0;
	}
int mywgetch(WINDOW *win) {  
	int key; /*should be int for KEYSTROKE_ERROR */

	if((key=playkeys.get())==KEYSTROKE_ERROR) {
		key=realmywgetch(win);
		}
	if(isrecording==1) {
		if(addtorecordkeys(key)==KEYSTROKE_ERROR) {
				return ERR;
				}
		}
	return key;
	}
int ggetch(WINDOW *w) {
	int key;
#ifndef DONTINTCURSES 
	waitinput=1;
	if((mysigsetjmp ( resizejmp,1 ) )) {
		waitinput=0;
		signal(SIGWINCH,resizehandler);
		return REFRESH ;
		};
#endif
key=mywgetch(w);
#ifdef DONTINTCURSES 
#ifndef NOSIGNALS
if(wantsignal) {
	kill(pid,wantsignal);
	}
#endif
if(resizeflag)
	return REFRESH ;
#else
	waitinput=0;
#endif
	if(key==-1) {
		#ifdef GETOTHERTERM
		getotherterm( ) ;
		return REFRESH ;
		#else
		abort();	
		#endif
		}
	return key;
	}




