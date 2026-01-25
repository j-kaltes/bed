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
#ifndef GLOBAL_H
/*#if defined(PLUGIN)||defined(FAKE_DYNAMIC)
#define iscalled(proc) int proc(void);
#else
#define iscalled(proc) 
#endif
*/
#define iscalled(proc) int proc(void);
#define  GLOBAL_H
class Editor;
#include "offtypes.h"
#include "filter.h"
extern int withinxterm;
extern void rewriteall(void) ;
extern int resized(); 
extern Editor *editfile(const char *file) ;
extern Editor *newfile(void) ; /*opens empty file */
extern Editor *getactive(void);
extern void stopmain(void) ;
extern void contmain(void) ;

extern int makehelp(char *text,int len,int  (Editor::*proc)(void)) ;
#define sethelp(proc,text) static char proc##help[]=text "\n\nplugin " __FILE__;int helphelp##proc=makehelp(proc##help,sizeof(proc##help),&Editor::proc);
extern const char *getfile(const char *prompt,const char *ask); 
extern  Editor *editin(Editor *ed);
extern int dynlink(const char *name) ;
#define clcallproc(cl,pr) (cl->*(pr))()
#define callproc(pr) clcallproc(this,pr)
extern int (Editor::*nameproc(char *str))(void) ;
extern int addprocedure(int (Editor::*proc)(void)); 
extern int call_open(int (Editor::*proc)(void)) ;
extern int call_switch(int (Editor::*proc)(void)) ;
extern int call_once(int (Editor::*proc)(void)) ;
extern int str_call_open(char *name) ;
extern int str_call_once(char *name) ;
typedef int DataType;
typedef  DataType Type;
class ScreenPart;
extern int getindatatype(ScreenPart *thesub) ;
int addsubbin(ScreenPart *super,ScreenPart *sub,unsigned char *bin,unsigned char *subbin,int len) ;

#undef BEDPLUGIN
#define BEDPLUGIN 
#ifndef PLUGIN_CPP
#undef initprocedure
#define initprocedure(x) int avar##x=x();
#undef executeprocedure
#define executeprocedure(x) int execute##x=call_open(&Editor::x);
#undef call_on_open
#undef call_on_start
#undef call_on_switch
#define call_on_open(x) int call_on_open_##x=call_open(&Editor::x);
#define call_on_switch(x) int call_on_switch_##x=call_switch(&Editor::x);
#define call_on_start(x) int call_on_start_##x=call_once(&Editor::x);
#undef call 
#undef callname
#if defined(PLUGIN) || defined(FAKE_DYNAMIC)
extern int adddynprocedure(const char *name,int (Editor::*proc)(void)) ;
#define call(x)  int call##x=adddynprocedure(#x,&Editor::x) ;
#define callname(name,x)  int call##x=adddynprocedure(name,&Editor::x) ;
#else /*dyn */
#define call(x) int (Editor::*for_procedures_h_##x##_prodef)(void)=&Editor::x;
#define callname(name,proc)  int (Editor::*for_procedures_h_##proc##_prodefname)(void)=&Editor::proc;char name_part_##proc##_end[]=name;
#endif /*dyn */
#ifdef PLUGIN
#undef BEDPLUGIN
#define BEDPLUGIN char bedplugin[]="bedplugin";
#endif
#endif /*PLUGIN_CPP*/
#endif /*GLOBAL_H*/
