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
#ifndef KEYPROC_H
#define KEYPROC_H
#include "keyinput.h"
#include "debug.h"
class Editor;
class KeyEdit: public keyinput {
public:
void startkeys(void) ;
inline void  *lookup(unsigned char input) {
	intptr_t  proc=keyseq(input) ;
DEBUGGING("lookup %lx(%ld)\n",proc,proc);
	if(proc==-1||proc==-2) {
DEBUGGING("-1||-2");
		return NULL;
		}
DEBUGGING("proc");
	return (void*) proc;
	};
int keymaped(int (Editor::*proc)(void),int nr,...) ;
    int keymap(unsigned char *keybuf,int nrkeys, int (Editor::*proc)(void)) ;

 intptr_t terminfokey(const char *keyname,int (Editor::*proc)(void)) ;

int addterminfokeys(void) ;
	};

extern KeyEdit keyedit;
#define setkeymap(keyedit,keys,nr,func) keyedit.keymap(keys,nr,func)
#include "varmacro.h"

#ifdef OLDGCCVARMACRO
#define setkeymaped(keyedit,func,proc...) keyedit.keymaped (func,                    ##proc) VARMACRO
#else
#define setkeymaped(keyedit,func,...) keyedit.keymaped (func,  __VA_ARGS__) 
#endif

#define defaultkeys(keyedit) keyedit.startkeys();
#define endkey(keyedit) keyedit.isstart()
#define	resetkey(keyedit)	keyedit.reset()
#define procproc(keyedit,procedure) *((int (Editor::**)(void))procedure)
#define lookkey(keyedit,input) keyedit.lookup(input)
#endif
