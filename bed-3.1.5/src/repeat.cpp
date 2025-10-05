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
#include "datatypes.h"
#include "dialog.h"
#include "screen.h"
#include "repeat.h"
parentd(HasSub,Repeat) 
extern char showfileinfo;

#define count nrsubparts
datatype(Repeat)
	Repeat::Repeat ( Editor *ed,ScreenPart *p, int nr,int x1, int y1): HasSub( ed)	{
		part=p;
		count=nr;
		chrepeat(nr);
		};
	 Repeat::Repeat ( Repeat const &rep):HasSub(rep) {
		part=rep.part->newone();
		count=rep.count;
	 	active=rep.active;
	 	};
	Repeat::~Repeat() {
		if(part)
			delete part;
		}

ScreenPart * Repeat::freesubpart(int pos) {
	ScreenPart *oldpart=part;
	part=NULL;
	return oldpart;
	}	
 void Repeat::poschanged(int pos)  {
 		if(pos>maxres)
 			return;

		int nr=pos/(part->maxres+spacebetween(part));
		if(!isleaf(part)) {
			int newhalf=pos-(part->maxres+spacebetween(part))*nr;
			if(active!=nr||part->half!=newhalf) {
				active=nr;
				part->half=newhalf;
				part->poschanged(newhalf);
				showfileinfo=1;
				}
			}
//		part->poschanged();
		if((pos%(part->maxres+spacebetween(part)))>=part->maxres) {
			active=-1;
			}
		else {
			active=nr;
			}
		mklabel();
		showfileinfo=1;
		}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
 void Repeat::mklabel(void) {
		part->mklabel();
	if(active>=0&&active<count) {
		snprintf(label,maxlabel,"%d:<%d>%s",active,getindatatype(part),part->label);
		}
	else {
		if(!userlabel) {
			int len=snprintf(label,maxlabel,"%d*%s",nrsubparts,part->label);
			mkfilterstr(label+len,maxlabel-len);
			}
		else
			strcpyn(label,userlabel,maxlabel);
		}
	}

#pragma GCC diagnostic pop


	ScreenPart *Repeat::getchild(int &forstart,int &binstart,int pos) {
		if(active<0||pos>=maxres||(pos%(part->maxres+spacebetween(part)))>=part->maxres) {
			return NULL;
			}
		int nr=pos/(part->maxres+spacebetween(part));
		forstart=nr*(part->maxres+spacebetween(part));
		binstart=nr*part->bytes;
		return part;
		}



	ScreenPart *Repeat::newone(void) const {return new Repeat(*this);}
int Repeat::calcsizes(void) {
	if(count<1) {
		maxres=1;
		bytes=1;
		active=-1;
		return -1;
		}
	part->contain=this;
	type=part->type,
	part->calcsizes();
	if(part->bytes<=0)
		return -1;
	if(active>=count)
		active=count-1;
	maxres=count*(part->maxres+spacebetween(part));
	maxres-=spacebetween(part);
	int res=screenupdatefilters((part->bytes)*count);
	if(res>0) {
		bytes=res;
		return bytes;
		}
	return -1;
	}
	int Repeat::chrepeat(int nr) {
		if(nr>0)
			count=nr;
		 calcsizes();
		return count;
		}

ScreenPart *Repeat::subpart(const int i) const {
	return part;
	}
int Repeat::isel(const int ch) const{return part->isel(ch);}

ScreenPart *Repeat::setsubpart(ScreenPart *pa,int i) {
		if(i<0) {
			i=active;	
			if(i<0)
				return NULL;
			}
		ScreenPart *old= part;
		part=pa;
		part->contain=this;
		return old;
		}
int  Repeat::userconfig(int kind) {
        return editor->repeatcontain();
        }


