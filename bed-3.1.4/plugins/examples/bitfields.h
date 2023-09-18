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
#ifndef BITFIELDS_H
#define BITFIELDS_H
#include "../filter.h"
#include "../screenpart.h"
#include "../hassub.h"
extern char *filtername(Bitfield) ;
typedef short Bitnr;
typedef short Bytenr;
//extern int bitcpy(unsigned char *output,unsigned char *input,char bitout,char bitin,int bitnr) ;

extern int bitcpy(unsigned char *output,const unsigned char *const input,const char bitout,const char bitin,const int bits) ;
class Bitfield: public Filter {
private:
	mutable int maxlen;
	mutable char *convstring;
	public:
	short number;
mutable	int filebits;
	Bitnr *bitnr;
	Bytenr *bytenr;
	char wantsaway;
	int wantaway(void) override ;
	Bitfield(int bits,int bytes,ScreenPart *pa);
	Bitfield(int bytes);
	Bitfield(int fb,int sb,const char *conv,ScreenPart *pa); 
	Bitfield(Bitfield const &rev);
	int extendtolen(int newnumber) ;
	int getbit(int pos);
	int calcsizes(void) const ;
	int resizeitem(int it,int size) ;
	int addrange(short beg,short len,short tobytes) ;
	virtual char *convstr(void) const override ;
	~Bitfield() ;
	virtual Filter *newone(void) const override;
	virtual int chbytes(int b)  override ;
	virtual int chbytesback(int b)  override ;
	int addposition(int pos,int bytes,int max) ;
	int rmpos(int pos) ;
	int chbitspos(int pos,int bits) ;
	int chbytespos(int pos,int bytes) ;
	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const override ;
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override ;
	virtual int  equal(const Filter *fil) const override ;

	virtual int file2screenpos(int pos)  override ;
	virtual int screen2filepos(int pos)  override ;
	};
#endif
