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
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "../hassub.h"
#ifdef EDITOR
BEDPLUGIN
class Editor {
public:
#include "faked.h"
int Editor::getanswer(const char *question,char *answ,int anslen,int ansroom=5);
int bitfield(void) ;
int changebitfield(void) ;
};
#else
#endif
#include "bitfields.h"
class BitCon;
//s/=\(.*\);/(\1),/g

	Bitfield::Bitfield(int bits,int bytes,ScreenPart *pa):  Filter(bytes,bytes,filtername(Bitfield),filtername(Bitfield)), 
		maxlen(0),
		number(1),
		bitnr(new Bitnr[1]),
		bytenr(new Bytenr[1]),
		wantsaway(1)
	{

		part=pa;

		bitnr[0]=bits;
		bytenr[0]=bytes;
		calcsizes();
		}
	int Bitfield::wantaway(void) {
		return wantsaway;
		}


    Bitfield::Bitfield(int bytes): Filter(bytes,bytes,filtername(Bitfield),filtername(Bitfield)),                         
                maxlen(0),                                                                           
                number(0),                                                                           
                wantsaway(1)                                                                         
                {                                                                                    
                part=NULL;                                                                                                    
                calcsizes();                                                                         
                }                                                                                    
                        

//Format: want:fbits:number:b;by,b;by,
	Bitfield::Bitfield(int fb,int sb,const char *conv,ScreenPart *pa) : Filter(fb,sb,filtername(Bitfield),filtername(Bitfield)),
		maxlen(0)
		{
		part=pa;

		const char *ptr=conv;
		char *endptr;
		wantsaway=(*ptr++=='0')?0:1;
		if(*ptr++!=':')
			err=1;
		filebits=strtol(ptr, &endptr,16);
		if(ptr==endptr)
			err=2;
		ptr=endptr;
		if(*ptr++!=':')
			err=3;
		number=strtol(ptr, &endptr,16);
		if(ptr==endptr)
			err=4;
		ptr=endptr;
		if(*ptr++!=':')
			err=5;
		bitnr=new Bitnr[number];
		bytenr=new Bytenr[number];
		for(int i=0;i<number;i++) {
			bitnr[i]=strtol(ptr, &endptr,16);
			if(ptr==endptr)
				err=6;
			ptr=endptr;
			if(*ptr++!=';')
				err=7;
			bytenr[i]=strtol(ptr, &endptr,16);
			if(ptr==endptr)
				err=8;
			ptr=endptr;
			ptr++;
			}
		calcsizes();
		}

	Bitfield::Bitfield(Bitfield const &rev): Filter(rev),maxlen(0),number(rev.number),filebits(rev.filebits) ,
		bitnr(new Bitnr[number]),
		bytenr(new Bytenr[number]),
		wantsaway(rev.wantsaway)
		{
		part=NULL;
		memcpy(bitnr,rev.bitnr,sizeof(Bitnr)*number);
		memcpy(bytenr,rev.bytenr,sizeof(Bytenr)*number);
		}
	char *Bitfield::convstr(void) const {
//Format: want:fbits:number:b;by,b;by,
// space bitnr and bytenr times number, space number, space filebits, space spec times count + space zero;
int newlen= 2*(((sizeof(Bitnr)+sizeof(Bytenr))*number)+sizeof(number)+sizeof(filebits))+ number*2+4+1+1;
		if(newlen>maxlen) {
			if(maxlen) {
				delete[] convstring;
				}
			maxlen=newlen;
			convstring=new char[maxlen];
			}
		int len=snprintf(convstring,maxlen,"%c:%x:%hx:",(wantsaway?'1':'0'),filebits,number);
		for(int i=0;i<number;i++)
			len+=snprintf(convstring+len,maxlen-len,"%hx;%hx,",bitnr[i],bytenr[i]);
		return convstring;
		}
	Bitfield::~Bitfield() {
		if(number) {
			delete[] bitnr;
			delete[] bytenr;
			}
		if(maxlen)
			delete convstring;
		}
	Filter *Bitfield::newone(void) const {
		calcsizes(); /*Removed otherwise not const*/
		return new Bitfield(*this);
		}
	int Bitfield::chbytes(int b) {
//		calcsizes();
		return bytesscreen;
		}
	int Bitfield::chbytesback(int b) {
//			calcsizes();
		return bytesfile;
		}
/*
What to do if pos>number?
*/
	int Bitfield::extendtolen(int newnumber) {
		if(newnumber<=number)
			return number;
		Bitnr *bittmp=new Bitnr[newnumber];
		Bitnr *bytetmp=new Bytenr[newnumber];
		if(number>0) {
			memcpy(bittmp,bitnr,number*sizeof(Bitnr));
			memcpy(bytetmp,bytenr,number*sizeof(Bytenr));
			delete[] bitnr;
			delete[] bytenr;
			}
		else
			number=0;
		memset(bittmp+number,'\0',sizeof(Bitnr)*(newnumber-number));
		memset(bytetmp+number,'\0',sizeof(Bytenr)*(newnumber-number));
		bytenr=bytetmp;
		bitnr=bittmp;
		number=newnumber;
		return number;
		}
		
	int Bitfield::addposition(int pos,int bytes,int max) {
		if(max>=number) {
			Bitnr *bittmp=new Bitnr[number+1];
			Bitnr *bytetmp=new Bytenr[number+1];
			if(pos) {
				memcpy(bittmp,bitnr,pos*sizeof(Bitnr));
				memcpy(bytetmp,bytenr,pos*sizeof(Bytenr));
				}
			if(number>pos) {
				memcpy(bittmp+pos+1,bitnr+pos,(number-pos)*sizeof(Bitnr));
				memcpy(bytetmp+pos+1,bytenr+pos,(number-pos)*sizeof(Bytenr));
				}
			if(number>0) {
				delete[] bitnr;
				delete[] bytenr;
				}
			bitnr=bittmp;
			bytenr=bytetmp;
			number++;
			bitnr[pos]=bytes*8;
			bytenr[pos]=bytes;
			calcsizes();	
			}
		return 0;
		}
	int Bitfield::rmpos(int pos) { 
		if(pos<0||pos>=number)
			return -1;
		number--;
		if(number>pos) {
			memmove(bitnr+pos,bitnr+pos+1,number-pos);
			memmove(bytenr+pos,bytenr+pos+1,number-pos);
			}
		calcsizes();
		return pos;
		}
	int Bitfield::getbit(int pos) {
			if(pos>=0&&pos<number)
				return bitnr[pos];
			return -1;
			};
	int Bitfield::chbitspos(int pos,int bits) { 
		if(pos>=0&&pos<number) {
			int byt=(bits+7)/8;
			if(bytenr[pos]<byt)
				bitnr[pos]=bytenr[pos]*8;
			else
				bitnr[pos]=bits;
			calcsizes();
			return pos;
			}
		return -1;
		}
	int Bitfield::chbytespos(int pos,int bytes) { 
		if(pos>=0&&pos<number) {
			int bitin=bytes*8;
			bytenr[pos]=bytes;
			if(bitnr[pos]>bitin)
				bitnr[pos]=bitin;	
			calcsizes();
			return pos;
			}

		return -1;
		}
	int Bitfield::fromfile(const unsigned char * const input,unsigned char *const output) const   {
		int startbit=0;
		unsigned char *outptr=output;
		const unsigned char *inptr=input;
		memset(output,'\0',bytesscreen);
		for(int i=0;i<number;i++) {
			if(bytenr[i]) {
				bitcpy(outptr,inptr,0,startbit,bitnr[i]);
				outptr+=bytenr[i];
				}
			int bij=startbit+bitnr[i];
			inptr+=bij/8;
			startbit=bij%8;
			}
		return bytesscreen;
		}
	int Bitfield::tofile(const unsigned char *const input,unsigned char *const output) {
		int startbit=0;
		unsigned char *outptr=output;
		const unsigned char *inptr=input;
		for(int i=0;i<number;i++) {
			if(bytenr[i]) {
				bitcpy(outptr,inptr,startbit,0,bitnr[i]);
				inptr+=bytenr[i];
				}
			int bij=startbit+bitnr[i];
			outptr+=bij/8;
			startbit=bij%8;
			}
		return bytesfile;
		}




	int Bitfield::calcsizes(void) const {
		bytesscreen=0;
		filebits=0;
		for(int i=0;i<number;i++) {
			filebits+=bitnr[i];
			bytesscreen+=bytenr[i]; 
			}
		bytesfile=(filebits+7)/8;
		return 0;
		}









	int  Bitfield::equal(const Filter *fil) const {
		if(!Filter::equal(fil))
			return 0;
		Bitfield *bitf=(Bitfield *)fil;
		if(number!=bitf->number)
			return 0;
		if(memcmp(bitnr,bitf->bitnr,number*sizeof(Bitnr)))
			return 0;
		if(memcmp(bytenr,bitf->bytenr,number*sizeof(Bytenr)))
			return 0;
		return 1;
		};
	int Bitfield::screen2filepos(int pos) {
		int scpos=0,fipos=0;
		for(int i=0;i<number&&scpos<pos;i++){
			scpos+=bytenr[i];
			fipos+=bitnr[i];
			}
		return fipos/8;	
		}

	int Bitfield::file2screenpos(int pos) {
		int scpos=0,fipos=0,bitpos=pos*8;
		for(int i=0;i<number&&fipos<bitpos;i++){
			scpos+=bytenr[i];
			fipos+=bitnr[i];
			}
		return scpos;	
		}
registerfilterclasspart(Bitfield) 
#ifdef EDITOR
int Editor::bitfield(void) {
	ScreenPart *super=activemode(),*act;
	act=super->getsubpart();
	Bitfield *ad=new Bitfield(1);
	if(ad->error()) {
		delete ad;
		toscreen();	
		message("Error adding add");
		return -1;
		}
	if(addfilter(ad)<0) {
		toscreen();	
		message("Added filter failted!!");
		}
	else {
		toscreen();	
		message("Filter added!!");
		}
	return 0;
	}

int Editor::changebitfield(void) {
	ScreenPart *cur=activemode(),*act=NULL,*par=NULL;
	while(act=cur->getchild()) {
		par=cur;
		cur=act;
		}
	if(par)	{
		for(int i=par->firstfilter;i<par->lastfilter;i++) {
			if(par->filters[i]->name==filtername(Bitfield)) {
				Bitfield *bit=(Bitfield *)par->filters[i];
				char ans[100];
				int act=((HasSub *)par)->activesubpart();

				sprintf(ans,"%d",bit->bitnr[act]);
				if(getanswer("Bits: ",ans,40) <0)  {
					message("Wrong answer");return -1;
					}
					
				bit->resizeitem(act,atoi(ans));
				par->calcsizes();
				activemode()->calcsizes();
				modechanged();
				return 0;

				}
			}
		message("No bitfield filter in %s",par->getname());
		return -1;
		}
	message("No parent of %s",cur->getname());
	return -1;
	}
call(bitfield)
call(bitfielder)

#endif
/*
How to manage bitfields?
Possibilities:
	1. transform Contains to bitfield

Add 	bitfield filter, which shows everything the same as usual
Add function to modify every element of this
Probleem: chbytes etc. Needs change filter
*/
