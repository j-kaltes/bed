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
#ifdef GLOBAL
#include <stdio.h>
#include <ctype.h>
#include "../screenpart.h"
#include <limits.h>
class  unsignedint : public General {	
	public:	 unsignedint ( Editor *ed,int x1=0, int y1=0): General( ed, 10 ,10, 4 ,x1, y1)	{
//		name="unsignedint";
		};	
	declareparent 
	virtual int format2bin(const char * const formal, unsigned char *bin, int len) const override {	
	       int start,end;
                for(start=0;start<len&&formal[start]&&isspace(formal[start]);start++)
                                ;
                for(end=start;end<len&&formal[end]&&!isspace(formal[end]);end++)
                                ;
                if(end==start)
                        return 0;
		long long get;
		sscanf(formal, "%Lu" ,&get);	
		if(get>INT_MAX||get<INT_MIN )
			return 0;
		*((int*)bin)=get;
		return end;	
		}
	
		ScreenPart *newone(void) const override {
			return new unsignedint(*this);
			}
virtual	int bin2formal(unsigned char *bin, char *formal, int len) override {	
	sprintf(	formal, "%10.10u" ,*(( int *)bin));	return 0;	}	
virtual	int isel(const int ch) const override {return ((ch>='0'&&ch<='9')||(ch=='-')||(ch==' '));}
	};

parentd(ScreenPart,unsignedint)
datatype(unsignedint)
#endif



#ifdef EDITOR
#endif

