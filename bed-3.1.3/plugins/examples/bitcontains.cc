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
/* Problem:                                                                  */
/* datbytes()=bytes                                                          */
/* when no filters are set                                                   */
/* So both the total of memory and diskspace is bytes                        */
/* BitContain tries to make these different                                      */
/* So can't use contains functions anymore                                   */
/* A solution is to bind to BitContain a filter                                 */
/* So BitContain only intercepts add and rm subparts to modify                   */
/* And lets the rest be done by the filter                                   */
/* Mon Oct 30 22:39:46 2000                                                  */




#include "global.h"
BEDPLUGIN
#include "../dialog.h"
#include "../screen.h"
class Editor {
public:
#include "faked.h"

//s/^\([^(]*\)Editor::/\1 GeenEditor /g
 void  GeenEditor clearmessage(void);
int  GeenEditor getanswer(const char *question,char *answ,int anslen,int ansroom=5);
int  GeenEditor changebitfield(int kind) ;
int  GeenEditor searcher(int);
int  GeenEditor addtocontain(ScreenPart *part);

iscalled(bitfield)
iscalled(changebitfield)
iscalled(bitfielder)
};

#include "bitfields.h"
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../filter.h"
#include "../screenpart.h"
#include "contain.h"
#include "../hassub.h"

#include "typelist.h"
typedef short Bitnr;
//extern int bitcpy(unsigned char *output,unsigned char *input,char bitout,char bitin,int bitnr) ;

#define filterpresent(part) ((part)->filnr>=(part)->firstfilter&&(part)->filnr<(part)->lastfilter&&filters[(part)->filnr]->name==filtername(Bitfield) ) 
#define bincontype 0x1234
class BitContain: public Contains {
protected:
	int filnr;
	Bitfield *fil;
	public:
	declareparent 
	BitContain(Editor *ed):  Contains(ed),filnr(-1),fil(NULL) {
		type=bincontype;
		calcsizes();
		}
	BitContain(Editor *ed,ScreenPart *part):  Contains(ed) {
		type=bincontype;
		int byt;
		if(part) 
			byt=part->getbytes();
		else
			byt=0;

		fil=new Bitfield(byt*8,byt,part);
		fil->wantsaway=0;
		filnr=lastfilter;
		addfilter(fil);
		if(part) {
			Contains::addsubpart(part,0);
			}
		calcsizes();
		}
	BitContain(BitContain const &rev): Contains(rev)
 {
		if(filterpresent(this)) {
			filnr=firstfilter;
			fil=(Bitfield *)filters[firstfilter];
			}
		else {
			filnr=-1;fil=NULL;
			}
		}
int calcsizes(void) override{
	Contains::calcsizes();
	if(!filterpresent(this)) {
		for(int i=firstfilter;i<lastfilter;i++)
			if( filters[i]->name==filtername(Bitfield)) {
				filnr=i;
				goto found;
				}
		return bytes;
		}
	found:
	fil=(Bitfield *)filters[filnr];
	fil->extendtolen(nrsubparts);
	for(int i=0;i<nrsubparts;i++) {
		ScreenPart *sub=subpart(i);
		fil->bytenr[i]=sub->bytes;
		int bits=sub->bytes*8;
		if(fil->bitnr[i]>bits)
			fil->bitnr[i]=bits;
		}



	fil->calcsizes();
        return bytes;
	}
	
	virtual ScreenPart *newone(void) const override {
		return new BitContain(*this);
		};
	virtual int addsubpart(ScreenPart *part,int pos=-1)  override {
       		if(pos==-1)
                        pos=nrsubparts;
                if(pos>nrsubparts)
                        return -1;
		if(filterpresent(this)) {
			part->calcsizes();
			((Bitfield *)filters[filnr])->addposition(pos,part->getbytes(),nrsubparts);
			}
		return Contains::addsubpart(part,pos);
		}

	virtual ScreenPart *rmsubpart(int pos=-1)  override {
		 if(pos==-1)
				pos=nrsubparts-1;
		ScreenPart *res=Contains::rmsubpart(pos);
		if(res==this) {
			if(filterpresent(this)) {
				((Bitfield *)filters[filnr])->rmpos(pos);
				calcsizes();
				}
			}
		return res;
		}
	ScreenPart *setsubpart(ScreenPart *part,int i=-1) override { 
		ScreenPart *old=Contains::setsubpart(part,i);
		if(old) {
			if(filterpresent(this)) {
				subpart(active)->calcsizes();
				((Bitfield *)filters[filnr])->chbytespos(active,subpart(active)->bytes);
				calcsizes();
				}
			}
		return old;
		}
	virtual int chrepeat(int b) override {
		int res;
		if((res=Contains::chrepeat(b))==b) {
			if(filterpresent(this)) {
				subpart(active)->calcsizes();
				((Bitfield *)filters[filnr])->chbytespos(active,subpart(active)->bytes);
				calcsizes();
				}
			}
		return res;
		}
	virtual int chbytes(int b)  override{
		int res;
		res=Contains::chbytes(b);
		if(res>=0) {
			if(filterpresent(this)) {
				subpart(active)->calcsizes();
				((Bitfield *)filters[filnr])->chbytespos(active,subpart(active)->bytes);
				calcsizes();
				}
			}
		return res;	
		}

int  userconfig(int kind) override{	
	switch(kind) {
		case 0:return editor->bitfield();
		default: return editor->changebitfield(kind);
		};
	}
friend int Editor::changebitfield(int kind) ;
	};

parentd(Contains,BitContain) 
datatype(BitContain)




int hasbitfield(ScreenPart *cur) {
	if(!cur)
		return -2;
	for(int i=cur->firstfilter;i<cur->lastfilter;i++) 
		if(cur->filters[i]->name==filtername(Bitfield)) 
			return i;
	return -1;
	}
int Editor::changebitfield(void) {
	clearmessage();
	if(changebitfield(0)<0) {
		message("Not a bitfield");
		return -1;
		}
	return 0;
	}

int Editor::changebitfield(int kind) {
	ScreenPart *cur=activemode(),*act=NULL,*par=NULL;
	while((act=cur->getchild())) {
		par=cur;
		cur=act;
		}
	int filnr=-1;
	if(kind&&((filnr=hasbitfield(cur))>=0)) {
			if(has_super(cur,HasSub)) 
				((HasSub *)cur)->poschanged(cur->half);
		}
	else {
		if(par&&(filnr=hasbitfield(par))>=0)
			cur=par;
		else
			if((filnr=hasbitfield(cur))<0)
				return filnr;
		}

	Bitfield *bit=(Bitfield *)cur->filters[filnr];
	int active;
	char ans[100];
	if(has_super(cur,HasSub))
		active=((HasSub *)cur)->activesubpart();
	else
		active=0;
	if(active>=bit->number) {
		bit->setpart(cur);
		bit->calcsizes();
		}
	sprintf(ans,"%d",bit->bitnr[active]);
	if(getanswer("Bits: ",ans,40) <0)  {
		message("Cancelled");return -1;
		}
	if(active<bit->number)
		bit->chbitspos(active,atoi(ans));
	cur->calcsizes();
	activemode()->calcsizes();
	modechanged();
	return 0;
	}


int Editor::bitfield(void) {
	ScreenPart *sub=activemode(),*par=NULL,*tmp;
	while((tmp=sub->getchild())) {
		par=sub;	
		sub=tmp;
		}
	message("Bitfield around %s. Add more fields in the usual way.",sub->getname());
	if(par) {
		BitContain *bit= new BitContain( this,sub);
		HasSub *has=(HasSub *)par;
		has->setsubpart(bit);
		return 0;
		}
	else {
		BitContain *bit= new BitContain( this,sub);
		replacepart(bit);	
		return 0;
		}

	}
call(bitfield)
call(changebitfield)
call(bitfielder)
int Editor::bitfielder(void) {
while(1) {
int res=1;
  begindialog(10,20,6);
mvwaddstr(takescr,1,2,"Bitfield");
    saveact("~New", 3,2);
    saveact("~Change Bits", 5,2); 
    saveact("C~ancel", 7,2); 
  enddialog(res,res);
   filesup();
	switch(res) {
	case (INT_MAX-1):resized();rewriteall();break;
	case 1:return bitfield();
	case 2: return changebitfield();
	case 3: ;
	case 0:return -1;
	}
   }
};
/*
How to manage bitfields?
Possibilities:
	1. transform Contains to bitfield

Add 	bitfield filter, which shows everything the same as usual
Add function to modify every element of this
Probleem: chbytes etc. Needs change filter
*/
