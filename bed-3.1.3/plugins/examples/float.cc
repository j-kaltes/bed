/* 
       -mlong-double-64

      -m96bit-long-double
       -m128bit-long-double
*/
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
#include <string.h>
#include <ctype.h>
#include "screenpart.h"
#include "typelist.h"
//#include "floatdef.h"


#define FLOATDEFS \
	floatdata2(Float,13,float,"%g","%013.7g")\
	floatdata2(Double,23,double,"%lg","%023.16lg")\
	longdoubledata
#if SIZEOF_LONG_DOUBLE<12
#define	longdoubledata
#else
#ifdef REALLONGDOUBLE16
#define	longdoubledata floatdata2(LongDouble,36,long double,"%Lg","%036.28Lg")
#else
#define	longdoubledata floatdata(LongDouble12,12,36,long double,"%Lg","%036.28Lg")\
	floatdata16(LongDouble16,16,36,long double,"%Lg","%036.28Lg")
#endif
#endif
#define floatdata16 floatdata
#define floatdata2(Name,Takes,Type,Tobin,Frombin) floatdata(Name,(int)sizeof(Type),Takes,Type,Tobin,Frombin)


	class Float: public General {
		const char *tobin;
		const char *frombin;
		public:
		declareparent 
		Float(Float const &fl):General(fl),tobin(fl.tobin),frombin(fl.frombin) { }
		Float( Editor *ed,int x1=0, int y1=0): General( ed,13,0,4,x1, y1) { chbytes(4);};
		virtual int chbytes(int b) override {
			switch(b) {
#undef floatdata
#define floatdata(Name,Bytes,Takes,Type,Tobin,Frombin) case Bytes: tobin=Tobin;frombin=Frombin;maxres=Takes;datbytes(Bytes);break ;

				FLOATDEFS
				case -1: return 1;
				default: return -1;
				}
			return datbytes();
			}
#if SIZEOF_LONG_DOUBLE<12
#define floatnr 2
#else
	#if defined(REALLONGDOUBLE16)|| SIZEOF_LONG_DOUBLE==12
	#define floatnr 3
	#else
	#define floatnr 4
	#endif
#endif
virtual int userconfig(int kind) override {
	return sizelabels(kind, floatnr , "~float",sizeof(float),"~double",sizeof(double)
#if SIZEOF_LONG_DOUBLE>=12
,
#if floatnr==3
"~long double",sizeof(long double)
#else
"lo~ng double (12)",12,"~long double (16)",16
#endif
#endif
);
	}
		ScreenPart *newone(void) const override {
			return new Float(*this);
			}
virtual	int format2bin(const char *formal, unsigned char *bin, int len) const override {
	       int start,end;
                for(start=0;start<len&&formal[start]&&isspace(formal[start]);start++)
                                ;
                for(end=start;end<len&&formal[end]&&!isspace(formal[end]);end++)
                                ;
                if(end==start)
                        return 0;
#if floatnr<4
		sscanf(formal+start,tobin,bin);
#else
	unsigned int by=datbytes();
	if(by>sizeof(double)&&by<sizeof(long double)) {
		long double get;
		sscanf(formal+start,tobin,&get);
		memcpy(bin,&get,by);
		}
	else
		sscanf(formal+start,tobin,bin);
#endif
		return end;
		}
		virtual int bin2formal(unsigned char *bin, char *formal, int len) override {
			switch(datbytes()) {

#undef floatdata
#define floatdata(Name,Bytes,Takes,Type,Tobin,Frombin) floatdatacp(Name,Bytes,Bytes,Takes,Type,Tobin,Frombin)
#if SIZEOF_LONG_DOUBLE==12
#undef floatdata16
#define floatdata16(Name,Bytes,Takes,Type,Tobin,Frombin) floatdatacp(Name,Bytes,sizeof(Type),Takes,Type,Tobin,Frombin)
#endif
#undef floatdatacp
#define floatdatacp(Name,Bytes,cpbytes,Takes,Type,Tobin,Frombin)\
			case Bytes: {\
				if(len<Bytes) {\
					memcpy(formal,"Error",6);\
					return -1;\
					}\
				Type get=0;\
				memcpy(&get,bin,cpbytes);\
				if(!(get<0)&&!(get>0)&&!(get==0)) {\
					memset(formal, ' ',Takes-3);\
					memcpy(formal+Takes-3,"nan",4);\
					return 0;\
					}\
				sprintf(	formal,frombin,get); };break;
				FLOATDEFS
				default: return -1;
				};
			return 0;
			}
		int virtual isel(const int ch) const override {return ((ch>='0'&&ch<='9')||(ch=='.')||(ch=='-')||(ch=='e')||(ch=='+')||(ch==' '));}
		};

parentd(ScreenPart,Float) 
datatype(Float)

#endif




