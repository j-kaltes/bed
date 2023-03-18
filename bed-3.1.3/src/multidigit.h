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
#ifndef MULTIDIGIT_H
#define MULTIDIGIT_H
#include "screenpart.h"
#define MultiDigit Digit
class ScreenPart;
class MultiDigit: public ScreenPart {
	public:
	declareparent 
	MultiDigit(MultiDigit const &old);
	MultiDigit( Editor *ed,int b=10,int len=4,int order=0,int x1=0, int y1=0);
	virtual int chbytes(int b)  override;
	virtual short chbase(short b)  override;
	virtual ScreenPart *newone(void) const  override;
//	int formal2bin(char *formal, unsigned char *bin, int len) ;
	virtual	int form2bin(const char *formal, unsigned char *bin, int &len) const  override;
	virtual	int bin2formal(unsigned char *bin, char *formal, int len)  override;
	virtual	int isel(const int key) const override;
	virtual int calcsizes(void)  override;
	int byteoff2screen(int pos)  override;
	int screen2byteoff(int pos)  override;
	};


inline int digitsuper(ScreenPart *part) {
	return has_super(part,Digit);
	}


#endif
