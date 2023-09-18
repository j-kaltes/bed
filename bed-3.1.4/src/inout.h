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
#ifndef INOUT_H
#define INOUT_H
#include "editor.h"
#include "screen.h"
#include "main.h"
#define editmove(y,x) wmove(editscreen,y,x)
#define NORMAAL normaal(editscreen) 
#define UNUSEDVALUE(x) (void) x
//s/wattrset([^)]*)/UNUSEDVALUE(&)/g
#define	COMMANDON UNUSEDVALUE(wattrset(editscreen,COMMANDATTR));

#define	COMMANDOFF NORMAAL
#define LIVEWITHXITERM
#define MESSAGELINE Screenheight-nomessage





inline void Editor::overline() {for(int i=0;i<Screenwidth;i++) waddch(editscreen,' ');}
extern int ismessage;
#define					rewritescreen redistribute
inline void Editor::textovermessage(void) {editmove(MESSAGELINE,0);clearline();putline(MESSAGELINE);cursorpos();ismessage=0;}
inline void Editor::restoremessageline(void)	{if(nomessage&&ismessage) textovermessage();};


#endif
