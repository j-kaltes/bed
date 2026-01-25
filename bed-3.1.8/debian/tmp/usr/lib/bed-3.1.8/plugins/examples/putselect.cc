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
/*                                                                           */
/* Thu May 30 09:32:55 2002                                                  */
#include "global.h" 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>  
#include "screenpart.h"
#include "dialog.h"
#include "screen.h"
//#define DEBUGFLAG
//#define waslong int
#define waslong long
#ifdef OLDGCCVARMACRO
	#ifdef DEBUGFLAG
	#define NDEBUG(VARPAR...)  {fprintf(output,"%d: ",pid); fprintf(output, VARPAR);fflush(output);}
	#else
	#define NDEBUG(VARPAR...)
	#endif
#else
	#ifdef DEBUGFLAG
	#define NDEBUG(...) {fprintf(output,"%d: ",pid); fprintf(output, __VA_ARGS__);fflush(output);}
	#else
	#define NDEBUG(...)
	#endif
#endif
BEDPLUGIN

#ifdef UNBLOCKSIGNALS
void unblock(int signum); 
#else
#define unblock( signum)
#endif

/*
#define stopmainthread DEBUG("stop main unimplemented")
#define contmainthread DEBUG("cont main unimplemented")
*/
#define stopmainthread stopmain()
#define contmainthread contmain()

//s/targets\[.\] = (CARD32)//g
#if defined(__x86_64__)
//#define ZEROTARGETS ,0
#define ZEROTARGETS 
using CARDBITS=CARD64;
#else
#define ZEROTARGETS 
using CARDBITS=CARD32;
#endif
void setsighandlers(void) ;


#define maxnum(x,y) (((x)>(y))?(x):(y))
#define minnum(x,y) (((x)<(y))?(x):(y))

extern pid_t pid;
#ifdef DEBUGFLAG
#define EVENTNAMES 1
FILE  *output=fopen("/tmp/putselect.txt","a+");
#endif

#ifdef EVENTNAMES
#include "eventnames.h"
#endif
#include "varmacro.h"
//FILE *output=stderr;
class Editor;



class GetEvent {
	private:
	XEvent transxev;
	pthread_mutex_t mut;
	pthread_cond_t  cond;
	public:
	 GetEvent(void) {
	 pthread_mutex_init(&mut,NULL);
	 pthread_cond_init(&cond,NULL);
NDEBUG("after pthread_cond_init(&cond,NULL);\n");
		start();
	}
	 void unlock(void) {
		pthread_mutex_unlock(&mut);
	 	}
	 ~GetEvent() {
	NDEBUG(" before pthread_cond_destroy(&cond);\n");
	pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mut);
	NDEBUG(" after pthread_cond_destroy(&cond);\n");
	 	}
	void start(void) {
		transxev.type=None;
		}
	int setevent(XEvent *xev) {
NDEBUG("setevent\n");
		  pthread_mutex_lock(&mut);
NDEBUG("after mutex_lock\n");
		  transxev=*xev;
NDEBUG("after		  transxev=*xev; \n");
   pthread_cond_signal(&cond);
NDEBUG("after cond_signal\n");
		  pthread_mutex_unlock(&mut);
NDEBUG("after mutex_unlock\n");
return 0;
		}
int getotherxev(XEvent *xev) {
		pthread_mutex_lock(&mut);
		   int retcode = 0;

#define BUGGY_pthread_cond_timedwait
#ifndef BUGGY_pthread_cond_timedwait
	  struct timeval now;
	  struct timespec timeout;
	      gettimeofday(&now,NULL);
		  timeout.tv_sec = now.tv_sec + 5;
		  timeout.tv_nsec = now.tv_usec * 1000;
	    while (transxev.type==None&&retcode!=ETIMEDOUT)  {
				NDEBUG("Before pthread_cond_timedwait\n");
				retcode=pthread_cond_timedwait(&cond, &mut,&timeout);
				NDEBUG("AFter pthread_cond_timedwait\n");
			}
#else
	    while (transxev.type==None)  
       retcode=pthread_cond_wait(&cond, &mut);
#endif

		*xev=transxev;
	     pthread_mutex_unlock(&mut);
		return retcode;
	}
	};

static pthread_mutex_t selmutex;
static Atom xa_clipboard;
static Display	*selectdisplay;
//XSetIOErrorHandler

class Program {
	private:
 int screen_num;
 Atom	 xa_incr;
 Atom xa_raw ;
 Atom xa_datatype ;
 Atom	 xa_targets ;
 Atom data_atom;
Atom xa_compound_text, xa_multiple,  xa_text, xa_timestamp;
const char *errorbuf;
Time selection_time;
void initselection(void) ;
	public:
	sigjmp_buf eventjmp;
int	caneventjmp;
GetEvent pastget;
GetEvent propertyget;
	Window selectwin;
  pthread_t threadselection,threadmain;
const char *iserror(void) {
	return errorbuf;
	};

int getselection(XSelectionEvent	*xsel,unsigned char **data);
int initmainwin(void) ;
int pastclip(Editor *ed,Atom type=XA_STRING,Atom selection=XA_PRIMARY) ;
void getselectionnotify(Atom selection,Atom target,XEvent *xev) ;
int pastspecial(Editor *ed,Atom selection=XA_PRIMARY) ;
int pastclipboard(Editor *ed,Atom selection=XA_PRIMARY) ;
int eventloop(void) ;
void	endwindow(void);
Program(void);
~Program() ;
int  giveselection(XSelectionRequestEvent *req) ;
int sayselection(Time eventtime,Atom selection=XA_PRIMARY) ;
};


void * initselectionthread(void *arg) {
       sigset_t sigs;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	sigemptyset(&sigs);
#ifdef DONTINTCURSES
	sigaddset(&sigs, SIGUSR1) 
#endif
	sigaddset(&sigs,SIGWINCH);
	sigaddset(&sigs,SIGCONT);
	sigaddset(&sigs,SIGSTOP);
	sigaddset(&sigs,SIGTSTP);

	sigaddset(&sigs,SIGINT);
	sigaddset(&sigs,SIGTERM);
	sigaddset(&sigs,SIGHUP);

	sigaddset(&sigs,SIGQUIT);
	sigaddset(&sigs,SIGALRM);
	pthread_sigmask(SIG_BLOCK,&sigs,NULL);
	setsighandlers() ;
	Program *prog=(Program *)arg;
	if(!prog->iserror())
		prog->eventloop();
	return nullptr;
	}
Program *startsel(void) {
	Program *seldata=new Program();
	if(!seldata)
		return NULL;
	if(!seldata->iserror()) {
	  pthread_create(&(seldata->threadselection), NULL, initselectionthread, seldata);
	seldata->threadmain=pthread_self();
	}
	else  {
	seldata->threadselection=seldata->threadmain;
	delete seldata;
	return NULL;
	}
return seldata;
}
//static Program *seldata=startsel();;
static Program *seldata=NULL;

#ifdef OLDGCCVARMACRO
	#ifdef DEBUGFLAG
	#define error(VARPAR...)  {fprintf(output,"%d: ",pid);if(seldata) if(pthread_equal(seldata->threadselection,pthread_self())) {	fprintf(output,"thread: "); } fprintf(output, VARPAR);fflush(output);}
	#define DEBUG(VARPAR...) error(VARPAR)
	#else
	#define error(VARPAR...)
	#define DEBUG(VARPAR...)
	#endif
#else
	#ifdef DEBUGFLAG
	#define error(...) {fprintf(output,"%d: ",pid);if(seldata) if(pthread_equal(seldata->threadselection,pthread_self())) {	fprintf(output,"thread: "); } fprintf(output, __VA_ARGS__);fflush(output);}
	#define DEBUG(...) error(__VA_ARGS__)
	#else
	#define error(...) 
	#define DEBUG(...)
	#endif
#endif

void unlocks(void) {
	DEBUG("unlock\n");
	pthread_mutex_unlock(&selmutex);
	if(seldata) {
		 seldata->pastget.unlock(); 
		 seldata->propertyget.unlock(); 
		}
	contmainthread;
	}

int signalen[]={SIGINT,SIGTERM,SIGHUP};
const int signalenlen= sizeof(signalen)/sizeof(signalen[0]);
int sigvolg(int sig) {
	for(int i=0;i<signalenlen;i++)
		if(sig==signalen[i])
			return i;
	return signalenlen;
}
void unlockhandler(int sig) {
	DEBUG("unlockhan\n");
	unlocks();
	unblock(sig);
	}	
void (*orighandler[signalenlen+1])(int);
void selectionhandler(int sig) {
	unlocks();	
	if(seldata) {
		if(pthread_equal(pthread_self(),seldata->threadselection))  {
		DEBUG("Selection thread\n");
			pthread_kill(seldata->threadmain,SIGUSR2);
			}
		else {
			DEBUG("Main thread\n");
			pthread_kill(seldata->threadselection,SIGUSR2);
			}
		}
	DEBUG("int %d\n",sig);
	int volg=sigvolg(sig);
	if( orighandler[volg]) { 
		DEBUG("Original handler\n");
		void (*tmphandler)(int)=orighandler[volg];
		orighandler[volg]=NULL;
		return tmphandler(sig);
		}

	unblock(sig);
	}
void pipehandler(int sig) { 
	DEBUG("X Error Handler ");
	if(seldata&&pthread_equal(pthread_self(),seldata->threadselection)) {
		unlocks();
//		kill(pid,SIGUSR2);
		pthread_kill(seldata->threadmain,SIGUSR2);
		if(seldata->caneventjmp) {
			DEBUG("jmp to loop\n");
			unblock(sig);
		     siglongjmp(seldata->eventjmp, 4);
		}
		}
	else {
		unlocks();
		if(seldata)
			pthread_kill(seldata->threadselection,SIGUSR2);
		DEBUG("jmp to main\n");
		pthread_kill(seldata->threadmain,SIGINT);
		}
	DEBUG("after\n");
	}

int ioerrorhandler(Display *dis) {
	pipehandler(SIGPIPE);
	return 0;
	}
int errorhandler(Display *dis,XErrorEvent *ev) {
	return ioerrorhandler(dis);
	}
Program::~Program() {
	if( threadselection!=threadmain)
		pthread_cancel(threadselection);
	pthread_mutex_destroy(&selmutex);
	
	endwindow();
	seldata=NULL;

}

static const char	cantopen[]="Can't open display";
int Program::initmainwin(void) {
	errorbuf=NULL;
	if(!withinxterm)
		return -1;
	if(!(selectdisplay=XOpenDisplay(NULL))) {
		errorbuf=cantopen;
		withinxterm=0;
		return -1;
		}
if ((fcntl(ConnectionNumber(selectdisplay), F_SETFD, 1)) == -1) {
        DEBUG("putselect: child cannot disinherit TCP fd");
	}
else {
        DEBUG("putselect: did disinherit TCP fd");
}


	screen_num =DefaultScreen(selectdisplay);
	Window	parwin=RootWindow(selectdisplay,screen_num);
	selectwin = XCreateSimpleWindow(selectdisplay,parwin, 
				0, 0, 1, 1, 0, BlackPixel(selectdisplay,
				screen_num), WhitePixel(selectdisplay,screen_num));
	return selectwin;
	}


void Program::initselection(void) {
	 pthread_mutex_init(&selmutex,NULL);
	xa_clipboard=XInternAtom( selectdisplay,"CLIPBOARD",False);
	xa_raw = XInternAtom(selectdisplay, "RAW", False);
	xa_datatype = XInternAtom(selectdisplay, "DATATYPE", False);
	 xa_targets = XInternAtom(selectdisplay, "TARGETS", False);
	data_atom=XInternAtom(
		selectdisplay,"PASTSELECTION",False); // Use existing Atom?

#ifdef ADDINCR
	xa_incr = XInternAtom(selectdisplay, "INCR", False);
#endif
       xa_compound_text = XInternAtom(selectdisplay, "COMPOUND_TEXT", False);
       xa_multiple = XInternAtom(selectdisplay, "MULTIPLE", False);
    	 xa_text = XInternAtom(selectdisplay, "TEXT", False);
    	 xa_timestamp = XInternAtom(selectdisplay, "TIMESTAMP", False);
	XFlush(selectdisplay);
	/*
struct sigaction newact;
 newact.sa_handler=unlockhandler;
 sigemptyset(&newact.sa_mask);
 newact.sa_flags = SA_RESTART;
 newact.sa_flags = 0;
sigaction(SIGUSR2,&newact,NULL);
 newact.sa_handler=pipehandler;
 sigemptyset(&newact.sa_mask);
 newact.sa_flags = SA_RESTART;
sigaction(SIGPIPE,&newact,NULL);
*/
signal(SIGUSR2,unlockhandler);
signal(SIGPIPE,pipehandler);
}
void unblockhandler(int sig) {
sigset_t sigs;
sigemptyset(&sigs);
for(int i=0;i<signalenlen;i++) 
	sigaddset(&sigs,signalen[i]);
pthread_sigmask(SIG_UNBLOCK,&sigs,NULL);
signal(SIGUSR1,unblockhandler);
DEBUG("Unblockhandler\n");
}
void setsighandlers(void) {

for(int i=0;i<signalenlen;i++) {
	if(!orighandler[i]) {
	/*
		struct sigaction newact,oldact;
		 newact.sa_handler=selectionhandler;
		 sigemptyset(&newact.sa_mask);
		 newact.sa_flags = SA_RESTART;
		sigaction(signalen[i],&newact,&oldact);
		orighandler[i]=oldact.sa_handler;
		*/
		orighandler[i]=signal(signalen[i],selectionhandler);

		}
	}
orighandler[signalenlen]=NULL;
XSetErrorHandler(errorhandler );
XSetIOErrorHandler(ioerrorhandler );
}
int seldataok(void) {
	if(!withinxterm)
		return 0;
	if(!seldata) {
		seldata=startsel();
		}
	if(!seldata)
		return 0;
	if(seldata->iserror())
		return 0;
	setsighandlers() ;
	return 1;
	}
Program::Program(void) {

caneventjmp=0;
pthread_mutex_lock(&selmutex);
	if(initmainwin() >0) {
		initselection();
		}
pthread_mutex_unlock(&selmutex);
	//setsighandlers() ;
}

#define XCheckNextEvent(selectdisplay,xev) XCheckIfEvent(selectdisplay,xev,eventproc,NULL)
Bool eventproc(Display *dis,XEvent *xev,XPointer arg) {
	return True;
}
Bool eventtypeproc(Display *dis,XEvent *xev,XPointer arg) {
	long ltype=(long)arg;
	int type=(int)ltype;
	DEBUG("special: Event %s\n",event_names[xev->type]);
	if(type==xev->type)
		return True;
	return False;
}
Bool eventshowproc(Display *dis,XEvent *xev,XPointer arg) {
	DEBUG("special: Event %s\n",event_names[xev->type]);
	return False;
}
#define USEONEDISPLAY
void Program::endwindow(void) {
	if(selectdisplay)  {
		XDestroyWindow(selectdisplay, selectwin);
#ifndef USEONEDISPLAY
		 XCloseDisplay(selectdisplay);
#endif
		 	}
	}

Editor	*	shownselectioneditor=NULL;
OFFTYPE		shownselectionpos=-1,
		shownselectionother=-1;
char *shownselection=NULL;
char *rawselection=NULL;
char *copyshown=NULL;
int rawsize=-1;
extern Editor *getselectioneditor(void);
Editor *setselectioneditor(Editor *ed) ;
void selectionoff(void ) {
		shownselectionpos=-1;
		if(shownselection) {
			char *tmp= shownselection;
			shownselection=NULL;
		 	free(tmp);
			}
		if(rawselection) {
			char *tmp= rawselection;
			rawselection=NULL;
			free(tmp);
			}
}

#define freeglobal(name) {char *tmp=name;name=NULL;free(tmp);}
int Program::eventloop(void) {

if(sigsetjmp(eventjmp,2)) {
	pthread_mutex_lock(&selmutex);
	selectionoff();
	if(copyshown)
		freeglobal(copyshown)
       XDestroyWindow(selectdisplay, selectwin);
	XCloseDisplay(selectdisplay);
	withinxterm=1;
	while((withinxterm=1,initmainwin() <0)) {
		pthread_mutex_unlock(&selmutex);
		sleep(10);
		pthread_mutex_lock(&selmutex);
		}
	initselection();
	pthread_mutex_unlock(&selmutex);
	}
pthread_mutex_lock(&selmutex);
caneventjmp=1;
waslong event_mask=PropertyChangeMask;
XSelectInput(selectdisplay, selectwin, event_mask);
XEvent xev;

DEBUG("eventloop\n");
int disfd= ConnectionNumber(selectdisplay);
fd_set          sets;
while (1)  {

// Other Check functions don't seem to catch the selection event
while(XCheckNextEvent(selectdisplay,&xev)==False) 
//while(XCheckTypedWindowEvent(selectdisplay,selectwin,SelectionRequest,&xev)==False) 
{
		pthread_mutex_unlock(&selmutex);
		FD_ZERO(&sets);
		FD_SET(disfd, &sets);
		DEBUG("loop: Wait for select\n");
		select(disfd+1, &sets, 0, 0, NULL);
		pthread_mutex_lock(&selmutex);
		DEBUG("loop: after lock select\n");
		}
#ifdef EVENTNAMES
		DEBUG("%s\n",event_names[xev.type]);
#endif
	switch  (xev.type) {
		case SelectionNotify:

			pastget.setevent(&xev) ;
			  break;
		case SelectionRequest: 		
			       giveselection(&xev.xselectionrequest); 
			       break;
		case SelectionClear:
				selectionoff();
			       break;
		case PropertyNotify:
			propertyget.setevent(&xev) ;
			  break;
		} /* end switch */
	} /* end while */
return 0;
}





int Program::sayselection(Time eventtime,Atom selection) {
	  XSetSelectionOwner(selectdisplay,selection,selectwin, eventtime);
		 if(XGetSelectionOwner(selectdisplay, selection)!=selectwin) {
			XFlush(selectdisplay);
			DEBUG("XSetSelectionOwner failed\n");
			return -1;
			};
	DEBUG("sayselect %ld\n",selection);
         selection_time=eventtime;
	XFlush(selectdisplay);
	return 0;
	}
extern int mytmpname(char *buf,int len,const char *name) ;


#define MAXPAST 0xFFFFFF

/*
const waslong usecinsec=1000000L;
int uwaitfd(int fd,waslong usec) {
	      fd_set          sets;
	      struct timeval  tv;
	    tv.tv_sec = usec/usecinsec ;
	tv.tv_usec = usec%usecinsec ;
	FD_ZERO(&sets);
	 FD_SET(fd, &sets);
	if(select(fd + 1, &sets, 0, 0, &tv)>0) {
			 return 1;
		}
	 return 0;
	}
inline int Program::waitdisplay(waslong usec) {
	int disfd= ConnectionNumber(selectdisplay);
	return uwaitfd(disfd,usec);
	}
	*/
int Program::getselection(XSelectionEvent	*xsel,unsigned char **data)
{
     unsigned char *prop=NULL;
     Atom actual_type,property=xsel->property;
     int actual_format;
     unsigned waslong nitems;
     unsigned waslong bytes_after;
if( property==None) {
	return -1;	
		}
else {
	pthread_mutex_lock(&selmutex);
	propertyget.start(); 
	if((XGetWindowProperty(selectdisplay, selectwin, property, 0, MAXPAST, True, AnyPropertyType, &actual_type,
&actual_format, &nitems, &bytes_after, &prop)!=Success)||(actual_type==None&&bytes_after==0&&actual_format==0)) {
	pthread_mutex_unlock(&selmutex);
	return -1;
		}
	else {
		pthread_mutex_unlock(&selmutex);
		
		if(actual_type==xa_incr) {
			waslong datalen=*((waslong *)prop);
			unsigned char *databuf=(unsigned char *)Xmalloc(datalen);
			waslong datapos=0;
			XFree(prop);
			XEvent xev;
while(1) {
	DEBUG("Before getotherxev ");
	 if(propertyget.getotherxev(&xev)==ETIMEDOUT) {
			DEBUG("Timed out\n");
			XFree(databuf);
			datapos=-1;
		 break ;
	 	}
	if(xev.type==PropertyNotify) {
		propertyget.start(); 
		if(xev.xproperty.atom!=property||xev.xproperty.window!=selectwin||xev.xproperty.state!=PropertyNewValue) {
			DEBUG("not right property");
			continue;
		}
		DEBUG("right property");
		pthread_mutex_lock(&selmutex);
	if((XGetWindowProperty(selectdisplay, selectwin, property, 0, MAXPAST, True, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop)!=Success)||(actual_type==None&&bytes_after==0&&actual_format==0)) {
		pthread_mutex_unlock(&selmutex);
		DEBUG("Error in get property\n");
		XFree(databuf);
		datapos=-1;
		break;
		
		}
	else {
	pthread_mutex_unlock(&selmutex);
		if(nitems==0) {
			DEBUG("zero lenght\n");
			XFree(prop);
			*data=databuf;
			break;
			}
		DEBUG("len=%ld\n",nitems);
		memcpy(databuf+datapos,prop,nitems);
		XFree(prop);
		datapos+=nitems;
		}
										   
		}
	else {
		DEBUG("Different %s\n",event_names[xev.type]);
		propertyget.start(); 
		}
	}
		return datapos;
		}
	}
	}
if(nitems<0)
	nitems=0;
*data=prop;
return nitems;
}

		
int pastclipboard(Editor *ed,Atom selection=XA_PRIMARY) ;

int getselbuf(char **buf) ;

int pastspecial(Editor *ed,Atom selection=XA_PRIMARY) ;

int withinpastclipboard(Editor *ed,Atom selection=XA_PRIMARY) ;

#include "config.h"
#include CURSES_HEADER
/*#include <curses.h> */
#define GeenEditor
class Editor {
public:

int  GeenEditor putbuf(char *buf,OFFTYPE buflen);


int  GeenEditor loaddatatype(char *gegs,int endf,int addfield) ;
#include "faked.h"
int  GeenEditor insertshown(char *gegs, off_t endf) ;
/*static int Editor::lensel;
static char *Editor::selbuf;
*/
int pastselection(void) {
	int ret;
	if(!seldataok()) {
		 return withinpastclipboard(this);
		}
	ret= seldata->pastclipboard(this);
	return ret;
	}
int pastclipboard(void) {
	int ret;
	if(!seldataok()) {
		 return withinpastclipboard(this,xa_clipboard);
		}
	 ret= seldata->pastclipboard(this,xa_clipboard);
	return ret;
	}
int copyclipboard(void) {
	int ret;
	if((ret=copyselect())<0)
		return ret;
	if(!withinxterm)
		return ret;
	if(!seldataok()) {
		message("copy restricted to this bed");
		return -1;
		}
	pthread_mutex_lock(&selmutex);
	if(copyshown) {
		char *tmp= copyshown;
		copyshown=NULL;
		free(tmp);
		}
	seldata->sayselection(CurrentTime,xa_clipboard);
	pthread_mutex_unlock(&selmutex);
	return 0;
	}
int makeselection(void) {
	int ret=setselect();
	if(!withinxterm) {
		return ret;
		}
	Editor *ed= getselectioneditor();
	if(!ed) {
		message("select failed");
		return 0;
		}
	ret= ed->becomeowner();
	return ret;
	}
int exportselection(void) {
	if(!withinxterm) {
		message("Only with connection with xserver");
		return -1;
			}
	Editor *ed=getselectioneditor();
	if(!ed) {
		message("No selection");
		return 0;
		}
	return ed->becomeowner();
	}
int becomeowner(void) {
	if(!seldataok()) {
			message("selection restricted to this bed");
		return -1;
		}
	if(getselect()==OFFTYPEINVALID) {
		pthread_mutex_lock(&selmutex);
		selectionoff() ;
		pthread_mutex_unlock(&selmutex);
		return 0;
		}
	pthread_mutex_lock(&selmutex);
	seldata->sayselection(CurrentTime,XA_PRIMARY) ;
	XFlush(selectdisplay);
	pthread_mutex_unlock(&selmutex);
return 0;
}

int pastspecial(void) {
	if(!withinxterm) {
		message("Only pastspecial via xwindows");
		return -1;
		}
	if(!seldataok()) {
		if(seldata&&seldata->iserror())
			message("System error toclipboard %s",seldata->iserror());
		else
			message("System error: startsel failed");
		return -1;
		}
	int ret= seldata->pastspecial(this,xa_clipboard) ;
	return ret;
	}
/*
int ashell(void) {
	endwin();
	system("/bin/sh");
	refresh();
	}
*/
};



extern int standardascii(ScreenPart *x) ;

void Program::getselectionnotify(Atom selection,Atom target,XEvent *xev) {
	pastget.start();
DEBUG("After pastget.start();\n");
	pthread_mutex_lock(&selmutex);
DEBUG("After mutex_lock;\n");
	XConvertSelection(selectdisplay,selection, target,data_atom,selectwin,CurrentTime);
	XFlush(selectdisplay);
	pthread_mutex_unlock(&selmutex);
DEBUG("After mutex_unlock;\n");
	pastget.getotherxev(xev); 
DEBUG("After getotherxev\n");
	}	
int Program::pastspecial(Editor *ed,Atom selection) {
	XEvent xev;
	getselectionnotify(selection,xa_targets,&xev);
	if(xev.type==SelectionNotify) {
		int targetsize;
		CARDBITS *targets;
		    targetsize=getselection(&xev.xselection,(unsigned char **)&targets); 
		    if(targetsize>0) {
			    int has=0;
			    for(int i=0;i<targetsize;i++)  {
				    if(targets[i]==xa_raw) {
					    has|=1;
					}
				    else {
					    if(targets[i]==xa_datatype) {
						    has|=2;
					       }
					    else
					    if(targets[i]==XA_STRING) {
						    has|=4;
					       }
				    }
				}
			    XFree(targets);
			    if(has!=7) {
				    ed->message("No past special");
				    return -1;
				}
			    static int res=0;
			    static int take=1;
			    Atom forms[]={XA_STRING,xa_datatype};
			     int wlin=8;  
			       int wcol=22;
			     while(1) {
				     int sopt=3;
			     begindialog(wlin,wcol,20);
			    mvwaddstr(takescr,1,2,"Past special");
			     beginoptions(take);
				opt("~Shown",sopt+0,3,0);
				opt("~Datatype",sopt+1,3,1);
				 oke(sopt+3,3);
				 act("~Cancel",sopt+3,12);
			     endoptions(take);
			     enddialog(res,res);
			ed->filesup();
			switch(res) {
				case (INT_MAX-1): {
					res=0;
					ed->rewrite();
					continue;
					};
				case 4:;
				case 0:
					return 0;
				};
			if(res>0&&res<3)
				take=res-1;
			res--;
			if(take>=0&&take<2)
				return  pastclip(ed,forms[take], selection);
			    }
			}
		    else {
			    if(targetsize==0)
				    XFree(targets);
			    ed->message("No targets");return -1;
		    	}
		}
	else {
		ed->message("Time out waiting for targets");
		return -1;
		}

	}	    

int Program::pastclip(Editor *ed,Atom type,Atom selection) {
	int fromotherlen=-1;
	unsigned char *fromotherdata=NULL;
	XEvent xev;
	getselectionnotify(selection,type,&xev) ;
	if(xev.type==SelectionNotify) {
		if(xev.xselection.property!=None) {
		     fromotherlen=getselection(&xev.xselection,&fromotherdata); 
			if(fromotherlen>=0) {
				if(fromotherlen) {
					if(type==XA_STRING) {
					ScreenPart *part=ed->activemode();
					if(!standardascii(part)) {
						if(ed->insertshown((char *)fromotherdata,fromotherlen)<=0) {
							ed->message("Insert failed");
							return -1;	
							}
						}
					else {
						goto dopastclipraw;
						}
					}
					else
					if(type==xa_raw) {
					dopastclipraw:
						ed->putbuf((char *)fromotherdata,fromotherlen);
								/*
				int bytes=part->getbytes();
				OFFTYPE under=((ed->geteditpos()/bytes)*bytes)+ed->getfilepos();;
				 ed->putmem(under,(char *)fromotherdata,fromotherlen);
				ed->topos(under+fromotherlen-1);
				*/
					 	}
					else if(type==xa_datatype) {
						int addfield=0;
 switch(ed->getkeyinput("(a)ad field/(s)eparate?")) {
	                 case 'a': addfield=1;break;
		        case 's':
		   default: addfield=0;break;
				    };
			 ed->loaddatatype((char *)fromotherdata,fromotherlen,addfield);
						}
				}
				DEBUG("found\n");
				fromotherlen=-1;
				XFree(fromotherdata);
				return 0;
				}
			else {
				DEBUG("not found\n");
				ed->message("Didn't get selection quick enough");
			   }
			}
		else
				DEBUG("None property\n");
		}
	else
		ed->message("timeout");
	return -1;
	}
int withinpastclipboard(Editor *ed,Atom selection) {
	if(selection==xa_clipboard)
		return ed->pastselect();
	else {
		if(selection==XA_PRIMARY) {
			Editor *seled=getselectioneditor();
			if(!seled)
				return -1;
			OFFTYPE len,start;
			len=seled->getselregion(&start);
			if(!len)
				return -1;
			char *rawselection=(char *)malloc(len+1);
			if(rawselection) {
				seled->getmem(start,rawselection, len);
				rawselection[len]='\0';
				ed->putbuf(rawselection, len);
				free(rawselection);
				return 0;
				}
			else
				return -1;
			}
		}
	return 1;
	}
int Program::pastclipboard(Editor *ed,Atom selection) {
	if(!withinxterm)
		return withinpastclipboard(ed, selection);
	pthread_mutex_lock(&selmutex);
	Window owner= XGetSelectionOwner(selectdisplay, selection);
	XFlush(selectdisplay);
	pthread_mutex_unlock(&selmutex);
	if(owner==None) {
		return withinpastclipboard(ed, selection);
		}
if(seldata&&owner==seldata->selectwin) {
	int ret;
	if((ret=withinpastclipboard(ed, selection)<=0))
			return ret;
	}
	int ret=-1;
Atom trytypes[]={xa_raw,XA_STRING};
for(int typenr=0;typenr<(int)(sizeof(trytypes)/sizeof(trytypes[0]));typenr++) {
	if((ret=pastclip(ed,trytypes[typenr],selection))>=0)
		return ret;
	}
return ret;
}

typedef enum  {NONE,COPY,SELECT,BOTH} Selecttype;
//OFFTYPE maxselectlen=10000000;
//OFFTYPE maxformatlen=10000000;
extern long long getfree(void);

OFFTYPE getmaxselectlen(void) {
	return getfree()/2; /*Yes what?*/
	}
OFFTYPE getmaxformatlen(void) {
	return getfree();
	}
Selecttype getselecttype(void) {
	return COPY;
	}
extern ScreenPart *getselectdatatype(void);
extern int datatype2str(char *gegs,int maxgegs,ScreenPart *sub) ;
const unsigned int klipsize=80;



int MyChangeProperty(Display *display,Window win, Atom property,Atom type ,int format ,const unsigned char *data,int nr) {
	auto maxtrans=XMaxRequestSize(display);  //*4 gives problems
	int trans,start=0,left=nr;
	DEBUG("maxtrans=%ld\n",maxtrans);
	XDeleteProperty(display,win,property);
	while(	left>0) {
		trans=minnum(maxtrans,left);
		XChangeProperty(display,win, property,type , format ,PropModeAppend,data+start,trans); 
		DEBUG("MyChange: start=%d, nr=%d\n",start,nr);
		left-=trans;
		start+=trans;
		}
	return nr;
	}

int  Program::giveselection(XSelectionRequestEvent *req) {
//	unsigned waslong maxsel=0xFFFFFFF;
	OFFTYPE maxsel=0xFFFFFFF;
	XEvent newev;
   newev.type=SelectionNotify;
    newev.xselection.type = SelectionNotify;
    newev.xselection.property = None;
    newev.xselection.display = req->display;
    newev.xselection.requestor = req->requestor;
    newev.xselection.selection = req->selection;
    newev.xselection.target = req->target;
    newev.xselection.time = req->time;
    {
      XClassHint classhint;
      if(XGetClassHint(selectdisplay, req->requestor, &classhint)) {
	      DEBUG("class=%s,name=%s\n",classhint.res_class, classhint.res_name);

			      
//			      if(!strncmp("internal clipboard owner",classhint.res_name,24)) 
	      if(classhint.res_class&&!strncasecmp("klipper",classhint.res_class,7)) 
	      {
		      DEBUG("this klipper\n");
			maxsel=minnum(maxsel,klipsize);
		    //   XDestroyWindow(selectdisplay,req->requestor);
		//       return -1;
			     }
		      else
			 DEBUG("not klipper\n");
		      }

      	}
static unsigned int prevsize=klipsize+1;
stopmainthread;
if(maxsel>klipsize&&prevsize==klipsize) {
		shownselectionpos=-1;
		}
prevsize=maxsel;
	Editor *ed=getselectioneditor();
	OFFTYPE pos,other;
	char *selbuf;
	int lensel=0;
	Selecttype type=getselecttype();	
	if(req->selection==xa_clipboard)
		type=COPY;
	else
		type=SELECT;
	if(type==NONE) {
		 XSendEvent(selectdisplay,req->requestor,False,0,&newev);
		XSetSelectionOwner(selectdisplay, req->selection, None, CurrentTime);
		XFlush(selectdisplay);
		 contmainthread;
		return -1;
		}

	if(ed&&((other=ed->getselect())!=OFFTYPEINVALID)&& (type==SELECT||type==BOTH)) {
		maxsel=minnum(maxsel,getmaxselectlen());
			
			type=SELECT;
			pos=(ed->getfilepos()+ed->geteditpos());
			}
	else {
		if((type==COPY||type==BOTH)&&(lensel=getselbuf(&selbuf))>=0) {
			type=COPY;
			}
		else {
		 XSendEvent(selectdisplay,req->requestor,False,0,&newev);
		   XSetSelectionOwner(selectdisplay, req->selection, None, CurrentTime);
		XFlush(selectdisplay);
		   contmainthread;
		   return -1;
		   }
	   }
	
	{
    char           *cl[2], dummy[1];

//    Atom            target;
 //   XICCEncodingStyle style;
 Atom  target = XA_STRING;
	XICCEncodingStyle    style = XStringStyle;
    XTextProperty   ct;

   if(req->target==xa_targets) {

const	CARDBITS targets[]= {
	xa_raw ZEROTARGETS,
	 xa_datatype ZEROTARGETS,
	 XA_STRING ZEROTARGETS,
	 xa_text ZEROTARGETS,
	 xa_compound_text ZEROTARGETS,
	 xa_targets ZEROTARGETS};

//       XChangeProperty(selectdisplay, req->requestor, req->property, XA_ATOM, (8 * sizeof(targets[0])), PropModeReplace, (unsigned char *)targets, (sizeof(targets) / sizeof(targets[0])));
       XChangeProperty(selectdisplay, req->requestor, req->property, XA_ATOM, 32, PropModeReplace, (unsigned char *)targets, sizeof(targets)/4 );
	newev.xselection.property = req->property;
   }
   else if (req->target ==xa_timestamp) {
	XChangeProperty(selectdisplay, req->requestor, req->property, XA_INTEGER,
			sizeof(Time), PropModeReplace, (unsigned char *)&selection_time, 1);

	newev.xselection.property = req->property;
    } 
else if(req->target==XA_STRING||req->target == xa_compound_text|| req->target == xa_text){
	ScreenPart *sdata=(type==SELECT)? ed->activemode(): getselectdatatype();
	if(standardascii(sdata)) {
		goto dogiveselraw;
		}
if (req->target == xa_compound_text|| req->target == xa_text) {
	    target = xa_compound_text;
	    style = (req->target == xa_compound_text) ? XCompoundTextStyle
		: XStdICCTextStyle;
	}

    if(type==SELECT) {
	    if( shownselectioneditor==ed&& shownselectionpos==pos&& shownselectionother==other) {
	    cl[0]=shownselection;
    		}
	   else {
		ScreenPart *sdata=ed->activemode();
		if(shownselection) {
			char *tmp= shownselection;
			shownselection=NULL;
			free(tmp);
			}
		if(rawselection)
			freeglobal(rawselection);
		OFFTYPE start,len;
		len=ed->getselregion(&start);
		if(len>0) {
			rawsize=len=minnum(len,maxsel);
			if((rawselection=(char *)malloc(rawsize+1))) {
				ed->getmem(start,rawselection, len);
				rawselection[rawsize]='\0';
				shownselectioneditor=ed;
				shownselectionpos=pos;
				shownselectionother=other;
				int itemsize=(sdata->maxres+sdata->apart);
				int needsize=(2+(rawsize/sdata->getbytes()))*itemsize;
				shownselection=cl[0]=(char *)malloc(needsize+itemsize);
				int len=sdata->bin2search((unsigned char *)rawselection,shownselection,  rawsize);
				shownselection[len]='\0';
				DEBUG("len %d\n",len);
				freeglobal(rawselection);
				}
			}
		}
       }
    else {
    	if(!copyshown) {
		ScreenPart *sdata=getselectdatatype();
		int itemsize=(sdata->maxres+sdata->apart);
		int needsize=(lensel/sdata->getbytes())*itemsize+2*itemsize;
		int rawlen=lensel;
		if(needsize>getmaxformatlen()) {
			needsize=getmaxformatlen()-(itemsize*2);
			rawlen=(needsize/itemsize)*sdata->getbytes();
			}
		copyshown=cl[0]=(char *)malloc(needsize+itemsize);
		if(copyshown) {
			int len=sdata->bin2search((unsigned char *)selbuf,copyshown,  rawlen);
			DEBUG("max: " OFFPRINT "\n", getmaxformatlen());
			DEBUG("Allocated %d((%d/%d)*%d+%d), filled with %d\nrawlen=%d\n",needsize,lensel,sdata->getbytes(),itemsize,itemsize,len,rawlen);
			DEBUG("getbytes()=%d,bytes=%d,datbytes=%d\n",sdata->getbytes(),sdata->bytes,sdata->datbytes());
			copyshown[len]='\0';
			}
		}
	else {
		DEBUG("Using old conversion");
		cl[0]=copyshown;
		}
    	}

	if(!cl[0]) {
	      cl[0] = dummy;
	      *dummy = '\0';
		}
	XmbTextListToTextProperty(selectdisplay, cl, 1, style, &ct);
		DEBUG("XA_STRING: ");
	if(MyChangeProperty(selectdisplay, req->requestor, req->property,
			target, 8, ct.value, (int)ct.nitems)>0)
		newev.xselection.property = req->property;
	else
		XDeleteProperty(selectdisplay,req->requestor,req->property);
    }
else if(req->target==xa_raw) {
dogiveselraw:
    if(type==SELECT) {
//	ScreenPart *sdata=ed->activemode();
	OFFTYPE len,start;
	if((len=ed->getselregion(&start))) {
		if(rawselection)
			freeglobal(rawselection);
		len=minnum(len,maxsel);
		if((rawselection=(char *)malloc(len+1))) {
			ed->getmem(start,rawselection, len);
			rawselection[len]='\0';
			rawsize=len;
			DEBUG("xraw: ");
		       if(MyChangeProperty(selectdisplay,req->requestor, req->property,req->target , 8,  (const unsigned char *)rawselection,len)<0) {
			       XDeleteProperty(selectdisplay, req->requestor, req->property);
				}
		       else
				newev.xselection.property = req->property;
		       char *tmpfree=rawselection;
		       rawselection=NULL;
		       free(tmpfree);
			}
	}
    }
    else {
	DEBUG("copy: ");
	
       if(MyChangeProperty(selectdisplay,req->requestor, req->property,req->target , 8,  (const unsigned char *)selbuf,lensel)<0) {
		XDeleteProperty(selectdisplay,req->requestor, req->property);
		DEBUG("failed\n");
       		}
       else {
		newev.xselection.property = req->property;
		DEBUG("Success\n");
       	}
    	}
}
else if(req->target==xa_datatype) {
	if(ScreenPart *sdata=(type==SELECT)? ed->activemode(): getselectdatatype()) {
		static char *conv=NULL;
		conv=(char *)realloc(conv,4096*1024);
		datatype2str(conv,4096*1024,sdata); 
		DEBUG("datatype %s\n",conv);
	       XChangeProperty(selectdisplay,req->requestor, req->property,req->target , 8, PropModeReplace, (const unsigned char *)conv,strlen(conv));
	       free(conv);conv=NULL;
		newev.xselection.property = req->property;
		}
	}
	
	   
	 XSendEvent(selectdisplay,req->requestor,False,0,&newev);
	XFlush(selectdisplay);
	 contmainthread;
	 return 0;
	};	

}

sethelp(pastclipboard,"Past clipboard. This is data copied by bed or a Xwindows program.");
sethelp(makeselection,"Turn on or off selection. Selections can be copied and pasted, saved,\nedited etc. Within a xterm the selection is usually shared with other\nX window Programs.\n\nBed itself has no limit on selection size.\nIn case you are running bed under Xwindows while using the selection mechanism\nof plugin putselect.plug (the default) you can get into trouble.\n\nIn certain Linux distributions the X windows system is sabotaged by loading\nin the standard configuration of KDE an antisocial program called klipper.\nKlipper asks periodically for the X windows selections so problems arise\nwhile using large selections.\nbed gets around this problem by only giving the first few bytes of selections\nto programs of class klipper. But maybe there are klipper like programs\naround who don't use class name klipper.");
sethelp(pastselection,"Past Xwindows selection.");
sethelp(copyclipboard,"Copies selection.");
sethelp(pastspecial,"Only available if a bed owns the clipboard.\nShown:\n	With this function you can past data formatted as shown on\n	the screen. So you can for example insert in ascii the data as\n	shown in heximals\nDatatype:\n	Past datatype of copied data.");
sethelp(exportselection,"Make selection the xwindows selection.");
call(pastclipboard)
call(pastselection)
call(copyclipboard)
call(makeselection)
call(pastspecial)
call(exportselection)

//callname("newshell",ashell)
