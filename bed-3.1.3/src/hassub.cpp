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
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "editor.h"
#include "dialog.h"
#include "screen.h"
#include "hassub.h"
parentd(ScreenPart,HasSub) 
	HasSub::HasSub( Editor *ed,int nr,int act): ScreenPart( ed,0, 0) {
	 	nrsubparts=nr;
	 	active=act;
		};	
	 HasSub::HasSub ( HasSub const &con):ScreenPart(con) {
//		name=con.name;
	 	nrsubparts=con.nrsubparts;
	 	active=con.active;
		};
	HasSub::~HasSub() {
		}




int HasSub::addkey(unsigned char *bin,int binlen, int key, int pos ) {
		int forstart,binstart;
		ScreenPart *sub=getchild(forstart,binstart,pos);
		if(!sub||sub==this) {
			return -1;
			}
		if(sub->firstfilter<sub->lastfilter) {
			int datbyt= sub->datbytes();
			char tmp[datbyt+2];
			int fbyt=sub->getbytes();
			sub->fromfilefilters((char * )bin+binstart,fbyt,tmp) ;
			sub->addkey((unsigned char *)tmp,datbyt,key,pos-forstart);
			sub->tofilefilters(tmp,datbyt,(char *)bin+binstart) ;
			}
		else
			sub->addkey(bin+binstart,sub->datbytes(),key,pos-forstart);
		return 0;
		}

#define bin2form(part,bin,form,len) part->fromascii((unsigned char *)(form),(unsigned char *)(bin), len) 
//#define form2bin(part,form,bin,len) part->makeascii((unsigned char *)(bin),(unsigned char *)(form), len) 
#define s2bin(part,form,bin,len) part->filterformat2bin((const char *)(form),(unsigned char *)(bin), len) 
int HasSub::form2bin(const char *formal, unsigned char *bin, int &len) const {	
		int forstart=0,binstart=0,took,left;
		ScreenPart *sub;
		for(int i=0;i<nrsubparts&& ((left=(len-forstart))>0) ;i++)  {
			sub=subpart(i);
			if((took=s2bin(sub,forstart+( char *)formal,binstart+bin, minnum(sub->maxres,left)))<=0)
				break;
			forstart+=took+spacebetween(sub);
			binstart+=sub->bytes;
			}
		if(forstart) {
			len=(forstart-spacebetween(sub));	
			return binstart;
			}
		else {
			len=0;
			return 0;
			}
		}


	int HasSub::bin2formal(unsigned char *bin, char *formal, int len) {	
		int forstart=0,binstart=0,last=nrsubparts-1;
		for(int i=0;;i++)  {
			ScreenPart *sub=subpart(i);
			if(bin2form(sub,binstart+bin,forstart+( char *)formal,(int)sub->bytes)<0)
				return -1;
			if(i>=last)
				break;
			forstart+=sub->maxres;
			binstart+=sub->bytes;
			int spaafter=spacebetween(sub);
			for(int j=0;forstart<maxres&&j<spaafter;j++)
				formal[forstart++]=' ';
//				formal[forstart++]='\t';
			}
		return 0;	

		}	
int HasSub::isel(const int ch) const {
	for(int i=0;i<nrsubparts;i++)
		if(subpart(i)->isel(ch))
			return 1;
	return 0;
	}

int HasSub::calcsizes(void) {
	int sbytes=0;
	maxres=0;
	for(int i=0;i<nrsubparts;i++) {
		ScreenPart *sub=subpart(i);
	//	sub->left=sbytes;
		sub->top=maxres;
	        sub->calcsizes();
		sub->oldhalf=sbytes;
		sub->half=maxres;
		if(sub->bytes<=0)
			return -1;
		sbytes+=sub->bytes;
		maxres+=spacebetween(sub)+sub->maxres;
		}
	bytes=screenupdatefilters(sbytes);
/*
PROBLEM: is  also called by canresize in some way
	do {
		bytes=screenupdatefilters(sbytes);
		} while((bytes<=0&&getlastfilter()&&rmlastfilt()));
*/
		
	if(nrsubparts>0)
		maxres-=spacebetween(subpart(nrsubparts-1));
	return bytes;
	}
/*
Changes screenbytes to b;

*/
int HasSub::chbytes(int b) {
	int res;
	if(active>=0&&active<nrsubparts) {
		ScreenPart *part=subpart(active);
		res=part->chbytes(b);
		if(res<1)
			return res;
		part->screenupdatefilters(part->bytes);
		calcsizes();
		return bytes;
		}
	return -1;
	}
short HasSub::chbase(short b) {
	if(active>=0&&active<nrsubparts) {
		int res=subpart(active)->chbase(b);
		calcsizes();
		return res;
		}
	return -1;
	}
int HasSub::activesubpart(short newact) {
	if(newact>=0&&newact<nrsubparts) {
		active=newact;
		type=active;
		}
	return active;
	}

ScreenPart *HasSub::getchild(int &forstart,int &binstart,int pos) {
	if((active>=0)&&pos<maxres) {
		forstart=0,binstart=0;
		for(int i=0;i<nrsubparts;i++)  {
			ScreenPart *sub=subpart(i);
			int end=sub->maxres+spacebetween(sub);
			if(pos<(forstart+end)) {
				if( (pos>=forstart) && (pos<(forstart+sub->maxres)))
					return sub;
				else
					return NULL;
				}
			forstart+=end;
			binstart+=sub->bytes;
			}
		}
	return NULL;	
	}
