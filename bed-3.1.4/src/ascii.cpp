#include "defines.h"
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
#include <string.h>
#include <ctype.h>
#include "ascii.h"
Ascii::Ascii ( Editor *ed,int x1, int y1): ScreenPart( ed, 1, 256, 1 ,x1, y1,0)	{
//	name=	Ascii::thename;
	nonprint='.';
	problemchars=0;
	};	
Ascii::Ascii(Ascii const &part): ScreenPart(part){
		nonprint=part.nonprint;
		problemchars=part.problemchars;
		}
ScreenPart *Ascii::newone(void) const {return new Ascii(*this);}
int Ascii::form2bin(const char *formal, unsigned char *bin, int &len) const {	
		memmove((char *)bin,formal,len);
		return len;	
		}
int Ascii::addkey(unsigned char *bin,int binlen, int key, int pos ) {
		bin[pos]=key;
		return 0;
		}
//int Ascii::isel(int ch) {return (!iscntrl(ch)) ;}
#define low(x) (x>31&&x<0x7f) 
#define all(x) (x&0xe0)&&(!(x==0x7f))
#define lowup(x) (x>0xA0||(x>0x1f&&x<0x7f))

inline int Ascii::isel(const int ch) const {
		switch(problemchars) {
			case 0: return low(ch);
			case 'a':return all(ch);
			default :return lowup(ch); /*==u */
			};
		};
//int Ascii::isel(int ch) {return (isprint(ch)) ;}
int Ascii::bin2formal(unsigned char *bin, char *formal, int len) {	
		unsigned char ch;
		for(int i=0;i<len;i++) {
			ch= bin[i];
			if(isel(ch))
				formal[i]=ch;	
			else
				formal[i]=nonprint;
			}
		return 0;	
		}	

int Ascii::chbytes(int b) {
		if(b==-1)
			return 1;
		if(b>0) {
			if(b>1)
				apart=1;
			else
				apart=0;
			datbytes(b);
			maxres=b;
			}
		return b;
		}
char *Ascii::getconvstr(void) {
		static char convstr[3]={0,0,0};
		convstr[0]=nonprint;
		convstr[1]=problemchars;
		return convstr;
		}
int Ascii::setconvstr(char *str) {
		if(isel(str[0])) {
			nonprint=str[0];
			}
		else
			return -1;
		problemchars=str[1];
		return 0;
		}

int Ascii::byteoff2screen(int pos) {
		return pos;
		}
int Ascii::screen2byteoff(int pos) {
		return pos;
		}

parentd(ScreenPart,Ascii)
