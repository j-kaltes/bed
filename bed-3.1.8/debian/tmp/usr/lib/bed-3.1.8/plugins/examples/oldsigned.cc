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
#include "global.h"
BEDPLUGIN

#include <stdio.h>
#include "../screenpart.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>

#define signedname "Signed"
static inline int mkmultimaxres(int len, int b) {return (((int)ceil(log(pow(2,8*len)-1)/log((int)b))));}
#include "oldsigned.h"
	Signed::Signed ( Editor *ed,int b,int x1, int y1): General( ed, 11,10, b ,x1, y1)	{
	bytes=b;
	int eb=datbytes();
#ifdef HAS_STRTOLL
	if(eb>8||!strchr("1248",eb+'0')) {
#else
	if(eb>4||!strchr("124",eb+'0')) {
#endif
		bytes=4;
		}

//	name=signedname;
	memcpy(frombin,"%0",2);
	chbytes(bytes);
	};	
	Signed::Signed ( Editor *ed,int x1, int y1): General( ed, 11 ,10, 4 ,x1, y1)	{
//	name="Signed";
	memcpy(frombin,"%0",2);
	};	
	 Signed::Signed ( Signed const &sig):General(sig) {
			memcpy(frombin,sig.frombin,12);
			}
int Signed::userconfig(int kind) {
	return sizelabels(kind,4, "~char",sizeof(char),"~short",sizeof(short),"~int",sizeof(int),"~long long",sizeof(long long));
	}
	int Signed::format2bin(const char *formal, unsigned char *bin, int len) const {	
#ifdef HAS_STRTOLL
		char *endp;
		long long get=strtoll(formal,&endp,type);
		len=endp-formal;
#else
		long  get=0;
		sscanf(formal,"%ld",&get);
#endif
		switch(datbytes()) {
		case	sizeof(char): if(get<SCHAR_MIN||get>SCHAR_MAX)
				return -1;
			*((char*)bin)=(char)get;
			break;
		case	sizeof(short): if(get<SHRT_MIN||get>SHRT_MAX)
				return -1;
			*((short*)bin)=(short)get;
			break;
		case	sizeof(int): if(get<INT_MIN||get>INT_MAX)
				return -1;
			*((long*)bin)=(long)get;
			break;
		case	sizeof(long long): *((long long*)bin)=get;
				break;
		default: return 0;
			}
		return len;	
		}

	int Signed::chbytes(int b) {
		const char *lab;
		int oldbytes=datbytes();
		datbytes(b);
		switch(datbytes()) {
			case sizeof(char): lab="hd";break;
			case sizeof(short):lab="hd";break;
			case sizeof(int):lab="ld";break;
			case sizeof(long long):lab="lld";break;
			case -1: return 1;
			default: datbytes(oldbytes); return -1;
			};
		maxres=mkmultimaxres(datbytes(),type)+1;
		sprintf(frombin+2,"%d.%d%s",maxres,maxres-1,lab);
		return datbytes();
		}
	ScreenPart * Signed::newone(void) const {
		return new Signed(*this);
		}
	int  Signed::bin2formal(unsigned char *bin, char *formal, int len) {	
		long long get;
		if(bin[len-1]&0x80)
			memset((char *)&get,0xFF,sizeof(long long));
		else
			memset((char *)&get,0,sizeof(long long));
		memcpy((char *)&get,bin,len);
		snprintf(	formal,maxres+1, frombin ,get);	return 0;	
		}	
	int  Signed::isel(const int ch) const {return ((ch>='0'&&ch<='9')||(ch=='-')||(ch==' '));}



#ifdef Signed
parentd(ScreenPart,OldSigned) 
#include "../typelist.h"
datatypename(OldSigned,OldSigned::thename )
#else
parentd(ScreenPart,Signed) 
#include "../typelist.h"
datatypename(Signed,Signed::thename )
#endif


#endif



#ifdef EDITOR
/*
int Editor::SIGNEDINT (void) {
	ScreenPart *act=activemode();
	Signed  *part=new  Signed ( this,act->bytes);
	if(installpart(part)==-1) {	
		delete part;	
		message("Adding " "int" " failt");	
		return -1;	
		}	
	return 0;	
	}  
*/

#endif

//call(SIGNEDINT)
