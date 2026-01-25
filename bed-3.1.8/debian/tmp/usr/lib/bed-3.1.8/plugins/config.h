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
#define LINEMAX "100"
#define COLMAX "200"
#ifdef __linux__
#define GETOTHERTERM 1
#endif
#ifdef PDCURSES
#define USE_KEYPAD 1
	#define CURSES_HEADER <CURSESTYPE.h>
#else

#ifdef HAVE_NCURSES_H
#define CURSES_HEADER <ncurses.h>
#else
#ifdef HAVE_NCURSES_NCURSES_H
#define CURSES_HEADER <ncurses/ncurses.h>
#else
#define CURSES_HEADER <curses.h>
#endif
#endif
#endif
#include "offtypes.h"
#define WIN_MOUSE_MOVED MOUSE_MOVED
#undef MOUSE_MOVED

#if defined(PDCURSES) && defined(_WIN32)
#define DONTINTCURSES 1 // Blocks the program during threads, maybe nessessary. Probably also not working properly without threads.

#if defined(PDC_BUILD)&&PDC_BUILD<3600
#define NOENDWIN 1
#endif
#endif
