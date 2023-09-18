#include "defines.h"
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
#include <string.h>
#include <alloca.h>
#include "filter.h"
#include "myalloc.h"
#include "editor.h"
#include "screenpart.h"
typedef char *charptr ;
/*
At firstfilter stands the filter nearest to the file,
at lastfilter-1 the filter nearest to screen */

char nothing[]="x";
int filtersmaxlen=0,filtersnr=0;
char **filtersnames=NULL;
Filterprocptr  *filtersfuncs=NULL;
//Filter *(**filtersfuncs)(int,int,const char*,ScreenPart *)=NULL ;

Filterprocptr  getfilterfunc(const char *name,int len) {
//Filter *(*getfilterfunc(const char *name,int len)) (int,int,const char *,ScreenPart*) {
	int i;
	for(i=0;i<filtersnr;i++)
		if(!memcmp(name,filtersnames[i],len)&&!filtersnames[i][len])
			return filtersfuncs[i];
	return NULL;
	};
//char*	initfilter(const char *str, Filter *(*func)(int,int,const char *,ScreenPart *)) {
char*	initfilter(const char *str, Filterprocptr func) {
		int len=strlen(str);
		if(filtersmaxlen>=filtersnr) {
			filtersmaxlen=(filtersmaxlen+10)*2;
			char **tmp=filtersnames;
			filtersnames=new charptr[filtersmaxlen];
			if(tmp) {
				memcpy(filtersnames,tmp,filtersnr*sizeof(char*));
				delete[] tmp;
				}
			Filter *(**tmpfunc)(int,int,const char*,ScreenPart *) =filtersfuncs;
			filtersfuncs=new Filterprocptr[filtersmaxlen];
			if(tmpfunc) {
				memcpy(filtersfuncs,tmpfunc,filtersnr*sizeof(Filter *(*)(int,int,char*)));
				delete[] tmpfunc;
				}
			}
		filtersnames[filtersnr]=new char[len+1];
		memcpy(filtersnames[filtersnr],str,len+1);
		filtersfuncs[filtersnr++]=func;
		return filtersnames[filtersnr-1];
		}
//s/=\(.*\);/(\1),/g
	int Filter::error(void) {
		return err;
		}

static  char noconvstr[]="";
	char *Filter::convstr(void) const {
		return noconvstr;
		}
	int  Filter::equal(const Filter *fil) const {
		if(strcmp(fil->label,label))
			return 0;
		if(fil->bytesscreen!=bytesscreen)
			return 0;
		if(fil->bytesfile!=bytesfile)
			return 0;
		return 1;
		}

	int Filter::setpart(ScreenPart *p) {
		part=p;
		return 0;
		}
	Filter::~Filter() {}

/*If within a datatype an byte offset translates in a offset in what is
written to the screen, this function should make
*/
	int Filter::screen2filepos(int pos) {
		return (pos*bytesfile)/bytesscreen;
		}
	int Filter::file2screenpos(int pos) {
		return (pos*bytesscreen)/bytesfile;
		}
int ScreenPart::byte2screen(int pos) {
	int newpos=fromfilefilterspos(pos);
	return byteoff2screen(newpos);
	}
int ScreenPart::screen2byte(int pos) {
	int newpos=screen2byteoff(pos);
	if(newpos>=0)
		return tofilefilterspos(newpos);
	else 
		return newpos;
	}
int ScreenPart::fromfilefilterspos(int pos) const {
	int newpos=pos;
	for(int i=firstfilter;i<lastfilter;i++	) 
		newpos=filters[i]->file2screenpos(newpos);
	return newpos;
	}
int ScreenPart::tofilefilterspos(int pos) const {
	int newpos=pos;
	for(int i=(lastfilter-1);i>=firstfilter;i--	) 
		newpos=filters[i]->screen2filepos(newpos);
	return newpos;
	}
int ScreenPart::fromfilefilters(const char *const input,int &inplen,char *output) const {
	int len=inplen;
	const char *ptr=input;
	for(int i=firstfilter;i<lastfilter;i++	) {
		int byt=filters[i]->bytesscreen;
		char *tmp=(char *)alloca(byt);
		int uit=filters[i]->fromfile((unsigned char *)ptr,len,(unsigned char *)tmp);
		if(i==firstfilter)
			inplen=len;
		len=uit;
		ptr=tmp;
		}
	if(output==ptr)
		return len;
	memmove(output,ptr,len);
	return len;
	}
int ScreenPart::tofilefilters(char *input,int &inplen,char *output) const {
	char **tussen=(char **)alloca((lastfilter-firstfilter+1)*sizeof(char *));
	tussen[firstfilter]=output;
	for(int i=firstfilter;i<(lastfilter-1);i++	) {
		int byt=filters[i]->bytesscreen;
		tussen[i+1]=(char *)alloca(byt);
		filters[i]->fromfile((unsigned char *)tussen[i],filters[i]->bytesfile,(unsigned char *)tussen[i+1]);
		}
	char *ptr=input;
	int len=inplen;
	for(int i=lastfilter-1;i>=firstfilter;i--	) {
		int uit=filters[i]->tofile((unsigned char *)ptr,len,(unsigned char *)tussen[i]);
		if(i==(lastfilter-1))
			inplen=len;
		len=uit;
		ptr=tussen[i];
		}
	if(output==ptr)
		return len;
	memmove(output,ptr,len);
	return len;
	}

int Editor::addfilter(Filter *fil) {
	int fors,bins;
	ScreenPart *sub=parts[mode]->getsubpart(fors,bins,parts[mode]->half);
	sub->addfilter(fil);
	int res;
	if( (res=parts[mode]->calcsizes())<=0) {
		sub->rmlastfilter();
		return -1;
		}
	return res;
	}

Filter *ScreenPart::getlastfilter(void) {
	if(lastfilter>firstfilter) {
		return filters[lastfilter-1];
		}
	else
		return NULL;
	}
char *ScreenPart::lastfiltername(void) {
	if(lastfilter>firstfilter) {
		return filters[lastfilter-1]->label;
		}
	else
		return NULL;
	}

int canresize(ScreenPart *sub,int negbyt) ;
int ScreenPart::rmlastfilt(char *name) {
	if(lastfilter>firstfilter&&(!name||!strcmp(name,filters[lastfilter-1]->label))) {
		if(filters[lastfilter-1]->wantaway()) {
			lastfilter--;
			int wasbytes=bytes;
			int byt=filters[lastfilter]->bytesscreen;
			if(screenupdatefilters( byt)>0&&canresize(this,bytes-wasbytes)>=0) {
					
				delete filters[lastfilter];
				mklabel();
				editor->updated=0;
				return 0;
				}
			lastfilter++;
			screenupdatefilters( byt);
			return -2;
			}
		else
			return -3;
		}
	return -1;
	}
int ScreenPart::rmlastfilter(void) {
	int fors,bins;
	ScreenPart *sub=getsubpart(fors,bins,half);
	if(sub)  {

		 int res= sub->rmlastfilt();
		 calcsizes();
		 mklabel();
		 return res;
		 }
		
	return -1;
	}
int Editor::rmlastfilter(void) {
	int res;
	if(( res=parts[mode]-> rmlastfilter() )==-1) {
		message("No filters set");
		return res;
		}
	switch(res) {
		case -2: {
			message("Can't remove filter. Try change bytes before removing.");
			return res;
			};
		case -3: {
			message("Can't remove filter, not wantaway()");
			return res;
			}
		}
	return 0;
	}

int ScreenPart::allocfilters(int nr) {
	{
	if(maxfilter<(lastfilter+nr)) {
		int oldmaxfilter __attribute__ ((unused)) =maxfilter;
		if(maxfilter<2)
			maxfilter=2;
		maxfilter*=(2*(nr+maxfilter)/maxfilter);
		filters=myrealloc(Filter **,filters,Filter *,maxfilter,oldmaxfilter);
		}
	}
	return maxfilter;
	}
int ScreenPart::addfilter(Filter *fil) {
	allocfilters(1);
	fil->setpart(this);
	filters[lastfilter++]=fil;
	int byt= filters[lastfilter-1]->bytesscreen;
	if((screenupdatefilters(byt))>0) {
		mklabel();
		editor->updated=0;
		return lastfilter;
		}
	lastfilter--;
	delete filters[lastfilter];
	bytes=screenupdatefilters(byt);
	return -1;
	}

int ScreenPart::setfilter(Filter *fil) {
	allocfilters(1);
	fil->setpart(this);
	filters[lastfilter++]=fil;
	return lastfilter;
	}
int ScreenPart::copyfiltersafter(ScreenPart const *part) {
	int nrf=part->lastfilter-part->firstfilter;
	allocfilters(nrf);
	for(int i=0;i<nrf;i++) {
		filters[lastfilter]=part->filters[firstfilter+i]->newone();
		filters[lastfilter++]->setpart(this);
		}
	return lastfilter;
	}
/*
int ScreenPart::copyfiltersafter(ScreenPart *part) {
	int nrf=part->lastfilter-part->firstfilter;
	allocfilters(nrf);
	for(int i=0;i<nrf;i++) {
		filters[lastfilter]=part->filters[firstfilter+i]->newone();
		filters[lastfilter++]->setpart(this);
		}
	int byt= bytes;
	if(screenupdatefilters(byt)>0) {
		mklabel();
		editor->updated=0;
		return lastfilter;
		}
	for(int i=0;i<nrf;i++) 
		delete filters[--lastfilter];
	screenupdatefilters(byt);
	return -1;
	}
*/
#define writefilter	len+=sprintf(buf+len,"%s(%d,%d,\"%s\")",filters[i]->name,filters[i]->bytesfile,filters[i]->bytesscreen,filters[i]->convstr());
int ScreenPart::filtersstring(char *buf) {
	int len=0;;
	int i=firstfilter;
	if(i<lastfilter)
		writefilter;
	i++;
	for(;i<lastfilter;i++) {
		buf[len++]=',';
		writefilter;
		}
	return len;
	}

int ScreenPart::copyfiltersbefore(ScreenPart *part) {
	int nrf=part->lastfilter-part->firstfilter;
	allocfilters(nrf);
	memmove(filters+firstfilter+nrf,filters+firstfilter,(lastfilter-firstfilter)*sizeof(Filter *));
	for(int i=0;i<nrf;i++) {
		filters[i+firstfilter]= part->filters[i+part->firstfilter]->newone();
		filters[i+firstfilter]->setpart(this);
		}
	int byt= bytes;
	if(screenupdatefilters(byt)>0) {
		lastfilter+=nrf;
		mklabel();
		editor->updated=0;
		return lastfilter;
		}
	for(int i=0;i<nrf;i++)
		delete filters[i+firstfilter];
	memmove(filters+firstfilter,filters+firstfilter+nrf,(lastfilter-firstfilter)*sizeof(Filter *));
	screenupdatefilters(byt);
	return -1;
	}
int ScreenPart::filebytes(int b) {
	int byt=b,res;
	for(int i=firstfilter;i<lastfilter;i++	) {
		if((byt=filters[i]->chbytes(byt))<0)
			return -1;
		}
	if((res=chbytes(byt))<1)
		return res;
	return byt;
	}
int ScreenPart::screenupdatefilters(int byt) {
	for(int i=(lastfilter-1);i>=firstfilter;i--) {
		if((byt=filters[i]->chbytesback(byt))<0)
			return -1;
		filters[i]->err=0;
		}
	if(byt>0&&bytes!=byt) {
		bytes=byt;
		editor->shouldreinit=1;
		}
	return byt;
	}
int ScreenPart::equal(const ScreenPart *part) const {
	if(strcmp(part->getname(),getname()))
		return 0;
	if(bytes!=part->bytes)
		return 0;
	if(type!=part->type)
		return 0;
	if(firstfilter!=part->firstfilter)
		return 0;
	if(lastfilter!=part->lastfilter)
		return 0;
	for(int i=firstfilter;i<lastfilter;i++)
		if(!filters[i]->equal(part->filters[i]))
			return 0;
	return 1; 
	}
int ScreenPart::screenbytes(int b) {
	int res;
	if((res=chbytes(b))<1) {
		return res;
		}
	if(b==-1) {
		return 1;
		}
	return screenupdatefilters( res) ;
	}

#include "reverse.h"

registerfilterclass(Reverse) 
int ScreenPart::addreverse(int  len, int un) {
	Reverse *rev=new Reverse(len,un);
	return addfilter(rev);
	}
int ScreenPart::reverse(void) {
	Reverse *rev=new Reverse(datbytes(),1);
	if(lastfilter>firstfilter) {
		 if(rev->equal(filters[lastfilter-1])) {
			rmlastfilt();
			delete rev;
			return 0;
		 	}
		 }
	addfilter(rev);
	return 0;
	}
int addsubbin(ScreenPart *super,ScreenPart *sub,unsigned char *bin,unsigned char *subbin,int len) {
	if(super==sub) {
	    memmove(bin,subbin,len);	
	    return len;
	    }
	int forstart,binstart;
	ScreenPart *child=super->getchild(forstart,binstart,super->half);
	if(!child)
		return -1;
	int flen=super->getbytes();
	char nbin[super->datbytes()+20];
	int fillen=super->fromfilefilters((char *)bin,flen,nbin); 
	int nlen=addsubbin(child,sub,(unsigned char *)nbin+binstart,subbin,len);
	if(nlen>0) {
		super->tofilefilters(nbin,fillen,(char *)bin); 
		}
	return nlen;
	}
