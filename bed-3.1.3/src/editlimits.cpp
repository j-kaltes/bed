#include "defines.h"
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

#define certain 0
inline long long inmem(long long x) { 
	long long ret= ((x+(2*STBLOCKSIZE))+((((x/STBLOCKSIZE)/NUMBEROFTREELS)+1)*NUMBEROFTREELS*sizeof(Treel)));
	return ret;
	} /*rbtree takes NUMBERORTREELS a time of 7 byte? */
inline long long usecopy(long long x) { 
	long long ret= (3*x+inmem(x))+certain;
	return ret;
	} /*In selbuf when pasted in undo buf old and in memory new. Undo makes takes more memory than needed now*/
inline long long usetrunc(long long x) {return (2*x+inmem(x))+certain;} /*in undo buffer and by undo in memory */
inline long long useext(long long x) {
	long long ret= (2*x+inmem(x)+certain); 
	return ret;
	} /*In memory zero's and by undo in undo buffer */

 const int SHOULDMEM=1024*1024;
 const int MAXSELBUF=254094;	//((SHOULDMEM)*((double)SHOULDMEM))/usecopy(SHOULDMEM);
 const int MAXTOUNDO=337953;	//((SHOULDMEM)*((double)SHOULDMEM))/usetrunc(SHOULDMEM);
 const int MAXEXTEND=337953;	//((SHOULDMEM)*((double)SHOULDMEM))/useext(SHOULDMEM);


