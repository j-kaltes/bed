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
#ifndef CONVERT_H   
#define  CONVERT_H   
#include <features.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>

#include "system.h"
#define MAXANT 256
#undef minnum
#define minnum( x,y) ((x)>(y)?(y):(x))
extern int mkmultimaxres(int len, int b) ;
inline int numel(char key) { return (key-'0');};


#include "type.h"
inline const char *modename(DataType type) {
	static	char amodename[50];
	switch(type) {
		case BIN: return "-Bin";
		case OCT: return "-Oct";
		case HEX: return "-Hex";
		case  DEC: return "-Dec";
		case NOTYPE:
		case ASCII: return "";
		default: {
			sprintf(amodename,"-%d",(int)type);
			return amodename;
			}
		}
	}
inline int hex2dig(const char ch)  {return ((ch>='0'&&ch<='9')?(ch-'0'):((ch<='f'&&ch>='a')?(ch-'a'+10):
		((ch<='F'&&ch>='A')?(ch-'A'+10):30))); }
inline int chars2dig(const char ch) {return ((ch>='0'&&ch<='9')?(ch-'0'):((ch<='z'&&ch>='a')?(ch-'a'+10):
		((ch<='Z'&&ch>='A')?(ch-'A'+10):-1))); }

inline int hexel(unsigned char x)  {
	return (((x)<10)?((x)+'0'):(x+'A'-10));
	}
#include "screenpart.h"
//s/varfromascii\(.*\));/varfrombytes\1,MAXSEARCHSTR);
inline int makebytes(ScreenPart *part,unsigned char *uit,int maxuit,unsigned char *in, int inlen) {
	int maxin=((maxuit-1)/part->bytes)* (part->maxres+part->apart);
	return part->search2bin((char *)in,uit, minnum(inlen,maxin));
	}

inline int frombytes(ScreenPart *part,unsigned char *ant,unsigned char *ascant, int len,int maxuit) {
//		int maxin=((maxuit-1)/(part->maxres+part->apart))*part->datbytes();
		int maxin=((maxuit-1)/(part->maxres+part->apart))*part->bytes;
		int flen= part->fromascii((unsigned char*)ant,(unsigned char*)ascant,minnum(len,maxin));
		return (flen>part->apart?flen-part->apart:flen);
		}

#endif
