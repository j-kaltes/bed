/*     Bed a Binary EDitor for Linux and for Rxvt running under Linux.       */
/*     Copyright (C) 1998  Jaap Korthals Altes <binaryeditor@gmx.com>      */
/*                                                                           */
/*     Bed is free software; you can redistribute it and/or modify           */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     Bed is distributed in the hope that it will be useful,                */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with bed; if not, write to the Free Software                    */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/*                                                                           */
/* Sun Dec  6 18:34:27 1998                                                  */
#ifndef SCREEN_H
#define SCREEN_H
#include <features.h>
#include "config.h"

extern int thelines,thecols;
extern int OldMainScreenwidth;




#ifdef CONIO
#define UITGEBREID
#include <stdio.h>
#include "conio.h"

extern struct text_info texi;
#define Screenheight (texi.screenheight)
#define Screenwidth (texi.screenwidth)
#define move(y,x)	gotoxy((x)+1,(y)+1) 
#define printw	cprintf
#define addch	putch
#define refresh()
#define	clear() clrscr()
#define	clrtoeol() clreol()


#define NORMAALKLEUR 0

#define standout()	textcolor(LIGHTBLUE)
#define	standend()	textattr(NORMAALKLEUR) 

#define leaveok(x,y)
#define		echoon()
#define	noecho()
#define		mvaddstr(y,x,str) {gotoxy(x+1,y+1); cputs(str);}

#ifdef UITGEBREID
#define		addnstr(str,n) {cwrite(str,(n));}
#define		mvaddnstr(y,x,str,n) {gotoxy(x+1,y+1); cwrite(str,(n));}
#else
#define		addnstr(str,n) {cputs(str);}
#define		mvaddnstr(y,x,str,n) {char vooraddn=(str)[(n)];(str)[(n)]='\0';gotoxy((x)+1,(y)+1);cputs(str);(str)[(n)]=vooraddn;}
#endif

#define getstr(x)	cgets(x)
#define addstr(naam) cputs(naam)
#ifdef OLDGCCVARMACRO
#define		mvprintw(y,x,arg...)   {gotoxy(x+1,y+1); cprintf(##arg);}
#else
#define		mvprintw(y,x,...)   {gotoxy(x+1,y+1); cprintf(__VA_ARGS__);}
#endif
#define	cbreak()
#define	scrollok(x,y)
//#define	keypad(stdscr,TRUE);
#define scroll(stdscr) {gotoxy(1,1);delline();}
#define scrolldown() {gotoxy(1,1);insline();}
#define beep() (fputc(7,stdout), fflush(stdout))
#define flushinp() fflush(stdin)
#else
#ifdef CURSES 

#ifndef NOCURSES
#include CURSES_HEADER
#endif
extern int mywgetch(WINDOW *win)  ;

#if  defined(_WIN32)
#include <windows.h>
#undef MOUSE_MOVED
 #if defined(PDC_BUILD)&&PDC_BUILD<2600 
    #define consoleoutputhandle hConOut;
 #endif
extern HANDLE consoleoutputhandle;

extern CONSOLE_SCREEN_BUFFER_INFO original_screenbuffer;
#endif

#if defined(NCURSES_VERSION) && defined(_WIN32)
//#if  defined(_WIN32)

extern CONSOLE_CURSOR_INFO  startupcursor,invisiblecursor; 	
inline void hidecursor(void) 
{curs_set(0); 
if(consoleoutputhandle!=INVALID_HANDLE_VALUE)
			SetConsoleCursorInfo(consoleoutputhandle,&invisiblecursor);}
inline void showcursor(void) 
{curs_set(1); 
if(consoleoutputhandle!=INVALID_HANDLE_VALUE)
	SetConsoleCursorInfo(consoleoutputhandle,&startupcursor);
}
#define HIDECURSOR hidecursor()
#define SHOWCURSOR  showcursor()
#else
#ifdef HAS_CURS_SET
#define HIDECURSOR curs_set(0)
#define SHOWCURSOR curs_set(1)
#else
#define HIDECURSOR move(LINES,COLS)
#define SHOWCURSOR move(0,0)
#endif
#endif
/*
#ifdef _WIN32
extern int mybeep(void);
#define beep() mybeep()
#endif
*/
//s/#define[ 	]*\([a-zA-Z]*\).*/#undef \1

extern void	attrall(WINDOW *takescr,int attrs) ;
#else
#ifdef SLCURSES
#include <slcurses.h>
#define getstr(x)   {int ch; while((ch=getch())&&(ch!='\n')&&ch!=13&&ch!=EOF) { addch(ch); *x++=ch; }; }
#define halfdelay(x)
#define CURSES
#endif
#endif
#ifdef CURSES
//#define ndelay halfdelay(1)
//#define ndelay {cbreak();halfdelay(5);}

extern int ncursesdelay;
#define ndelay halfdelay(ncursesdelay)
//#define ndelay {cbreak();timeout(100);}
//#define ndelay raw()
//#define ndelay cbreak()
#define echoon() echo()
#define MainScreenheight thelines
#define MainScreenwidth thecols
#define gppconio_init()
#define gettextinfo(x)
//inline clearline() {for(int i=0;i<Screenwidth;i++) addch(' ');}
static inline int clearline(void) {return clrtoeol();}
//#define clearline() 
#endif
#endif

#endif
