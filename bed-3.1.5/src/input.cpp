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


#include <alloca.h>
#include "editor.h"
#include "screenpart.h"
#include "convert.h"
#include "screen.h"
#include "dialog.h"
#include "main.h"
#define erbij 3
//#define maxdepth 10
#define membij 0
int getlevels(ScreenPart *part) {
	int pos=part->half;
	ScreenPart *old=part,*sub;
	int forstart=0;
	int binstart=0;
	int f,b,levels=2;
	while(( sub=old->getchild(f,b,pos-forstart))&&sub!=old) {
		old=sub;
		forstart+=f;
		binstart+=b;
		levels++;
		}	
	return levels;
	}
int Editor::input(void) {
	int maxdepth=getlevels(parts[mode]);
	int upbytes=parts[mode]->bytes;
	unsigned char *uitbuf=(unsigned char *)alloca(upbytes+membij);
	int inpos=((editpos/upbytes)*upbytes);
//	unsigned char *raw;
	unsigned char *binbuf[maxdepth];
	int binstart[maxdepth];
//	ScreenPart *par[maxdepth-1],*sub;
	ScreenPart *par[maxdepth],*sub;
	char *viewpos=view.view()+inpos;
	memcpy(uitbuf,viewpos,upbytes);
//	memcpy(uitbuf,viewbuf+inpos,upbytes);
	int forstart;
	par[0]=parts[mode];
	int level=0;
	DEBUGGING("level: %s\n",par[level]->getname());
	binbuf[level]=uitbuf;
	binstart[level++]=0;

	while((par[level]=par[level-1]->getchild(forstart,binstart[level],par[level-1]->half))) {
		binbuf[level]=(unsigned char *)alloca(par[level-1]->datbytes()+membij);
		int by=par[level-1]->getbytes();
		par[level-1]->fromfilefilters( (char *)binbuf[level-1]+binstart[level-1],by,(char *)binbuf[level]);
		DEBUGGING("level: %s\n",par[level]->getname());
		level++;
		}
	sub=par[level-1];
	int maxlen=sub->maxres;
	int wlin=3,res=0;
	int datalen=maxlen*3/2+20;
	unsigned char data[datalen];
	binbuf[level]=(unsigned char *)alloca(par[level-1]->datbytes()+membij);
	int byt=sub->getbytes();
	sub->fromfilefilters( (char *)binbuf[level-1]+binstart[level-1],byt,(char *)binbuf[level]);
	sub->bin2formal(binbuf[level],(char *)data,sub->datbytes());

//	sub->fromascii(data,raw+binstart,sub->bytes);
	data[maxlen]='\0';
	char posstr[MAXFILES+3];
	OFFTYPE pos=filepos+(OFFTYPE)inpos;
	int posstrlen=snprintf(posstr,MAXFILES,numstr  ,pos); 
	if(binstart[level-1]>0)
		posstrlen+=snprintf(posstr+posstrlen,MAXFILES-posstrlen,"+%d~:",binstart[level-1]);
	else {
		strcpy(posstr+posstrlen,"~:");
		posstrlen+=2;
		}
	char databack[datalen];
	memcpy(databack,data,maxlen);
	again:
	while(1) {
		int showlen=minnum(maxlen, maxnum(Screenwidth-10-posstrlen,1)), wcol=showlen+erbij+5+posstrlen;
/* 
  {item **showformsarray;
int showformsiter=0; 
int showformsmaxar=sizeof((item* []){(showformsarray[showformsiter++]= new lineitem(posstr,(char *)data,1,1,showlen+3,datalen))})/sizeof(item *);
 if((takescr=getshowwin(wlin,wcol,(thelines-wlin)/2,(thecols-wcol)/2))) { showformsiter=0;
 showformsarray=new itemshowptr [showformsmaxar],
(showformsarray[showformsiter++]= new lineitem(posstr,(char *)data,1,1,showlen+3,datalen));
(void)0;
res=show(showformsarray,showformsmaxar,0);
for(showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];delete[] showformsarray;};}
*/

		alcsconfig(res,wlin,wcol,0,
			linernrmax(posstr,(char *)data,1,1, showlen+erbij,datalen)
			); 

		if(!res) { 
			editup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			goto again;
			}
#ifdef HAS_STRNLEN
		int newlen=strnlen((char *)data,maxlen);
#else
		int newlen=strlen((char *)data);
#endif
		if(newlen!=maxlen||memcmp(data,databack,newlen)) {
			DEBUGGING("Different; memcp data databack %s\n",data);
		//	sub->filterformat2bin((char *)data,raw+binstart,newlen);
			sub->form2bin((const char*)data,binbuf[level],newlen);
			DEBUGGING("level %d:%s\n",level,binbuf[level]);
			int iter;
			for(iter=level;iter>0;iter--) {
				int dbyt=par[iter-1]->datbytes();
				par[iter-1]->tofilefilters((char *)binbuf[iter],dbyt,(char *)binbuf[iter-1]+binstart[iter-1]);
//				DEBUGGING("level %d:%s\nbinstart=%d\n",iter-1,binbuf[iter-1]+binstart[iter-1],binstart[iter-1]);
				DEBUGGING("level %d\n",iter-1);	
				}
	DEBUGGING("iterend %d\n",iter);	
	DEBUGGING("uitbuf=%s\nbuffer+inpos=%s\ninpos=%d\nupbytes=%d"
,uitbuf,viewpos,inpos,upbytes);
			if(memcmp(uitbuf,viewpos,upbytes)) {
				DEBUGGING("Different; changestr ");
				changed=1;
				changestr(inpos,uitbuf,upbytes);
				cursorpos();
				}
			}
		break;
		}
	editup(); 
	return res;
	}





/*
			sub->tofilefilters((char *)temp,(char *)to);
			if(sub!=parts[mode]) {
				parts[mode]->tofilefilters( (char *)raw,(char *)uitbuf);
				}
*/
