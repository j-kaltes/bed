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
#ifndef MYALLOC_H
#define MYALLOC_H
#ifdef USE_NEW
char  *myreallocate(char *varname,int lench,int oldlench) ;
#define myallocar(type,len) new type[len]
#define myallocarp(ptr,type,len)  new (type)[len]
#define myalloc(type) new type
#define myallocp(ptr,type) new type
#define myfree(addres) delete addres
#define myfreear(addres) delete[] addres
#warning "delete[] addres instead of delete should be used for arrays"

#define myallocarpr(ptr,type,len)  new type[len]
#define myrealloc(cast,varname,type,len,oldlen) (cast) myreallocate((char *)varname,len*sizeof(type),oldlen*sizeof(type))
#define myallocpr(ptr,type) new type
#define myrefree(addres) delete addres
#else
#include <stdlib.h>
#ifdef DEBUGON
void *mymalloc(size_t size) ;
void freemy(void *ptr) ;
void *reallocmy(void *ptr, size_t size) ;
#endif
#define myallocar(type,len) (type *) mymalloc(sizeof(type)*len)
#define myallocarp(ptr,type,len) (ptr) mymalloc(sizeof(type)*len)
#define myallocarpr(ptr,type,len) (ptr) mymalloc(sizeof(type)*len)
#define myalloc(type) (type *)mymalloc(sizeof(type))
#define myallocp(ptr,type) (ptr)mymalloc(sizeof(type))
#define myallocpr(ptr,type) (ptr)mymalloc(sizeof(type))
#define myfree(addres) freemy(addres)
#define myfreear(addres) freemy(addres)
#define myrefree(addres) freemy(addres)
#define myrealloc(cast,varname,type,len,oldlen) (cast) reallocmy(varname,sizeof(type)*len)
#ifndef DEBUGON
#define mymalloc malloc
#define freemy free
#define reallocmy realloc
#endif
#endif
#endif /* MYALLOC_H*/
