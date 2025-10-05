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
#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include CURSES_HEADER
#ifdef PDCURSES
#undef MOUSE_MOVED
#endif

#include <windows.h>
/*
Make routines
to enlarge screen
to switch back to back to small size
enlarge at startup
previous size with shell and edit


On first start use or startup geometry is saved.

*/
#define minnum(x,y) (((x)<(y))?(x):(y))
static COORD screensize;
SMALL_RECT      srWindow; 

int saveerrors(char *name,char *format,...) {
	FILE *fp=fopen(name,"a");
	if(!fp) 
		fp=stderr;
       va_list ap;
     va_start (ap, format);
	 vfprintf(fp, format, ap);
	   va_end (ap);
	   if(fp!=stderr)
		 fclose(fp);
	}
#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define fprintf(out,format...) saveerrors("/tmp/errors.x",format);
#else
#define fprintf(out,...) saveerrors("/tmp/errors.x",__VA_ARGS__);
#endif

void resetsize(void) ;
static void ResizeConBufAndWindow( HANDLE hConsole, int xSize, int ySize);
int resizeconsole(int x, int y) ;
extern  HANDLE   hConOut ;
#ifndef TEST
//int hel=resizeconsole(-1,-1) ;
#endif
static HANDLE setscreensize(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;      
  HANDLE hConsole=GetStdHandle( STD_OUTPUT_HANDLE	);
//  HANDLE hConsole=   hConOut ;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
	  screensize=csbi.dwSize;
	  srWindow=csbi.srWindow;
	  atexit(resetsize);
     return hConsole;
	}
//#define INVALID_HANDLE (char *)NULL
//static HANDLE consolehandle=INVALID_HANDLE_VALUE;
static HANDLE consolehandle=setscreensize();

void resetsize(void) {
/*
#ifdef PDCURSES
		resize_term(screensize.Y,screensize.X);
#endif
*/

	if (!SetConsoleWindowInfo(consolehandle, TRUE, &srWindow))
	{
		fprintf(stderr,"SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect) failed\n");
	}
	if (!SetConsoleScreenBufferSize(consolehandle, screensize))
	{
		fprintf(stderr,"SetConsoleScreenBufferSize(hConsole, coordScreen failed\n");
	}
	if (!SetConsoleWindowInfo(consolehandle, TRUE, &srWindow))
	{
		fprintf(stderr,"SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect) failed\n");
	}
}

 int resizeconsole(int x, int y) {
	if(consolehandle==INVALID_HANDLE_VALUE)
		consolehandle=setscreensize();
	ResizeConBufAndWindow(consolehandle, x,  y);
}

static void ResizeConBufAndWindow(
	HANDLE hConsole,
	int xSize,
	int ySize)
{
    SMALL_RECT      srWindowRect;       /* hold the new console size */
    COORD           coordScreen;
DWORD			dwErr = 0;

    /* get the largest size we can size the console window to */
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    if(xSize<0)
	    xSize= coordScreen.X ;
    if(ySize<0)
	  ySize=coordScreen.Y ;

    /* define the new console window size and scroll position */
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    srWindowRect.Right =  (SHORT) (minnum(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (minnum(ySize, coordScreen.Y) - 1);

    /* define the new console buffer size */
	 coordScreen.X = xSize;
    coordScreen.Y = ySize;
	if (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
	{
		fprintf(stderr,"second SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect) failed\n");

	}

	if (!SetConsoleScreenBufferSize(hConsole, coordScreen))
	{
		fprintf(stderr,"SetConsoleScreenBufferSize(hConsole, coordScreen failed\n");
	}
	if (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
	{
		fprintf(stderr,"second SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect) failed\n");

	}

/*
#ifdef PDCURSES
		resize_term(ySize,xSize);
#endif
*/
}
#define INPLUGIN 1
#ifdef INPLUGIN
static int switchsizer(void) {
	static int maxc=0;
	if(maxc) {
			resetsize();
		maxc=0;
		}
	else {
		resizeconsole(-1,-1);
		maxc=1;
	}
	return maxc;
	}

#ifndef TEST2
#ifdef PLUGINSRC
#include "global.h"
BEDPLUGIN

extern int		resizeflag;
class Editor {	
public:
//	int rewrite(void);
	int switchsize(void) {
		if(!isendwin())
			endwin();
		switchsizer();
		refresh();
		resizeflag=1;
		return 0;
	}
};

call(switchsize)
#endif
#endif
#endif
#ifdef TEST
int  main(int argc,char **argv ){
		char ch;
	if(argc>2) {
		  HANDLE han=GetStdHandle( STD_OUTPUT_HANDLE	);
		ResizeConBufAndWindow(han,atoi(argv[1]),atoi(argv[2]));
		system("bed");
		}
	else
		{
		switchsizer();
		fprintf(stderr,"Use: %s x y\nresizes console to x y\n",argv[0]);
		}
return 0;
}
#endif
#else
#ifndef TEST
#include "global.h"
BEDPLUGIN
#endif
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
       #include <sys/types.h>
       #include <signal.h>
  #include <unistd.h>
#include "varmacro.h"

static char themessagestr[100];

#ifdef OLDGCCVARMACRO
#define messagestr(arg...) sprintf(themessagestr,##arg)
#else
#define messagestr(...) sprintf(themessagestr,__VA_ARGS__)
#endif
static	XWindowAttributes win_attrinit;
static int rxinit,ryinit;
static Window win=(Window)0;
static Display *display;
 int switchsize(void)
{
  static	XWindowAttributes win_attr;
// 	static unsigned int display_width, display_height;
 	static  int display_width, display_height;
	static int rx=200,ry=150;
	Window junkwin;
  XWindowAttributes win_attrtmp;
	int tmprx=0,tmpry=0;

	if(!win) {
		int focusrevert;
		int screen_num;
		char *winid;
		if ( (display=XOpenDisplay(NULL)) == NULL )
			{
			messagestr("rxvt.plug: cannot connect to X server %s", XDisplayName(NULL));
			return -1;
			}
		screen_num = DefaultScreen(display);
		display_width = DisplayWidth(display, screen_num);
		display_height = DisplayHeight(display, screen_num)-5;
		 XGetInputFocus (display, &win, &focusrevert);
		if(win==None) {
		       winid=getenv("WINDOWID");
			if(!winid||!(win = (Window)atol(winid))) {
				messagestr("Can't get window id");return -1;
				}
			}
	if(!XGetWindowAttributes(display, win,&win_attrinit)) {
		messagestr("Can't get windows attributes");return -1;
		}
	XTranslateCoordinates (display, win, win_attrinit.root, -win_attrinit.border_width, -win_attrinit.border_width, &rxinit, &ryinit, &junkwin);
		win_attr.width=642;win_attr.height=512 ;

		}
	if(!XGetWindowAttributes(display, win,&win_attrtmp)) {
		messagestr("Can't get windows attributes");return -1;
		}
#define  MAXWIDTH display_width
#define MAXHEIGHT display_height
	if(win_attrtmp.width>=((MAXWIDTH*7)/10)&&win_attrtmp.height>=((MAXHEIGHT*7)/10)) {
		XWarpPointer(display,None,win,0,0,0,0,rx-win_attr.x+(win_attr.width/2),ry-win_attr.y+(win_attr.height/2));
		XMoveResizeWindow(display, win, rx,ry,win_attr.width, win_attr.height ); 
		}
	else {
		XTranslateCoordinates (display, win, win_attrtmp.root, -win_attrtmp.border_width, -win_attrtmp.border_width, &tmprx, &tmpry, &junkwin);
		memcpy(&win_attr,&win_attrtmp,sizeof(win_attr));
		rx=tmprx;
		ry=tmpry;
		XMoveResizeWindow(display, win, 0,-2,MAXWIDTH,MAXHEIGHT);
//		XMoveResizeWindow(display, win, 0,0-6,display_width,display_height);
/*	XRaiseWindow(display,win);
	XFlush(display);
//	XSync(display,False); 
	if(!XGetWindowAttributes(display, win,&win_attrtmp)) {
		messagestr("Can't get windows attributes");return -1;
		}
	display_height=win_attrtmp.height;
	display_width=win_attrtmp.width;
	*/
	}

	XRaiseWindow(display,win);
	XFlush(display);
//	XSync(display,False); 
	return 0;
} 
#ifdef RESIZEINIT
void switchback(void) {
	Window junkwin;
	int rx,ry;
	XWarpPointer(display,None,win,0,0,0,0,rxinit-win_attrinit.x+(win_attrinit.width/2),ryinit-win_attrinit.y+(win_attrinit.height/2));
	XMoveResizeWindow(display, win, rxinit,ryinit,win_attrinit.width, win_attrinit.height );
	XRaiseWindow(display,win);
	XFlush(display);
//	pid_t pid=getpid();
//	kill(pid,SIGWINCH);
	}
#endif
#ifndef TEST
class Editor {	
public:
int toscreen(void) ;
int rewrite(void);
int message(const char *format ...) ;
 int switchsize(void) {
	int ret;
	themessagestr[0]=0;
//	messagestr("");
	ret= ::switchsize();
	toscreen();
 	message(themessagestr);
	return ret;
	}
#ifdef RESIZEINIT	
int resizeinit(void) {
	static int iter=0;		
	if(iter)
		return 0;
	if(::switchsize()>=0) {
		atexit(switchback);
		}
	else
		rewrite();
	iter++;
	return 0;
	}

#endif
};
#endif


#ifdef TEST

int main(void) {
	while(switchsize()>=0)
		getchar();
return 0;
	}
#else
sethelp(switchsize, "Switches xterm size to a large size if small or to a small size if large");
call(switchsize)
#ifdef RESIZEINIT	
executeprocedure(resizeinit)
#endif
#endif
#endif
