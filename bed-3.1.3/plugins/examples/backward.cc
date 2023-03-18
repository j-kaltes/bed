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
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../filter.h"
#include "../screenpart.h"
#include "../dialog.h"
#include "../screen.h"
extern char *filtername(Backward) ;
class Backward: public Filter {
	Filter *subfil;
	mutable char *convstring;
	public:
	Backward(Filter *fil) : Filter(fil->bytesscreen,fil->bytesfile) {
		subfil=fil;
		setlabel();
		}
	
	Backward(int fb,int sb,const char *conv,ScreenPart *p) : Filter(fb,sb,filtername(Backward)) {
		const char *end=strchr(conv, ':');
		if(!end) {
			err=1;
			return;
			}
Filterprocptr func=getfilterfunc(conv,end-conv);
		subfil=(*func)(sb,fb,++end,part);
	        if((err=subfil->error())) {
                       delete subfil;
                       return;
                       }

		setlabel();
		setpart(0);
		}
	Backward(Backward const &rev): Filter(rev) {
		subfil=rev.subfil->newone();
		err=subfil->err;
		setlabel();
		}
	void setlabel(void) {
		convstring=NULL;
		name=filtername(Backward);
		int len=strlen(subfil->label);
		label=new char[len+11];
		strcpy(label,"Backward(");
		memcpy(label+9,subfil->label,len);
		label[len+9]=')';
		label[len+10]='\0';
		}
	virtual int setpart(ScreenPart *p) override {
		part=p;
		return subfil->setpart(p);
		}

	virtual char *convstr(void) const override {
		char *conv=subfil->convstr();
		int lenconv=strlen(conv);
		int lenname=strlen(subfil->name);;
		if(convstring)
			delete[] convstring;
		convstring=new char[lenconv+lenname+2];
		memcpy(convstring,subfil->name,lenname);
		convstring[lenname]=':';
		memcpy(convstring+lenname+1,conv,lenconv+1);
		return convstring;	
		}
	~Backward() {
		if(subfil)
			delete subfil;
		}
	virtual Filter *newone(void) const override {
		return new Backward(*this);
		}
	virtual int chbytes(int b)  override {
		subfil->chbytesback(b);

		bytesscreen=subfil->bytesfile;
		bytesfile=subfil->bytesscreen;
		return bytesscreen;
		}
	virtual int chbytesback(int b)  override {
		subfil->chbytes(b);
		bytesscreen=subfil->bytesfile;
		bytesfile=subfil->bytesscreen;
		return bytesfile;
		}

	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const  override {
		return subfil->tofile(input,output);
		}
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override {
		return subfil->fromfile(input,output);
		}
	virtual int  equal(const Filter *fil) const override {
		if(!Filter::equal(fil))
			return 0;
		Backward *sk=(Backward  *) fil;
		if(!subfil->equal(sk->subfil))
			return 0;
		return 1;
		}
	virtual int screen2filepos(int pos)  override {
		return subfil->file2screenpos(pos);
		}

	virtual int file2screenpos(int pos)  override {
		return subfil->screen2filepos(pos); 
		}
	};
registerfilterclasspart(Backward) 



class Editor {
public:
#include "faked.h"
int backward(void) {
	ScreenPart *act=activemode();
	ScreenPart *part=act->getsubpart();
	Filter *fil;
	if((fil=part->getlastfilter())) {
		Backward *rev=new Backward(fil->newone());
		if(rev->error()) {
			message("Can't add filter");
			erefresh();
			delete rev;
			return -1;
			}
		part->rmlastfilt();
		int res=addfilter(rev);
		part->datbytes(part->getbytes());
//		part->chbytes(part->bytes);
//		part->calcsizes();
		act->calcsizes();
		modechanged();
		if(res<0){
			message("Unknown error");
			return -1;
			}
	message("Filter backwarded");
	return 0;
	}		
message("No filter to backward present");
return -1;
};
};


sethelp(backward,"Adds the backward filter.\nThis filter tries to reverse the direction of the last applied filter.")
call(backward)
