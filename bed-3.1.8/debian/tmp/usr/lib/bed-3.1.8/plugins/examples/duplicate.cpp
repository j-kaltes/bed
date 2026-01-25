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
The traditional hex editor is not very different from two editors
beside each other one for each dataformat;

44 6F 20 79 6F 75 20 6B 6E 6F Do you kno 
77 20 77 68 61 74 20 74 68 65 w what the 
20 70 75 72 70 6F 73 65 20 69  purpose i 
73 20 6F 66 20 61 20 68 65 78 s of a hex 
20 65 64 69 74 6F 72 3F 00 00  editor?.. 

With this filter you can put for each unit the different display units
immediately beside each other:

44 D 6F o 20   79 y 6F o 75 u 20   6B k 6E n 6F o 77 w 
20   77 w 68 h 61 a 74 t 20   74 t 68 h 65 e 20   70 p 
75 u 72 r 70 p 6F o 73 s 65 e 20   69 i 73 s 20   6F o 
66 f 20   61 a 20   68 h 65 e 78 x 20   65 e 64 d 69 i 
74 t 6F o 72 r 3F ? 00 . 00 . 00 . 00 . 00 . 00 . 00 . 

(make composed with this two field (hex,ascii) then add this filter to composed)
*/


#ifdef GLOBAL
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../filter.h"
#include "../screenpart.h"
extern char *filtername(Duplicate) ;
class Duplicate: public Filter {
	int number;
	public:
	Duplicate(int sbytes,short num=2) : Filter(sbytes,sbytes/2,filtername(Duplicate)), number(num){
		label=new char[10];
		sprintf(label,"dup %d",number);
		chbytesback(sbytes);
		}
	Duplicate(int fb,int sb,const char *conv) : Filter(fb,sb,filtername(Duplicate)) {
		sscanf(conv,"%d",&number);
		label=new char[10];
		sprintf(label,"dup %d",number);
		chbytesback(sb);
		}
	Duplicate(Duplicate const &rev): Filter(rev),number(rev.number) {
		label=new char[10];
		sprintf(label,"dup %d",number);
		chbytesback(bytesscreen);
		}
	char *convstr(void) const override {
		return label+4;
		}
	~Duplicate() {
		delete[] label;
		}
	virtual Filter *newone(void)const  override {
		return new Duplicate(*this);
		}
	virtual int chbytes(int b) override {
		bytesfile=b;
		bytesscreen=number*b;
		return bytesscreen;
		}
	virtual int chbytesback(int b)  override {
		if(number<1||b%number||b<number) {
			err=1;
			return -1;
			}
		bytesscreen=b;
		bytesfile=b/number;
		err=0;
		return bytesfile;
		}

	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const override {
		for(int i=0;i<number;i++)
			memmove(output+bytesfile*i,input,bytesfile);
		return bytesscreen;
		}
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override  {
		int i;
		for(i=0;i<(number-1);i++) {
			if(memcmp(input+bytesfile*i,output,bytesfile)) {
				memmove(output,input+bytesfile*i,bytesfile);
				return bytesfile;
				}
			}
		memmove(output,input+bytesfile*i,bytesfile);
		return bytesfile;
		}
	int  equal(const Filter *fil) const override {
		if(!Filter::equal(fil))
			return 0;
		if(number!=((Duplicate *)fil)->number)
			return 0;
		return 1;
		}
	virtual int screen2filepos(int pos)  override {
		return pos%bytesfile;
		}

	virtual int file2screenpos(int pos)  override {
		return pos;
		}
	};
registerfilterclass(Duplicate) 
#endif




#ifdef EDITOR

int duplicate(void) {
	char ans[50];
	if(inputget(ans,"Duplicate how many times? ") <0) {
		message("Cancelled");
		return -1;
		}
	int dup=atoi(ans);
	if(dup<2) {
		message("User Error: %d smaller then 2",dup);
		return -1;
		}
	int fors,bins;
	ScreenPart *super=activemode(),*act;
	act=super->getsubpart(fors,bins,super->half);
	int byt=act->datbytes();
	if(byt%dup) {
		message("User Error: %d (sizeof %s) not divisible by %d",byt,act->label,dup);
		return -1;
		}
	Duplicate *ad=new Duplicate(byt,dup);
	if(ad->error()) {
		delete ad;
		message("Error adding add");
		return -1;
		}
	addfilter(ad);
	toscreen();	
	return 0;
	}

#endif

call(duplicate)
