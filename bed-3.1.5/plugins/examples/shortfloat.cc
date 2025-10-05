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
/*     Sun Oct  5 12:42:34 CEST 2025                                         */
/*
This is a 16 bit power of 10 float
format that is used by Bluetooth devices:
https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/others/sfloat.html
*/
#ifdef GLOBAL
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "screenpart.h"
#include "typelist.h"
#include "shortfloat.hpp"
//#include "floatdef.h"



	class ShortFloat: public General {
		static constexpr const char tobin[]{"%lg"};
		static constexpr const char frombin[]{"%010.4g"};
		public:
		declareparent 
		ShortFloat(ShortFloat const &fl):General(fl) { }
		ShortFloat( Editor *ed,int x1=0, int y1=0): General( ed,10,0,2,x1, y1) { chbytes(2);};
		ScreenPart *newone(void) const override {
			return new ShortFloat(*this);
			}
virtual	int format2bin(const char *formal, unsigned char *bin, int len) const override {
	       int start,end;
            for(start=0;start<len&&formal[start]&&isspace(formal[start]);start++)
                            ;
            for(end=start;end<len&&formal[end]&&!isspace(formal[end]);end++)
                            ;
            if(end==start)
                    return 0;
            double get;
            sscanf(formal+start,tobin,&get);
            shortfloat sfl=shortfloat::fromdouble(get);
            memcpy(bin,&sfl,2);
            return end;
            }
virtual int bin2formal(unsigned char *bin, char *formal, int len) override {
            const shortfloat *sfl=(const shortfloat*)(bin);
            sprintf(	formal,frombin,(float)*sfl); 
            return 0;
            }
		int virtual isel(const int ch) const override {return ((ch>='0'&&ch<='9')||(ch=='.')||(ch=='-')||(ch=='e')||(ch=='+')||(ch==' '));}
		};

parentd(ScreenPart,ShortFloat) 
datatype(ShortFloat)

#endif




