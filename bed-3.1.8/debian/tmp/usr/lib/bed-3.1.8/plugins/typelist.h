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
#ifndef TYPELIST_H
#define TYPELIST_H
class Editor;
#include "screenpart.h"

class datatypes {
	
	public:
	static 	char **names;
	static ScreenPart *(**funcs)(Editor *) ;
	static int maxlen,nr,sorted;
	datatypes(const char  str[],ScreenPart *(*func)(Editor *ed)) ;
	static ScreenPart *(*getfunc(char *name,int len)) (Editor *) ;
	};

#define datatype(type) datatypename(type,#type)

/*type is the class name of a ScreenPart
name is the namefield of type
*/
#define datatypename(type,name)\
	ScreenPart *get##type(Editor *ed) {\
			return new type( ed);\
			}\
	datatypes def##type(name,&get##type);

#endif
