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
#ifndef ASCII_H
#define ASCII_H
#include "type.h"
#include "screenpart.h"




class  Ascii : public ScreenPart {	
	public:	 
	declareparent 
		
	char problemchars;
	unsigned char nonprint;	
	Ascii ( Editor *ed,int x1=0, int y1=0);
	Ascii(Ascii const &part);
	ScreenPart *newone(void) const override;
	int form2bin(const char *formal, unsigned char *bin, int &len) const override;
	int addkey(unsigned char *bin,int binlen, int key, int pos ) override;
	int bin2formal(unsigned char *bin, char *formal, int len) override; 	
	int chbytes(int b)  override;
//	virtual short chbase(short base) override {return -1;};
	int isel(const int ch) const override;
	char *getconvstr(void) override;
	int setconvstr(char *str) override;
	virtual int userconfig(int kind) override;
	int byteoff2screen(int pos) override ;
	int screen2byteoff(int pos) override ;
	};
#define asciisuper(x) has_super(x,Ascii)
#endif
