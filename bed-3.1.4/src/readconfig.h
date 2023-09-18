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
#ifndef READCOLORS_H
#define READCOLORS_H

#ifndef NOATTRIBUTES
#define TEXTATTR (fieldcolor[0].var)
#define COMMANDATTR (fieldcolor[1].var)
#define BARATTR (fieldcolor[2].var)
#define BARSTANDATTR (fieldcolor[3].var)
#define MENUATTR (fieldcolor[4].var)
#define MENUKEYATTR (fieldcolor[5].var)
#define MENUSELATTR (fieldcolor[6].var)
#define TITLEATTR (fieldcolor[7].var)
#define KEYATTR (fieldcolor[8].var)
#define BACKATTR (fieldcolor[9].var)
#define SELATTR (fieldcolor[10].var)
#define UNSELATTR (fieldcolor[11].var)
#define SELSELATTR (fieldcolor[12].var)
#define SHOWBACK (fieldcolor[13].var)
#define  SELECTIE (fieldcolor[14].var)
#define  CURSORATTR (fieldcolor[15].var)
#define  SELCURSORATTR (fieldcolor[16].var)
#define BETWEENPARTSATTR  (fieldcolor[17].var)
#define BETWEENFIELDSSATTR (fieldcolor[18].var)
#define  CURSORNOACTIVEATTR (fieldcolor[19].var)

#endif
#define NOWARNUNUSEDVALUE(x) (void) x
extern int readconfig(void);

#ifndef NOATTRIBUTES
typedef struct { char text[20]; chtype var; int colornr;int forg;int back; } Fieldcol;
extern Fieldcol fieldcolor[];
#define normaal(editscreen) NOWARNUNUSEDVALUE(wattrset(editscreen,TEXTATTR))
#else
#define normaal(editscreen) 
#endif

//s/wattrset([^)]*)/NOWARNUNUSEDVALUE(&)/
#ifndef NOATTRIBUTES
#define SELECTIEON			NOWARNUNUSEDVALUE(wattrset(editor->editscreen,SELECTIE))
#else
#define SELECTIEON			NOWARNUNUSEDVALUE(wattrset(editor->editscreen,SELECTIE))
#endif
//#define CURSORON		 if(editor->selected(editor->filepos+pos)) NOWARNUNUSEDVALUE(wattrset(editor->editscreen,0))
#ifndef NOATTRIBUTES
#define CURSORON		{if(!editor->selected(editor->filepos+pos)) NOWARNUNUSEDVALUE(wattrset(editor->editscreen,CURSORATTR)); else NOWARNUNUSEDVALUE(wattrset(editor->editscreen,SELCURSORATTR));}
#define CURSORONPAS		{if(!editor->selected(editor->filepos+pos)) NOWARNUNUSEDVALUE(wattrset(editor->editscreen,  CURSORNOACTIVEATTR)); else NOWARNUNUSEDVALUE(wattrset(editor->editscreen,SELECTIE));}
#else
#define CURSORON		
#endif
#define CURSOROFF		{if(!editor->selected(editor->filepos+pos)) normaal(editor->editscreen); else SELECTIEON;}
//#define CURSOROFF
//#define CURSORALL		| ((editor->selected(editor->filepos+pos))?(SELCURSORATTR):(CURSORATTR))
#define CURSORALL		

#endif
