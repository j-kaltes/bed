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
#ifndef KEYSTROKES_H
#define KEYSTROKES_H
#include <limits.h>
const int INVALID_KEY=ERR;
const int KEYSTROKE_ERROR=INT_MIN;
#include "editor.h"
typedef char Strokekeytype; 
class Keystrokes {
	private:
	int file;
	char *name;
	Strokekeytype *buf;
	int inbuf;
	int *inp,out,max;
	public:
	Keystrokes(char *name=0) ;
	~Keystrokes() ;
	int mmaptofile(char *name) ;
	void alloc(int len);
	void add(int key) ;
	inline int pending(void) {
		return *inp-out;
		}
	void flush(void) ;
	char *getname(void) {return name;}
	int get(void) ;
	int peek(void);
	int last(void) ;
	int fill(Keystrokes &other) ;
	int fill(Strokekeytype *keys,int len) ;
friend int Editor::playrecord(void) ;
friend int addtorecordkeys(int key);
friend int Editor::record(void);
friend int Editor::saverecord(void) ;
friend int Editor::playmacrofile(const char *name);
friend int	endrecording(void) ;

	};
#endif
