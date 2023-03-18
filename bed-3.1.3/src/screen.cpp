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
#include <unistd.h>
#undef ERR
#include <signal.h>
#undef ERR
#include "editor.h"
#include "main.h"
#include "cursesinit.h"
#include "screen.h"
extern nocurses_init curses_init_curses;
extern void resizehandler(int signum) ;
extern int makevisible(ScreenPart *part,int pos) ;

void waitforsigwinch(void) {
#ifdef HAVE_SIGSUSPEND
     sigset_t mask, oldmask;
     sigemptyset (&mask);
     sigaddset (&mask, SIGWINCH);
   sigprocmask (SIG_BLOCK, &mask, &oldmask);
    sigsuspend (&oldmask);
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
#endif

	}

extern char showfileinfo;
int Editor::toscreen(void) {
	if(editfocus<startactive||editfocus>=startnonactive) {
			startactive=editfocus;
			startnonactive=editfocus+1;
			}
	if(resizeflag) 
		resized();
	if(redistribute||(fromtop>(startnonactive-startactive-1))) {
		while(rewriteall()<0) {
			beep();
			waitforsigwinch();
			curses_init_curses.resized();
			}
			
		}
	if(shouldreinit)  
		reinit();
	if(dofromfile) {
		fromfile();
		updated=0;
		}

	if(makevisible(parts[mode],editpos) <0) {
		updated=0;
		}
	if((!updated||(selectpos!=OFFTYPEINVALID))) {
		writebuf(); updated=1;
		}
	else {
		if(showfileinfo) {
			datainfo();
			showfileinfo=0;
			}

		}
	showposition() ;
	wrefresh(editscreen);
	return 0;
	}

void Editor::filesup(void) {
	editup();
	}

#if defined(_WIN32)
#if defined(NCURSES_VERSION) 
//#if 1
extern int dosconsole(void) ;
int dosconsole(void) {
	return consoleoutputhandle!=INVALID_HANDLE_VALUE;
	}
HANDLE initcursorinfo(void) {
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	if(handle==INVALID_HANDLE_VALUE ) {
//		fprintf(stderr,"GetStdHandle(STD_OUTPUT_HANDLE) failed %ld\n", GetLastError());

				return INVALID_HANDLE_VALUE;
			}
	else {
			if(!GetConsoleCursorInfo(handle,&startupcursor)) {
//				fprintf(stderr," GetConsoleCursorInfo failed %ld\n", GetLastError());
				return INVALID_HANDLE_VALUE;
				}

			}
//		atexit(showcursor);
		invisiblecursor.dwSize=startupcursor.dwSize;	
		invisiblecursor.bVisible=FALSE;	
			return handle;	
   	}
CONSOLE_CURSOR_INFO  startupcursor,invisiblecursor; 	
/*WAS*/
HANDLE consoleoutputhandle=initcursorinfo();
#else
  #if defined(PDC_BUILD)&&PDC_BUILD<2600 

  #else
HANDLE   consoleoutputhandle = GetStdHandle(STD_OUTPUT_HANDLE);
  #endif
#endif
#endif
