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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "type.h"
#include "screenpart.h"
#include "digitstring.h"
#include "convert.h"
#include "multidigit.h"
parentd(ScreenPart,Digit)
	 int MultiDigit::calcsizes(void) {
		maxres=mkmultimaxres(datbytes(),type);
		ScreenPart::calcsizes();
		return maxres;
		}
	MultiDigit::MultiDigit(MultiDigit const &old): General(old) {
		}
	MultiDigit::MultiDigit( Editor *ed,int b,int len,int order,int x1, int y1):
		General( ed,mkmultimaxres(len,b),b,len,x1, y1) {
	//	calcsizes();
//		name=Digit::thename;
		}; 

	int MultiDigit::chbytes(int b) {
		if(b==-1)
			return datbytes();
		if(b>0) {
			datbytes(b);
			calcsizes(); 
			if(half<0||half>=maxres)		
				half=maxres-1;
			return b;
			}
		else
			return -1;
		};

	short MultiDigit::chbase(short b) {
		if(b==-1)
			return type;
		if(!b)
			return -type;
		if(b<2||b>36) {
			return -1;
			}
		type=b;
		calcsizes(); 
		if(half<0||half>=maxres)		
			half=maxres-1;
		return b;
		}

	ScreenPart *MultiDigit::newone(void) const {
		return new MultiDigit(*this);
		}

int fromdigitstring(const char *dec,int declen,int base,struct mint *uit);

	int MultiDigit::form2bin(const char *formal, unsigned char *bin, int &len) const {
		int start,end;
		for(start=0;start<len&&formal[start]&&isspace(formal[start]);start++) 
				;
		for(end=start;end<len&&formal[end]&&!isspace(formal[end]);end++) 
				;
		if(end==start) {
			len=0;
			return 0;
			}
		struct mint uit;
		uit.val=bin;
		uit.len=0;
//		uit.max=datbytes()+4;
		uit.max=datbytes();
		int byt=datbytes();
		memset(uit.val,0,byt);
		if(fromdigitstring(formal+start, end-start,type,&uit)!=0) {
			len=end;
			return byt;
			}
		len=end;
		return byt;
		}
/*
	int MultiDigit::formal2bin(char *formal, unsigned char *bin, int len) {
		struct mint uit;
		uit.val=bin;
		uit.len=0;
//		uit.max=len+4;
		uit.max=datbytes()+4;
		if(fromdigitstring(formal, maxres,type,&uit)!=0) {
			memset(bin,0,datbytes());
			}
		return 0;
		}
*/
	int MultiDigit::bin2formal(unsigned char *bin, char *formal, int len) {
		struct mint in;
		in.val=bin;
		in.len=len;
		in.max=len;
		todigitstring(&in,type,formal,maxres);
		return 0;
		}
	inline int MultiDigit::isel(const int key) const { int el=chars2dig(key); return (el>=0&&el<type); }
#include <limits.h>
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifdef  DBL_MAX 
#define MAXDOUBLE DBL_MAX 
#endif
#ifndef MAXDOUBLE 
	#ifdef __CYGWIN__
	#define MAXDOUBLE 1.7976931348623157e+308
	#else
	#include <values.h>
	#endif
#endif


static int maxcalcbytes=(int)floor(log(MAXDOUBLE )/log((double)256.0));
int mkmultimaxres(int len, int b) {
	if(b<1)
		return 0;
	if(len>=maxcalcbytes) {
		double ret= ceil((len*log((double)256.0)/log((double)b)));
		return (int) ret;
		}
//		return (int) ceil((8.0*len*log(2))/log(b));
	else
		return (((int)ceil(log(pow((double)2.0,(double)8.0*len)-1)/log((int)b))));
	}
int deelbaarlog[]={1,2,4}; 
int deelbaar[]={2,4,16}; 

int Digit::byteoff2screen(int pos) {
	int by=datbytes();
	if(by<2)
		return -1;
	for(int i=0;i<3;i++)
		if(type==deelbaar[i])
			return ((long long)pos*maxres)/by;
	return -1;
	}
int Digit::screen2byteoff(int pos) {
	int by=getbytes();
	if(by<2)
		return -1;
	for(int i=0;i<3;i++)
		if(type==deelbaar[i])
			return ((long long)pos*by)/maxres;
	return -1;
	}
