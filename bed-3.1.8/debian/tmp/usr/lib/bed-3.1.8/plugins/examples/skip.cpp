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
extern char *filtername(Skip) ;
class Skip: public Filter {
	int skip,take;
	public:
	Skip(int len,int period,int sbytes) : Filter(0,0,filtername(Skip)),
		skip(len),
		take(period) {
		label=new char[10];
		snprintf(label,10,"%d#%d",take,skip);
		chbytesback(sbytes);
		}
	Skip(int fb,int sb,const char *conv) : Filter(0,0,filtername(Skip)) {
		sscanf(conv,"%d#%d",&take,&skip);
		label=new char[10];
		snprintf(label,10,"%d#%d",take,skip);
		chbytesback(sb);
		}
	Skip(Skip const &rev): Filter(rev), skip(rev.skip), take(rev.take) {
		label= new char[10];
		strcpy(label,rev.label);
		}
	~Skip() {
		delete[] label;
		}
	virtual char *convstr(void) const  override{
		return label;
		}
	virtual Filter *newone(void) const override {
		return new Skip(*this);
		}
	virtual int chbytes(int b)  override {
		int per=take+skip;
		if(take>b||take<=0||skip<0||(b%per)){
			err=1;
			return -1;
			};
		err=0;
		bytesfile=b;
		bytesscreen=(bytesfile*take)/per;
		return bytesscreen;
		}
	virtual int chbytesback(int b)  override {
		if(take>b||take<=0||(b%take)) {
			err=1;
			return -1;
			}
		bytesscreen=b;
		bytesfile=(bytesscreen*(skip+take))/take;
		err=0;
		return bytesfile;
		}

	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const override {
		int per=skip+take;
		int hexlen=0;
		for(int oriter=0;oriter<bytesfile;oriter+=per,hexlen+=take) {
			memmove(output+hexlen,input+oriter,take);
			}
		return bytesscreen;
		}
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override {
		int per=skip+take;
		int oriter;
		oriter=0;
		for(int show=0;show<bytesscreen;oriter+=per,show+=take) {
			memcpy(output+oriter,input+show,take);
			}
		return bytesfile;
		}
	int  equal(const Filter *fil) const override {
		if(!Filter::equal(fil))
			return 0;
		Skip *sk=(Skip  *) fil;
		if(sk->skip!=skip||sk->take!=take)
			return 0;
		return 1;
		}


	virtual int screen2filepos(int pos)  override {
		int per=skip+take;
		int num=pos/take;
		int in=pos%take;
		return num*per+in;
		}

	virtual int file2screenpos(int pos)  override {
		int per=skip+take;
		int num=pos/per;
		int in=(pos%per);
		return num*take+(in<take?in:take);
		}
	};
registerfilterclass(Skip) 
#endif




#ifdef EDITOR
#define maxliner 20
#define myatoi atoi

int addskip(void) {
	int fors,bins;
	ScreenPart *super=activemode(),*act;
	act=super->getsubpart(fors,bins,super->half);
	int wlin=7,wcol=29,res=0;
	char after[maxliner],leav[maxliner];
	after[0]='1';
	after[1]='\0';
	leav[0]='1';
	leav[1]='\0';
	while(1) {
		again:
		alcsconfig(res,wlin,wcol,1,
			linernrmax("~After (num):",after,1,3,5,maxliner),
			linernrmax("~Skip  (num):",leav,3,3,5,maxliner),
			oke(5,3),
			act("~Cancel", 5,17)
			); 
		if(!res||res==4) { 
			filesup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			erefresh();
			continue;
			goto again;
			}
		int llen=0,lafter=0;
		switch(res) {
			case 4: llen=0;break;
			default: llen=myatoi(leav); lafter=myatoi(after);break;
			};
		Skip *rev=new Skip(llen,lafter,act->datbytes());
		if(rev->error()) {
			message("Can't add filter");
			erefresh();
			delete rev;
			continue ;
			}
/*
		int res =act->addfilter(rev);
		super->calcsizes();
*/
		int res=addfilter(rev);
		modechanged();
		if(res<0)		{
			filesup(); 
			message("Unknown error");
			return -1;
			}
			
		break;
		}
	filesup(); 
	return res;
	}

#endif

call(addskip)
