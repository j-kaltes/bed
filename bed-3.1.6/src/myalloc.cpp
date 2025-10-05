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
#include "myalloc.h"
#ifdef USE_NEW
#include <string.h>
char  *myreallocate(char *varname,int lench,int oldlench) {
	char *var=new  char[lench];
	memcpy(var,varname,oldlench);
	delete[] varname;
	return var;
	}

#else
#ifdef USEMALLOC
void * operator new(size_t size) {
	return  mymalloc(size);
	};
void  operator delete(void *p) { 
	freemy(p);
	}
#endif
#ifdef DEBUGON
void *mymalloc(size_t size) {
	void *mal=malloc(size);
	return mal;
	}
void freemy(void *ptr) {
	free(ptr);
	}
void *reallocmy(void *ptr, size_t size) {
	void *re=realloc(ptr,size) ;
	return re;
	}

#endif
#endif
