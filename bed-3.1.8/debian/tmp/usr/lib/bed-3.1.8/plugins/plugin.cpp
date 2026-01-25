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
#ifndef PLUGIN_CPP
#define PLUGIN_CPP
#undef executeprocedure
#undef initprocedure
#undef call_on_open
#undef call_on_start
#undef call_on_switch
#define call_on_open(x)
#define call_on_switch(x)
#define call_on_start(x)
#define executeprocedure(x)
#define initprocedure(x)
#undef UNDER
#undef EDITOR
#define call(x)
#define callname(name,proc)
#define GLOBAL

class Editor;
#include "global.h"
#ifdef PLUGIN
char bedplugin[]="bedplugin";
#endif
#include SOURCEFILENAME
#undef GLOBAL
class Editor {
public:
#define EDITOR
#include "faked.h"
//#define initprocedure(x) static int (Editor::*initplugin)(void);
#include SOURCEFILENAME
#ifndef PLUGIN
#undef GLOBAL
#undef EDITOR
#undef call
#undef callname
#endif

};
#undef GLOBAL
#undef EDITOR
#undef call
#undef callname
#if defined(PLUGIN)
#ifdef PROCARRAY
#define address(Type,func) &Type::func
#define callname(name,proc) address(Editor,proc),
#define call(x) address(Editor,x),
int (Editor::*Editor::dynproc[])(void) = {
#include SOURCEFILENAME
};
#undef call
#define call(x) #x,
#undef callname
#define callname(name,proc) name,
const char *Editor::dynprocnames[] = {
#include  SOURCEFILENAME
};
#endif //PROCARRAY
#endif //PLUGIN
#undef call
#undef callname

#ifndef PLUGIN
#ifdef FAKE_DYNAMIC
extern int adddynprocedure(const char *name,int (Editor::*proc)(void)) ;
#define call(x)  int call##x=adddynprocedure(#x,&Editor::x) ;
#define callname(name,x)  int call##x=adddynprocedure(name,&Editor::x) ;
#else
/*#define call(x) int (Editor::*x##_to_export_it)(void)=&Editor::x;
#define callname(name,proc)  int (Editor::*proc##_to_export_it)(void)=&Editor::proc;
*/
#define call(x) int (Editor::*for_procedures_h_##x##_prodef)(void)=&Editor::x;
#define callname(name,proc)  int (Editor::*for_procedures_h_##proc##_prodefname)(void)=&Editor::proc;char name_part_##proc##_end[]=name;
#endif // FAKE_DYNAMIC
#else // PLUGIN
#ifdef PROCARRAY
#define call(x) 
#define callname(name,proc)
#else
extern int adddynprocedure(const char *name,int (Editor::*proc)(void)) ;
#define call(x)  int call##x=adddynprocedure(#x,&Editor::x) ;
#define callname(name,x)  int call##x=adddynprocedure(name,&Editor::x) ;
#endif //PROCARRAY
#endif //PLUGIN
#undef initprocedure
#define initprocedure(x) int avar##x=x();
//#undef executeprocedure
//#define executeprocedure(x) static int execute##x=addprocedure(&Editor::x);

#undef executeprocedure
#define executeprocedure(x) int execute##x=call_open(&Editor::x);
#undef call_on_open
#undef call_on_start
#undef call_on_switch
#define call_on_open(x) int call_on_open_##x=call_open(&Editor::x);
#define call_on_switch(x) int call_on_switch_##x=call_switch(&Editor::x);
#define call_on_start(x) int call_on_start_##x=call_once(&Editor::x);

#include  SOURCEFILENAME
#undef EDITOR
#undef call
#undef executeprocedure
#define executeprocedure(x)
#undef call_on_open
#undef call_on_start
#undef call_on_switch
#define call_on_open(x)
#define call_on_switch(x)
#define call_on_start(x)
#undef initprocedure
#define initprocedure(x)
#define call(x)
#undef callname
#define callname(x,y)
#define UNDER
#include SOURCEFILENAME
#undef UNDER
#if defined(PLUGIN) && defined(PROCARRAY)
int Editor::dynprocnr = sizeof(dynprocnames)/sizeof(char*);

#undef GLOBAL
#undef EDITOR
#undef executeprocedure
#undef initprocedure
#undef call
#undef call_on_open
#undef call_on_start
#undef call_on_switch
#define call(x) 
#undef callname
#define callname(x,y)


#endif
#endif
