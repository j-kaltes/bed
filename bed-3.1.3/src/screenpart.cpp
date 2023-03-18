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
#include <features.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdarg.h>
#include "convert.h"
#include "editor.h"
#include "readconfig.h"
#include "datatypes.h"
#include "filter.h"
#include "visuals.h"

char * reverse(const char *const in, char *const out, const int len) {
	int mid=(len/2);
	for(int i=0;i<mid;i++) {
		char ch=in[i];
		out[i]=in[len-i-1];
		out[len-i-1]=ch;
		}
	if((mid*2)<len) {
		out[mid]=in[mid];
		}
	return out;
	}

char * reverse(const char *const in, char *const out,const  int len,const short unit) {
	char *temp=(char *)alloca(unit);
	int mid=len/2;
	for(int i=0;i<mid;i+=unit) {
		memmove(temp,in+i,unit);
		memmove(out+i,in+len-i-unit,unit);
		memmove(out+len-i-unit,temp,unit);
		}
	if((mid*2)<len) {
		out[mid]=in[mid];
		}
	return out;
	}

inline char * reverse(const unsigned char  *const in, const unsigned char *const out, const int len,const short unit) {
		return reverse((char *)in,(char *)out,len,unit);
		}
inline char * reverse(const unsigned char * const in, unsigned char *const out, const int len) {
		return reverse((char *)in,(char *)out,len);
		}
		
int &ScreenPart::nrx(void) {return editor->nrx;};
int &ScreenPart::nry(void) {return editor->nry;};

bool Editor::searchhit(int pos) {
	OFFTYPE off=filepos+pos;
	if(off>=standout1&&off< standout2)
		return true;
	return false;
	}
void ScreenPart::troggleselect(int pos) {
if(editor->searchhit(pos)) {
	if(editor->selected(editor->filepos+pos))
		wattrset(editor->editscreen,UNSELATTR);
	else
		wattrset(editor->editscreen,SELSELATTR);
	}
else {
	if(editor->selected(editor->filepos+pos))
			SELECTIEON;
		else
			normaal(editor->editscreen);
	}
}
#define unitsize (maxres+apart)
int ScreenPart::partwidth(void) {return ( (unitsize)*(nrx()/bytes)+spaceafter);};
inline int ScreenPart::oldpos(void) {
					return (editor->oldpos<editor->view.getshown()?editor->oldpos:0);
					};


/*
			if(part->left>=(visualoffset+visualwidth))
				goto leave;
			if((part->left +(part->nrx()*(part->maxres+part->apart))) <visualoffset)

There should be no half units bytes start screen and datatype;
If startscreen is inside this part
And the first visalbe unit doesn't start at offset 0, make it start at
offset zero;

		if(vis>part->left&&vis<(part->left+(part->nrx()*(part->maxres+part->apart)))) {
			int dif=((vis-part->left)/(part->maxres+part->maxres))*(part->maxres+part->apart);
			vis=part->left+dif;
			}
*/
int Editor::makevisible(ScreenPart *part,int pos) {
	int x,y;
	part->postoxy(pos,x,y);
	if(part->half<part->maxres)
		x+=part->half;
	else
		x+=part->maxres-1;	
	int lef=leftvis(part)-x;
	int vis= visualoffset;
	int unit= visualunit;
	if(lef>0) {
		vis-=(((lef/(unit))+1)*unit);
		if(vis<=(parts[0]->left)&&vis<visualwidth)
			vis=0;
		}
	else {
		lef=x-rightvis(part)+1;
		if(lef>0) {
			vis+=(((lef/(unit))+1)*unit);
			}
		}
	
	int dif=part->left-vis;
	if(dif>0&&mode>0&&dif<=parts[mode-1]->spaceafter)
		vis=part->left;
/*
	if(vis>0&&mode>0) {
		if(vis<parts[mode]->left)
	{
		ScreenPart *partl=parts[mode-1];
		int unitl=partl->maxres+partl->apart;
		int dif=((vis-partl->left+unitl-1)/(unitl))*(unitl);
		vis=partl->left+dif;
		if((parts[mode]->left-vis)<(partl->maxres+partl->spaceafter))
			vis=parts[mode]->left;
		}
		}
*/
	if(vis!=visualoffset) {
		visualoffset=vis;
		return -1;
		}
	return x;
	}

int  partaddch(ScreenPart *part,int ch) {
	int x;
	int y __attribute__((unused));
	WINDOW *win=part->editor->editscreen;
	getyx(win,  y,  x);
	if( x <(part->left-part->editor->visualoffset) ||x>(part->left+part->partwidth()-part->editor->visualoffset))
		return -1;
	waddch(win,ch);
	return x;
	}

int	ScreenPart::mkfilterstr(char *str,int max) {
	int len=0;
	for(int i=firstfilter;i<lastfilter;i++) {
		len+=snprintf(str+len,max-len," %s",filters[i]->label);
		}
	return len;
	}
void ScreenPart::mklabel(void) {
	if(!userlabel) {
		int len=0;
		len+=snprintf(label+len,maxlabel-len,"%s%s %d",getname(),modename(type),bytes);
		for(int i=firstfilter;i<lastfilter;i++) {
			len+=snprintf(label+len,maxlabel-len," %s",filters[i]->label);
			}
		}
	}

int ScreenPart::movepos(int pos) {
	int x,y;
	postoxy(pos,x,y);
	if(offvisual(x))
		return -1;
	wmove(editor->editscreen,top+y,left+x-editor->visualoffset);
	return x;
	}


int ScreenPart::datbytes(void) const  {
		if(lastfilter>firstfilter)
			return filters[lastfilter-1]->bytesscreen;
		else
			return bytes;
		}
int ScreenPart::getbytes(void) const {
	if(lastfilter>firstfilter)
		return filters[firstfilter]->bytesfile;
	else
		return bytes;
	}

/*
	Editor *editor;
	ScreenPart *contain;
	int left,top,spaceafter,maxres,apart,bytes,half,oldhalf;
	DataType type;
	*/
//	s/=\([^;]*\);/(\1),/g
	ScreenPart::ScreenPart(ScreenPart const &part):editor(part.editor),contain(NULL),left(part.left),top(part.top),spaceafter(part.spaceafter),maxres(part.maxres),apart(part.apart),bytes(part.bytes),half(part.half),oldhalf(part.oldhalf),type(part.type) ,userlabel(NULL), filters(NULL),firstfilter(0),lastfilter(0),maxfilter(0) {
		if(part.userlabel) {
			int len = strlen(part.userlabel)+1;
			userlabel=myallocar(char,len);
			memcpy(userlabel,part.userlabel,len);
			strcpyn(label,userlabel,len);
			}
		copyfiltersafter(&part);
		}
	ScreenPart::ScreenPart( Editor *ed,int max,int b,int len,int x1, int y1, int a):editor(ed), contain(NULL) ,
		left(x1),top(y1),
		spaceafter(1),
		maxres(max),
		apart(a),
		bytes(len),
		half(maxres),
		oldhalf(maxres-1),
		type((DataType)b),
		userlabel(NULL),
//		name=ScreenPart::thename;
		filters(NULL),
		firstfilter(0),lastfilter(0),maxfilter(0) {
		label[0]=0;
		}
 

	ScreenPart::~ScreenPart() {
		if(userlabel)
			myfree(userlabel);
		for(int i=firstfilter;i<lastfilter;i++) {
			delete filters[i];
			}
		if(filters)
			delete[] filters;
		}

void ScreenPart::unittoscreen(int pos) {
	int start=(pos/bytes)*bytes;
	troggleselect(start);
	if(oppos(start)>=0) {
		int inx=start%nrx();
		if(inx<(nrx()-bytes)) {
			for(int i=0;i<apart;i++)
				if(partaddch(this,' ')<0)
					return;
			}
		}
	}
#undef bijdoen 
#define bijdoen 100
void ScreenPart::printchar(int pos) {
	int start=(pos/bytes)*bytes;
	 oppos(start);
	}




int partmove(ScreenPart *part,int y,int x)  {
	return wmove(part->editor->editscreen,part->top+y,part->left+x-part->editor->visualoffset);
	}

int ScreenPart::putline(int y) {
	int pos;
	long last;
	last=minnum((y+1)*nrx(),editor->view.getshown());
	for(pos=y*nrx();pos<(int)last;pos+=bytes) {
		troggleselect(pos);
		if(oppos(pos)>=0&&(pos<(last-bytes))) {

#ifdef LINESBETWEENITEMS
		wattrset(editor->editscreen,BETWEENFIELDSSATTR);
#endif
#ifndef NOBACKGROUNDBETWEENITEMS
		for(int i=0;i<apart;i++) 
			if(partaddch(this,' ')<0) 
				break;
#endif

			}
		}
	
	return 0;
	}


inline int ScreenPart::oppos(int start) {
#ifdef DEBUGON
	if(start>=editor->view.getshown())
		DEBUGGING("\7oppos %d>%ld\n",start,editor->editpos);
#endif
	int x,y;
	postoxy(start,x,y);
	if((x+maxres)<=leftvis(this)||(x>=rightvis(this))) {
		if((x+maxres+apart)>leftvis(this)) {
			int endstr=minnum(maxres,rightvis(this)-x);
			partmove(this,y,x+endstr);
			return 0;
			}
		return -1;
		}
	char dec[maxres+1+bijdoen];
	OFFTYPE len=editor->mem.filesize-editor->filepos-start;
	if(len<bytes) {
		if(len>0) {
			dec[0]='X';
			memset(dec+1,' ',maxres-1);
			}
		else
			memset(dec,' ',maxres);
		}
	else {
		bin2f((unsigned char *)editor->view.view()+start, dec, bytes);
		}
	int off=maxnum(leftvis(this)-x,0);
	int endstr=minnum(maxres,rightvis(this)-x);
	partmove(this,y,x+off);
	waddnstr(editor->editscreen,dec+off,endstr-off);
	return start;
	}
int ScreenPart::oppos(int pos,int size) {
	int start=(pos/bytes)*bytes;
	int end=((pos+size+bytes-1)/bytes)*bytes;
	for(int i=start;i<end;i+=bytes)
		oppos(i);
	return end;
	}
int ScreenPart::rewritepart(int size) {
	int lastpos,pos=0,newpos, maxy=size/nrx(), maxlast=size%nrx(),y;
	int start=maxnum(editor->visualoffset-left,0);
	int startbyte=(start/(maxres+apart))*bytes ;
	int end=minnum(editor->visualwidth+editor->visualoffset,(nrx()*(long long)(maxres+apart))+left)-left;
	int endbyte=minnum(((end+maxres+apart-1)/(maxres+apart))*bytes,nrx());
	for(y=0;y<maxy;y++) {
		pos=y*nrx();
//		lastpos=nrx()+pos;
		lastpos=endbyte+pos;
		pos+=startbyte;
		for(;pos<lastpos;pos+=bytes) {
			troggleselect(pos);
			newpos=oppos(pos);
			if(newpos>=0&&((pos<(lastpos-bytes))||endbyte<nrx())) {
#ifdef LINESBETWEENITEMS
				wattrset(editor->editscreen,BETWEENFIELDSSATTR);
#endif
#ifndef NOBACKGROUNDBETWEENITEMS
				for(int i=0;i<apart;i++)
					partaddch(this,' ');
#endif
				}
			}
		}
	pos=maxy*nrx();
	lastpos=minnum(maxlast,endbyte)+pos;
	pos+=startbyte;
	if(pos<lastpos) {
		for(;pos<lastpos;pos+=bytes) {
				troggleselect(pos);
				newpos=oppos(pos);
				if(newpos>=0&&pos<(lastpos-bytes)) {
	#ifdef LINESBETWEENITEMS
					wattrset(editor->editscreen,BETWEENFIELDSSATTR);
	#endif
	#ifndef NOBACKGROUNDBETWEENITEMS
					for(int i=0;i<apart;i++)
					  partaddch(this,' ');
	#endif
					 }
			}
		}
		
	return 0;
	}
void ScreenPart::cursorpos(int pos) {
		int was=(oldpos()/bytes)*bytes;
		troggleselect(was);
		oppos( was);
		if(half<maxres) {
				int x,y;
				unsigned char ch;
				postoxy(pos,x,y);
				if(!offvisual(x+half)) {
					partmove(this,y,x+half);
					ch=winch(editor->editscreen)& A_CHARTEXT;
					CURSORON;
					partmove(this,y,x+half);
					waddch(editor->editscreen,ch&0xff );
					CURSOROFF;
					}
				poschanged(half);
				}


		else
			{
				CURSORONPAS;
				int start=(pos/bytes)*bytes;
				oppos( start);
				CURSOROFF;
			};
		
	}

int ScreenPart::nocursor(void) {
	int was=(oldpos()/bytes)*bytes;
	troggleselect(was);
	oppos( was);
	return 0;
	}
int ScreenPart::newchar(int key) { 
	if(!isel(key))
		return -1;
	return 1;
	}
int ScreenPart::addkey(unsigned char *bin,int binlen, int key, int pos ) {
	char form[maxres+1+bijdoen];
	if(bin2formal(bin,form,binlen))
		return -1;
	form[pos]=key;
	int flen=maxres;
	if(form2bin(form,bin,flen)<=0)
		return -1;
	return 0;
	}
int ScreenPart::addchar(int key) {
DEBUGGING("addchar %d ",key);
	if(!isel(key))
		return -1;
	int start=(editor->editpos/bytes)*bytes;
	if((start+bytes+editor->filepos)>editor->filesize())
		return -1;
	int hexlen=datbytes();
	unsigned char hex[hexlen+1+bijdoen];
	int fbyt=getbytes();
	fromfilefilters((char *)editor->view.view()+start,fbyt,(char *)hex);
	if(addkey(hex,hexlen,key,half))
		return -1;
	if(lastfilter>firstfilter) {
		unsigned char filebyte[bytes+1];
		memmove((char *)filebyte,(char *)editor->view.view()+start,bytes);
		tofilefilters((char *)hex,hexlen,(char *)filebyte);
	#ifdef SMALLONDO
		int beg,end;
		for(beg=0;beg<bytes;beg++)
			if(filebyte[beg]!=editor->view.view()[start+beg])
				break;
		
		for(end=bytes-1;end>=0;end--)
			if(filebyte[end]!=editor->view.view()[start+end])
				break;
		end++;
		editor->changestr(start+beg,filebyte+beg,end-beg);
	#else
		editor->changestr(start,filebyte,bytes);
	#endif
		}
	else
		editor->changestr(start,hex,bytes);
	editor->nextfast();
DEBUGGING("ret ");
	return 1;
	}
	
	
int ScreenPart::backchar(void) {
	if(!half) {
		half=maxres-1;
		return 0;
		}
	half--;	
	return 1;
	};
int ScreenPart::nextchar(void) {
	half=(half+1)%maxres;
	return half;
	};


int ScreenPart::select(void) {
	if(half<0||half>=maxres) {
		if(oldhalf<0||oldhalf>=maxres)
			half=maxres-1;  
		else
			half=oldhalf;  
		}
	if(editor->makevisible(this,editor->editpos)<0)
		editor->updated=0;
	else
		editor->cursorpos(); 
	return 0;
	}; 

int ScreenPart::bin2f(unsigned char *from, char *to, int len) {
	int sbyt=datbytes();
	if(lastfilter>firstfilter) {
		unsigned char temp[sbyt+bijdoen];
		int fbyt=getbytes();
		fromfilefilters((char *)from,fbyt,(char *)temp);
		return bin2formal(temp,(char *)to,sbyt);
		}
	else
		return bin2formal(from,(char *)to,sbyt);
	}	
/*
 int ScreenPart::f2bin(char *from, unsigned char *to, int len) {
 	int ret;
	int fbyt=datbytes();
	unsigned char temp[fbyt+bijdoen];
	fromfilefilters((char *)to,(char *)temp);
	ret=formal2bin((char*)from,(unsigned char *)temp,maxres);
	tofilefilters((char *)temp,(char *)to);
	return ret;
	}
*/

int ScreenPart::filterformat2bin(const char *from, unsigned char *to, int len) {
		int varlen=len;
		filformat2bin(from, to, varlen);
		return varlen;
		}

 int ScreenPart::filformat2bin(const char * const from, unsigned char *to, int &len) const{
	if(lastfilter>firstfilter)	{
		int retin;
		int fbyt=datbytes();
		int byt=getbytes();
		unsigned char temp[fbyt+bijdoen];
		fromfilefilters((char *)to,byt,(char *)temp);
		retin=form2bin((char*)from,(unsigned char *)temp,len);
		int retuit=retin;
		int uit=tofilefilters((char *)temp,retuit,(char *)to);
		len=(retuit*len/retin);
		return uit;
		}
	else {
		return form2bin((char*)from,(unsigned char *)to,len);
		}
	}
int ScreenPart::search2bin(const char *const formal, unsigned char *bin, const int len,int binmax) const {
	const int startbinmax=binmax-bytes+1;
	int b=0;
	for(int f=0,took,byt;f<len&&b<startbinmax;b+=byt) {
		while(!isel(formal[f]))
			if(++f>=len)
				return b;
		took= minnum(maxres,len-f);
		if((byt=filformat2bin(formal+f, bin+b, took))<=0)
			return b;
		f+=(took+apart);
		}
	return b;
	}

/*
int ScreenPart::makeascii(unsigned char *bin, unsigned char *formal, int len)  {
	int ap=maxres+apart;	
	int b=0,err=0;
	for(int f=0;f<len;f+=(ap),b+=bytes) {
		if(f2bin((char *)formal+f, bin+b, maxres)<0)
			err=-1;	
		}
	return b;
	}
*/
int ScreenPart::bin2search(unsigned char *bin,  char *formal, int len)  {
	int ap=maxres+apart;	
	int f=0, err=0;
	for(int b=0;b<=(len-bytes);f+=(ap),b+=bytes) {
	 	if(bin2f(bin+b, formal+f, bytes)<0)
	 		err=-1;
		for(int i=f+maxres;i<(f+ap);i++)
			formal[i]=' ';
//			formal[i]='\t';
		}
/*	if(err<0)
		return err;
		*/
	return f;
	}
		
ScreenPart *ScreenPart::getchild(int &forstart,int &binstart,int pos) {
	return NULL;
	}
ScreenPart *ScreenPart::getchild(void) {
	int forstart,binstart;
	return getchild(forstart,binstart,half);
	}

ScreenPart *ScreenPart::getsubpart(void) {
	int forstart,binstart;
	return getsubpart(forstart,binstart,half);
	}
ScreenPart *ScreenPart::getsuperpart(void) {
	ScreenPart *par=NULL,*cur=this;
	while((par=cur->getparent()))
		cur=par;
	return cur;
	}
ScreenPart *ScreenPart::getparent(ScreenPart *start) {
	return contain;
	}
/*
ScreenPart *ScreenPart::getparent(ScreenPart *start) {
	ScreenPart *old=start,*sub;
	int pos=start->half;
	int forstart=0;
	int binstart=0;
	int f,b;
	while(old&&(sub=old->getchild(f,b,pos-forstart))!=this) {
		old=sub;
		forstart+=f;
		binstart+=b;
		}
	return old;
	}
*/
ScreenPart *ScreenPart::getsubpart(int &forstart,int &binstart,int pos) {
	ScreenPart *old=this,*sub;
	forstart=0;
	binstart=0;
	int f,b;
	while(1) {
		sub=old->getchild(f,b,pos-forstart); 
		if(!sub||sub==old)
			break;
		old=sub;
		forstart+=f;
		binstart+=b;
		}
	return old;
	}

int ScreenPart::calcsizes(void)  {
	return screenupdatefilters(datbytes());
	}
char *ScreenPart::getconvstr(void) {
	static char non[]="";
	return non;
	}
int ScreenPart::setconvstr(char *str) {
	return 0;
	}
int ScreenPart::userconfig(int kind) {
	return editor->partconfig(this,kind);
	};
int ScreenPart::getbase(void) {return type;};
char *ScreenPart::takeuserlabel(void) {return userlabel;};
//struct parent ScreenPart::desc = { screenpartname,NULL};
int ScreenPart::sizelabels(int kind,int nr,...){
	va_list ap;
	va_start(ap,nr);
int ret= editor->sizelabels(this,kind,nr, ap,true) ;
	va_end(ap);
return ret;
	}
int ScreenPart::sizelabelsnl(int kind,int nr,...){
	va_list ap;
	va_start(ap,nr);
int ret= editor->sizelabels(this,kind,nr, ap,false) ;
	va_end(ap);
return ret;
	}
int Parent::isparent(const char *nam) const {
	if(name==nam)
		return 1;
	if(paren)
		return paren->isparent(nam);
	return 0;
	}
//char ScreenPart::thename[]="ScreenPart";
 class Parent ScreenPart::desc=Parent("ScreenPart",NULL);
/*
isel
maxres
bytes
formal2bin
bin2formal
*/
