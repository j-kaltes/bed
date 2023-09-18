
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

/*
n-bit 2's complement integers
*/

#ifdef GLOBAL
#include "global.h"
BEDPLUGIN

#include <stdio.h>
#include "../screenpart.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "signed.h"


int Signed::calcsizes(void) {
	MultiDigit::calcsizes();
	if(twoton.max)
		delete[] twoton.val;
	int byt=datbytes();
	twoton.max=byt+1;
	twoton.val=new unsigned char[twoton.max];
	if(twobit1) {
		memset(twoton.val,0xFF,byt);
		twoton.len=byt;
		}
	else
		{
		memset(twoton.val,'\0',byt);
		twoton.val[byt]=1;
		twoton.len=twoton.max;
		}
	return ++maxres;
	}
Signed::~Signed() {
	if(twoton.max)
		delete[] twoton.val;
	}
inline int istobit1(void) {
const int minone= -1;
const unsigned int maxuint=  UINT_MAX ;
	return maxuint!=static_cast<unsigned int>(minone);
	}
/*
inline int istobit1(void) {
const int32_t minone= -1;
const uint32_t maxuint= 0xFFFFFFFF;
	return maxuint!=static_cast<uint32_t>(minone);
	}
*/
	Signed::Signed ( Editor *ed,int b,int x1, int y1):MultiDigit( ed,10,b,0,x1, y1)	{
	twoton.max=0;
	twobit1=istobit1();
	calcsizes() ;
	};	
/*
	Signed::Signed ( Editor *ed,int x1, int y1): MultiDigit( ed,10,4,0,x1, y1)	{
		Signed(ed,4,x1,y1);
	};	
*/
	 Signed::Signed ( Signed const &sig):MultiDigit(sig) {
			twoton.max=0;
			twobit1=istobit1();
			calcsizes();
			}
	int Signed::form2bin(const char *formal, unsigned char *bin, int &len) const {	

		if(formal[0]=='-')	{
			mint uit,in;
			int byt=datbytes(),res;
			unsigned char  tmp[byt];
			memset(tmp,'\0',byt);
			int newlen=len-1;
			if((res=MultiDigit::form2bin(formal+1,tmp,newlen))==0)
				{
				len=1;
				return 0;
				}
			in.max=uit.max=byt;
			in.val=tmp;
			in.len=byt;	
			memset(bin,'\0',byt);
			uit.val=bin;
			uit.len=byt;
			subtract(&uit,&twoton,&in);
			len=newlen+1;
			return res;
			}
		else {
			return MultiDigit::form2bin(formal,bin,len);
			}
		return 0;
		}
/*
	int Signed::chbytes(int b) {
		int byt=datbytes();
		if(b!=byt) {
			int res=MultiDigit::chbytes(b);
			if(res==datbytes())
				initsigned();
			return res;
			}
		return byt;
		}
	short Signed::chbase(short base) {
		if(base!=type) {
			int res=MultiDigit::chbase(base);
			if(res==type)
				initsigned();
			return res;
			}
		return type;
		}
*/
	ScreenPart * Signed::newone(void) const {
		return new Signed(*this);
		}
	int  Signed::bin2formal(unsigned char *bin, char *formal, int len) {	
		int byt=datbytes();
		if(bin[byt-1]&0x80) {
			mint in,uit;
			unsigned char tmp[byt+1];
			memset(tmp,'\0',byt+1);
			uit.val=tmp;
			uit.max=byt+1;
			uit.len=0;
			in.val=bin;
			in.len=in.max=byt;
			subtract(&uit,&twoton,&in);
			MultiDigit::bin2formal(tmp,formal,len);
			formal[0]='-';
			}
		else {
			MultiDigit::bin2formal(bin,formal,len);
#ifdef SPACEEMPTY
			formal[0]=' ';
#else
			formal[0]='0';
#endif
			}
		return 0;
		}
#ifdef SPACEEMPTY
	int  Signed::isel(const int ch)const  {return (ch==' ')||(ch=='-')||MultiDigit::isel(ch);}
#else
	int  Signed::isel(const int ch) const {return (ch=='-')||MultiDigit::isel(ch);}
#endif




#ifdef Signed
parentd(ScreenPart,NewSigned) 
#include "../typelist.h"
datatypename(NewSigned,NewSigned::thename )
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
