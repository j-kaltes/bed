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
#ifdef ATOIBUG
#include <features.h>
#include <stdlib.h>
int strtoint(char *str) {
	const int res= atoi(str);
	return res;
	}
long mystrtol(char *bytes,char **endp,int base) {
	long res=strtol(bytes,endp,base);
	return res;
	}
/*
#ifdef LARGEFILES
long long strtolonglong(char *str) {
#ifdef HAS_ATOLL
	long long res= atoll(str);
#else
	long long res=strtoll(str,(char **)NULL,10);
#endif
	return res;
	}
#endif
*/
#endif

