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
#ifndef REVERSE_H
#define REVERSE_H
#include "filter.h"
extern char *Reversename;
extern char * reverse(const char *const in, char *const out,const  int len,const short unit) ;
class Reverse: public Filter {
	public:
	int unit;
	Reverse(int len,int u=1) : Filter(len,len),unit(u) {
		label=new char[10];
		snprintf(label,10,"r%d",unit);
		name=Reversename;
		if(unit<1)
			err=1;
		}
	Reverse(Reverse const &rev): Filter(rev) {
		name=Reversename;
		unit=rev.unit;
		label= new char[10];
		strcpy(label,rev.label);
		}
	~Reverse() {
		delete[] label;
		}
	Reverse(int fb,int sb,const char *conv):Filter(fb,fb,Reversename) {
		sscanf(conv,"%d",&unit);
		char *tmp=new char[10];
		snprintf(tmp,10,"r%d",unit);
		label=tmp;
		}
	virtual char *convstr(void) const override {
		return label+1;
		}
	virtual Filter *newone(void) const override{
		return new Reverse(*this);
		}
	virtual int chbytes(int bytes) override{
		bytesscreen=bytesfile=bytes;
		return bytes;
		}
	virtual int chbytesback(int bytes) override{
		return chbytes(bytes);
		}
	virtual int fromfile(const unsigned char *const input,unsigned char *const output) const override {
		reverse((char *)input,(char *)output,bytesscreen,unit);
		return bytesscreen;
		}
	virtual int tofile(const unsigned char * const input,unsigned char *const output) override {
		reverse((char *)input,(char *)output,bytesscreen,unit);
		return bytesscreen;
		}
	virtual int  equal(const Filter *fil) const override {
		if(!Filter::equal(fil))
			return 0;
		if(((Reverse *)fil)->unit!=unit)
			return 0;
		return 1;
		};

	virtual int screen2filepos(int pos) override{
		int units=bytesscreen/unit;
		int theunit=pos/unit;
		int inun=pos%unit;
		int rev=units-theunit-1;
		return (rev*unit+inun);
		}

	virtual int file2screenpos(int pos) override{
		return screen2filepos(pos);
		}
	};
#endif
