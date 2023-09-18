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
#include "config.h"
#include <features.h>
#include <stdlib.h>

#ifdef HAVE_SYS_IOCTL_H 
#include <sys/ioctl.h>
#endif
#include <stdio.h>
//#include <sys/termios.h>
#include "system.h"
#ifdef _WIN32

#include <windows.h>
#endif

#include "cursesinit.h"
extern int redistribute;
extern void initcolor(void) ;

int after_initscr=0;
#if defined(USE_NEWTERM) || defined(GETOTHERTERM)
extern SCREEN *thescreen;
#endif

extern int		resetcolors(void) ;
extern int upmenus(void) ;
extern void delmenus(void) ;
extern void getotherterm(void ) ;
//NEWTERM
#if defined(PDCURSES)&& !defined(USE_NEWTERM)
#include <stdlib.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h>

#ifdef USE_NBEDPATCH
#include "shell.h"
#endif
#include "win.h"
extern int margc;
extern char **margv;
static int beforeinitscr=0;
static void exitinitscr(void) {
	static int didexit=0;
	if(!didexit&&beforeinitscr) {
		didexit=1;
#ifdef __CYGWIN__
		char NBED[BEDSMAXPATH];
#else
	 char *NBED;
#endif
#ifdef SEARCHPATHNAME
,*thisprog
#endif
;
		if(char *bedenv=getenv(NCURSES_BEDENV)) {
#ifdef __CYGWIN__
			absunixpath(bedenv,NBED) ;
#else
			NBED=bedenv;
#endif
			}
		else {
			NBED= const_cast<char *>(NCURSES_BED);
	//		strcpy(NBED,NCURSES_BED);
			}
		fprintf(stderr,"%s failed, execute %s ...\n",margv[0],NBED);

#ifdef SEARCHPATHNAME
		thisprog=margv[0];
#endif
		margv[0]= NBED; /*ENV+conf*/
		if(execvp(margv[0], margv)==-1) {
#ifndef USE_NBEDPATCH
			perror(NBED);
#else
			if(errno==ENOENT) {
				char fullname[MAX_PATH];
#ifndef SEARCHPATHNAME
	if( GetModuleFileName(
			NULL,	// handle of module to find filename for 
			fullname,	// address of buffer for module path 
			MAX_PATH 	// size of buffer, in characters 
		 )>0) {
#else
				char *filcmp;
	 if(SearchPath(

				NULL,	// address of search path 
				thisprog,	// address of filename 
				".exe",	// address of extension 
				MAX_PATH,	// size, in characters, of buffer 
				fullname,	// address of buffer for found filename 
			&filcmp 	// address of pointer to file component 
		 )>0)	 {
#endif
	if(access(NCURSES_BED,0)) {
			char binpatch[]= CONFDIRWIN"\\binpatch.exe";
			char bedpatch[]= CONFDIRWIN"\\bedpatch";
/*
			char command[MAX_PATH];
			sprintf(command,"\"%s\" \"%s\" \"%s\" \"%s\" -a",binpatch,fullname,NCURSES_BED,bedpatch);
			fprintf(stderr,"create %s:\n%s\n",NCURSES_BED, command);
			system(command);
*/
			if(!process(binpatch,fullname,NCURSES_BED,bedpatch,"-a")) {
				margv[0]=NCURSES_BED;
				execvp(margv[0], margv);
				perror("error executing bedn.exe"); 
				}
		}
	else {
	  fprintf(stderr,"%s does exists, but execvp failed\n",NCURSES_BED);
		}

				}
	else {

#ifdef SEARCHPATHNAME
		fprintf(stderr,"Can't execute %s, can't find %s to create %s\n",margv[0],thisprog,NCURSES_BED);
#else
		fprintf(stderr,"Can't execute %s, can't find program name",margv[0]);
#endif
		}
			}
		else {
			perror(NBED);
			}
#endif
		}
        }
	}
#endif

#if defined(_WIN32)
extern CONSOLE_SCREEN_BUFFER_INFO original_screenbuffer;
CONSOLE_SCREEN_BUFFER_INFO original_screenbuffer;
#endif

	void nocurses_init::curses_init(void) { /*Initialize screen */

#if defined(_WIN32)
	#ifdef NCURSES_VERSION
	if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
	#endif
  GetConsoleScreenBufferInfo(consoleoutputhandle, &original_screenbuffer);
#endif

#ifdef USE_NEWTERM
		thescreen=newterm(NULL, stdout,stdin);
		if(!thescreen) {
			char *term=getenv("TERM");
			fprintf(stderr,"newterm/3 failed, ");
			if(!term) 
				fprintf(stderr,"give environmental variable TERM a value and try again\n");
			else
				fprintf(stderr,"is TERM=%s alright?\n",term);
			exit(1	);
			}
		set_term(thescreen);
#else
#ifdef PDCURSES
beforeinitscr=1;
    atexit(&exitinitscr);
#endif

//fprintf(stderr,"%p%s%p\n",consoleoutputhandle,(consoleoutputhandle==INVALID_HANDLE_VALUE?"=":"!="), INVALID_HANDLE_VALUE);
		if(initscr()==NULL) {
#ifdef PDCURSES
			beforeinitscr=0;
			exitinitscr();
#endif
			}
after_initscr=1;

#ifdef PDCURSES
beforeinitscr=0;
#endif
#endif
#ifdef _WIN32
#ifdef NCURSES_VERSION
if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
{
	  COORD scrbuf;
	scrbuf.X=COLS;scrbuf.Y=LINES;
	SetConsoleScreenBufferSize(consoleoutputhandle,scrbuf);
	}
#endif
#endif
		thelines=LINES;
		thecols=COLS;

			
		setscreen();
		clearok(stdscr,TRUE);
		redistribute=1;
		OldMainScreenwidth=MainScreenwidth;
		HIDECURSOR;
		refresh();
		};
	void  nocurses_init::reset(void ) {
		resized();
		}
 
#ifdef DEBUGON
extern int errormessage(char *format ...) ;
#else

#ifndef OLDGCCVARMACRO
#define errormessage(commando,...)  
#else
#define errormessage(commando,args...)   
#endif
#endif
	int  nocurses_init::isresized(void) { /*Reset screen for program */

#if (! defined(PDCURSES)) && defined(TIOCGWINSZ) 
		struct winsize win;
	   if (
#if defined(NCURSES_VERSION) && defined(_WIN23)
consoleoutputhandle!=INVALID_HANDLE_VALUE ||
#endif
#ifdef GETOTHERTERM
!thescreen||
#endif //GETOTHERTERM
(ioctl(0, TIOCGWINSZ, &win) == 0 && (thelines != win.ws_row || thecols != win.ws_col))) {
return 1; /*resized*/
			}
return 0; /*not resized*/
#else // TIOCGWINSZ
#ifdef PDCURSES

CONSOLE_SCREEN_BUFFER_INFO csbi;      
 GetConsoleScreenBufferInfo(consoleoutputhandle, &csbi);
int scrHigh= csbi.srWindow.Bottom-csbi.srWindow.Top +1;
int scrWidth=csbi.srWindow.Right- csbi.srWindow.Left+1;
//fprintf(stderr,"width=%d, height=%d\n",scrWidth,scrHigh);
if(csbi.srWindow.Top!=0||csbi.srWindow.Left!=0||scrHigh!=thelines||scrWidth!=thecols) {
		return 1; /*resized*/
		}
return 0; /*not resized*/
#endif //PDCURSES
#endif // TIOCGWINSZ
return 1; /*resized*/
		};

#ifdef PDCURSES
#define PDNOCHANGESCREEN 1
#endif


#ifdef PDCURSES
	int  nocurses_init::setsize(void) { 
CONSOLE_SCREEN_BUFFER_INFO csbi;
 GetConsoleScreenBufferInfo(consoleoutputhandle, &csbi);
int scrHigh= csbi.srWindow.Bottom-csbi.srWindow.Top +1;
int scrWidth=csbi.srWindow.Right- csbi.srWindow.Left+1;
//fprintf(stderr,"width=%d, height=%d\n",scrWidth,scrHigh);
return setsize( scrHigh,scrWidth);
	}
	int  nocurses_init::setsize(int scrHigh,int scrWidth) { 
//        resize_term(scrHigh,scrWidth);
        resize_term(0,0);
#else
	int  nocurses_init::setsize(void) { 
#endif


			INCDEST();
			delmenus();
#ifndef  NOENDWIN
if(!isendwin())
			endwin();
#endif
#ifdef NEWTERMRESIZE
#ifdef HASDELSCREEN
			if(thescreen)
				delscreen(thescreen);
#endif // HASDELSCREEN
			thescreen=newterm(NULL, stdout,stdin);
			if(!thescreen) {
				NODEST();
				errormessage("resized\n") ;
				getotherterm();
				return 0;
				}				
			thelines=LINES;
			thecols=COLS;

			resetcolors() ;
			setscreen();
			clearok(stdscr,TRUE);
	//		refresh();
#else //NEWTERMRESIZE
#ifndef PDCURSES
			refresh(); /*back from endwin*/
#endif
			flushinp();
			thelines=LINES;
			thecols=COLS;
#ifndef PDCURSES
//DEBUGGING("Resized: thelines=%d,thecols=%d win_ws_row=%d, win.ws_col=%d LINES=%d, COLS=%d\n", thelines,thecols,win.ws_row,win.ws_col,LINES,COLS );
#endif
			redistribute=1;
			HIDECURSOR;
			OldMainScreenwidth=MainScreenwidth;
			if(upmenus()<0) {
				DECDEST();
				return -1;
				}
#if defined(NCURSES_VERSION) && defined(_WIN32)


if(consoleoutputhandle!=INVALID_HANDLE_VALUE) {
{
  COORD scrbuf;
	scrbuf.X=COLS;scrbuf.Y=LINES;
	SetConsoleScreenBufferSize(consoleoutputhandle,scrbuf);
//		resizeterm(scrHigh,scrWidth);
	}
	}
#endif
			refresh();
			DECDEST();
			return 0;
#endif //NEWTERMRESIZE
}
	int  nocurses_init::resized(void) { /*Reset screen for program */


#if (! defined(PDCURSES)) && defined(TIOCGWINSZ) 
		struct winsize win;
	   if (
#if defined(_WIN32)
#ifdef NCURSES_VERSION
consoleoutputhandle!=INVALID_HANDLE_VALUE ||
#endif
#endif
#ifdef GETOTHERTERM
!thescreen||
#endif //GETOTHERTERM
(ioctl(0, TIOCGWINSZ, &win) == 0 && (thelines != win.ws_row || thecols != win.ws_col))) {
#else // TIOCGWINSZ
#ifdef PDCURSES

CONSOLE_SCREEN_BUFFER_INFO csbi;
 GetConsoleScreenBufferInfo(consoleoutputhandle, &csbi);
int scrHigh= csbi.srWindow.Bottom-csbi.srWindow.Top +1;
int scrWidth=csbi.srWindow.Right- csbi.srWindow.Left+1;
//fprintf(stderr,"width=%d, height=%d\n",scrWidth,scrHigh);

#ifdef PDNOCHANGESCREEN
if(csbi.srWindow.Top!=0||csbi.srWindow.Left!=0||scrHigh!=thelines||scrWidth!=thecols) {
#endif
return setsize(scrHigh,scrWidth);
#endif //PDCURSES
#endif // TIOCGWINSZ
#ifndef PDCURSES
return setsize();
#endif
#if  (defined(PDNOCHANGESCREEN)&& defined(PDCURSES)) || defined(TIOCGWINSZ)
			}

		else
		{
			setscreen();
			clearok(stdscr,TRUE);
			redistribute=1;
			thelines=LINES;
			thecols=COLS;
			HIDECURSOR;
			OldMainScreenwidth=MainScreenwidth;
			refresh();
			return 0;
			}
#endif // defined(PDCURSES)||defined(TIOCGWINSZ)

		};
	void  nocurses_init::restore(void) { /*restore screen from before start program */
	if(!isendwin()) {
		SHOWCURSOR;
		move(thelines-1,0);
		clrtoeol();
		touchline(stdscr,thelines-1,1);
#ifndef NOENDWIN
		endwin();
#endif
		}
		};
	void  nocurses_init::setscreen(void) { /*Set screen for program */
		noecho();
	   	cbreak();
	   	nonl();
#ifdef USE_KEYPAD 
	keypad(stdscr,TRUE);
meta(stdscr,TRUE);
/*#ifdef PDCURSES
PDC_save_key_modifiers(FALSE);
PDC_return_key_modifiers(FALSE);
#endif
*/
#endif
	   	};





