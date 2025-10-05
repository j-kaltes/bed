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
#include <stdarg.h>
#include "editor.h"
#include "myalloc.h"
#include "onlykey.h"
#include "keyproc.h"
#include "system.h"
#include "screen.h"


extern int setkeystring(unsigned char *keybase,int nr,const char *str) ;
 void KeyEdit::startkeys(void) {

#ifdef OLDGCCVARMACRO
#define keylabeldef(proc, keys...)  setkeystring((unsigned char []){menukey, keys},sizeof((unsigned char []){menukey, keys}),#proc); VARMACRO
#define keydef(proc, keys...)  keymap((unsigned char []){keys},sizeof((unsigned char []){keys}),&Editor::proc); VARMACRO
#else
#define keylabeldef(proc,...)  {unsigned char tmpkeys[]={menukey,__VA_ARGS__};setkeystring(tmpkeys, sizeof(tmpkeys),#proc);};
#define keydef(proc, ...)  {unsigned char tmpkeys[]={__VA_ARGS__};keymap(tmpkeys, sizeof(tmpkeys),&Editor::proc);};
#endif

	#include "builtinkeys.h"
	#include "extrakeys.h"
#undef keydef
#undef keylabeldef
	addterminfokeys();
	};
intptr_t KeyEdit::terminfokey(const char *keyname,int (Editor::*proc)(void)) {
#ifdef HAS_TIGETSTR
 	char *out=tigetstr(const_cast<char *>(keyname));
	switch((intptr_t)out) {
		case 0L:;
//		case -1L: return reinterpret_cast<intptr_t>(out);
		case -1L: return (intptr_t)out;
		};
	int len=strlen(out);
	return keymap((unsigned char*)(out),len,proc);
//	return keymap(reinterpret_cast<unsigned char*>(out),len,proc);
#else
	return 0L;
#endif
	}
	


extern const int MAXKEYS;
int gettermcap(const char *input,unsigned char *output) ;
int termkeycapprocstring(const char *termcap,const char *procname) {
	unsigned char keybase[MAXKEYS],*keys=keybase+1;
	keybase[0]=menukey;
	int len;
	if((len=gettermcap(termcap,keys))>0) 
		return setkeystring(keybase,len+1,procname);
	return -1;
	}

int KeyEdit::addterminfokeys(void) {
#define termkey(name,proc)  terminfokey(name,funcptr(Editor,proc));
#define termkeylabel(termcap,procname)  termkeycapprocstring(termcap,procname);
#include "termkeys.h"
#undef termkey
#undef termkeylabel
	return 0;
	};

int KeyEdit::keymaped(int (Editor::*proc)(void),int nr,...) {
	va_list ap;
	int i;
	unsigned char ch[nr];
//	int *str;
	va_start(ap,nr);
	for(i=0;i<nr;i++) {
		ch[i]=static_cast<unsigned char>(va_arg(ap,int));
/*
What is this for??
		if(ch[i]==listshow) {
			str=va_arg(ap,int*);
			while(*str!=listshow)
				ch[i++]=*str++;
			}
*/
		}
	va_end(ap);
	return keymap(ch,nr,proc);
	}
 int KeyEdit::keymap(unsigned char *keybuf,int nrkeys, int (Editor::*proc)(void)) {
	int (Editor::**ptr)(void)=new (int (Editor::*)(void));
	(*ptr)=proc;
	return putseq((intptr_t)ptr,nrkeys, keybuf) ;
//	return putseq(reinterpret_cast<long>(ptr),nrkeys, keybuf) ;
	}
KeyEdit keyedit;
