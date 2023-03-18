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
#ifndef MAIN_H
#define MAIN_H
#include <features.h>
#include "screen.h"
extern int maxedit,nredit, editfocus,startnonactive;
extern int	startactive;
extern int waitinput;
extern int resizeflag;
class Editor;
extern Editor **edits;
inline int typevolg(int x) {return ((x>10)?1:(x>8?2:(x>2?3:4)));}
inline int maxof(int een,int twee) {return ((een>twee)?een:twee);}
extern int ggetch(WINDOW *w) ;
extern const char *getfile(const char *prompt,const char *ask) ;
extern int resized(void);
extern void putmenus(void) ;
extern void resizemenus(void) ;
extern void resizehandler(int signum) ;
extern void setactives(void) ;
extern Editor *argfile(void) ;
extern Editor *newfile(const char *file ); 
extern Editor *emptyfile(void) ;
extern Editor *editfile(const char *file) ;
int exitall(void) ;
int endedit(Editor *ed) ;
int addlast(void) ;
extern Editor *newfile(const char *file ) ;
extern pid_t pid;
#include <setjmp.h>

#ifdef HAS_SIGSETJMP 
#define mysigsetjmp(env,sigs) sigsetjmp(env,sigs)
extern sigjmp_buf screenjmp;
extern sigjmp_buf resizejmp;
#else
#define mysigsetjmp(env,sigs) setjmp(env)
extern jmp_buf screenjmp;
extern jmp_buf resizejmp;
#define		siglongjmp(resizejmp,signum) longjmp(resizejmp,signum)
#endif
extern int redistribute;
extern int menuon;
#define newscherm redistribute
#define RESIZE 12
extern int rewriteall(void) ;
extern int startbar;

extern int withinxterm;

int showstatestate(const char *str,short len,short pos=-1) ;
#endif
