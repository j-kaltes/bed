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
#include "type.h"
#include "system.h"
#include "repeat.h"
#include "contain.h"
#include "editor.h"
//#define NEWSUBPARTS
extern char showfileinfo;
	Contains::Contains ( Editor *ed): HasSub( ed)	{
		active=-1;
//		name=Contains::thename;
		maxsubparts=5;
		nrsubparts=0;
		maxres=0;bytes=0;
		spaceafter=1;
		apart=1;
#ifdef NEWSUBPARTS
	 	subparts=new (ScreenPart *)[maxsubparts];
#else
	 	subparts= myallocar(ScreenPart *,maxsubparts);
#endif
		};	
	 Contains::Contains ( Contains const &con):HasSub(con) {
//		name=con.name;
	 	maxsubparts=con.maxsubparts;
	 	nrsubparts=con.nrsubparts;
	 	active=con.active;
#ifdef NEWSUBPARTS
	 	subparts=new (ScreenPart *)[maxsubparts];
#else
	 	subparts= myallocar(ScreenPart *,maxsubparts);
#endif
	 	for(int i=0;i<nrsubparts;i++) {
	 		subparts[i]=(ScreenPart *)con.subparts[i]->newone();
			subparts[i]->contain=this;
			}
		};
	Contains::~Contains() {
		while(nrsubparts>0) {
			delete subparts[--nrsubparts];
			}

#ifdef NEWSUBPARTS
		delete[] subparts;
#else
		myfree(subparts);
#endif
		}
ScreenPart *Contains::setsubpart(ScreenPart *part,int i){
		if(i<0) {
			i=active;	
			if(i<0)
				return NULL;
			}
		ScreenPart *old= subparts[i];
		subparts[i]=part;
		part->contain=this;
		return old;
		}

/*
Oh toch niet weer in de hut.

Zal het eerst weten, zwart op wit.
*/
/* 
New label if and only if label datatype changed:
- other active
- other pos in Repeat
*/

 void Contains::poschanged(int pos)  {
 	if(pos<maxres) {
		int forstart=0;
		for(int i=0;i<nrsubparts;i++)  {
			ScreenPart *sub=subparts[i];
			int end=sub->maxres+sub->spaceafter;
			if(pos<(forstart+end)) {
				if( (pos>=forstart) && (pos<(forstart+sub->maxres))) {
					if(!isleaf(sub)) {
						int newhalf=pos-forstart;
						if(active!=i||sub->half!=newhalf) {
							active=i;
							sub->half=newhalf;
							sub->poschanged(newhalf);
							mklabel();
//							snprintf(label,maxlabel,"[%s]",sub->label);
							showfileinfo=1;
							}
						}
					else {
						if(active!=i) {
							active=i;
							subparts[active]->mklabel();
//							snprintf(label,maxlabel,"[%s]+%d",sub->label,sub->oldhalf);
							mklabel();
							showfileinfo=1;
							}
						}
					return;
					}
				else {
					break;
					}
				}
			forstart+=end;
			}
		}
	active=-1;
	mklabel();
	showfileinfo=1;
	return;
	}

int  Contains::userconfig(int kind) {
	switch(kind) {
		case 0: return editor->newcontain();
		default: return editor->uplevel();
		};
	return -1;
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
 void Contains::mklabel(void) {
 	if(active>=0&&active<nrsubparts) {
		ScreenPart *sub= subparts[active];
		sub->mklabel();
		snprintf(label,maxlabel,"<%d>%s",getindatatype(sub),sub->label);
		}
	else {
	if(!userlabel) {
		int len=snprintf(label,maxlabel,"%s(%d)",getname(),bytes);
		mkfilterstr(label+len,maxlabel-len);
		}
	else
		strcpyn(label,userlabel,maxlabel);
		}
		
 	}

#pragma GCC diagnostic pop

	int Contains::addsubpart(ScreenPart *part,int pos) {
		if(pos==-1)
			pos=nrsubparts;
		if(pos>nrsubparts)
			return -1;
		if(nrsubparts==maxsubparts) {
#ifdef NEWSUBPARTS

			ScreenPart **old=subparts;
			subparts=new ScreenPart*[maxsubparts*=2];
			memcpy(subparts,old,pos*sizeof(ScreenPart*));
			memcpy(subparts+pos+1,old+pos,(nrsubparts-pos)*sizeof(ScreenPart*));
			delete[] old;
			}
		else
#else
			subparts=myrealloc(ScreenPart **,subparts,ScreenPart *,(maxsubparts*2),maxsubparts);
			maxsubparts*=2;
			}
#endif
		if(nrsubparts>pos)
			memmove(subparts+pos+1,subparts+pos,(nrsubparts-pos)*sizeof(ScreenPart*));
		part->contain=this;
		subparts[pos]=part;
		nrsubparts++;

		active=pos;
		if(calcsizes()<=0) {
			freesubpart(pos);
			return -1;
			}
		return pos;
		}
	ScreenPart *Contains::rmsubpart(int pos) {
		if(pos==-1)
			pos=nrsubparts-1;
		if(nrsubparts==1)
			return NULL;
		delete subparts[pos];
		if(pos<(nrsubparts-1))
			memmove(subparts+pos,subparts+pos+1,(nrsubparts-pos-1)*sizeof(ScreenPart *));
		nrsubparts--;
		if(nrsubparts==1&& !getlastfilter()&& isleaf(subparts[0]) ) {
			nrsubparts=0;
			subparts[0]->contain=NULL;
			return subparts[0];
			}
		calcsizes();
		active=nrsubparts-1;
		return this;
		}

	ScreenPart * Contains::freesubpart(int pos) {
		if(nrsubparts<=0)
			return NULL;
		if(pos==-1)
			pos=nrsubparts-1;
		if(pos<0||pos>=nrsubparts)
			return NULL;
		if(pos<(nrsubparts-1))
			memmove(subparts+pos,subparts+pos+1,(nrsubparts-pos-1)*sizeof(ScreenPart *));
		nrsubparts--;
		calcsizes();
		active=nrsubparts-1;
		return subparts[pos];
		}
int Contains::chrepeat(int b) {
	if(issubindex()) {
		if(isrepeat(subparts[active])) {
			if(b<1)
				return -1;
			b=((HasSub *)subparts[active])->chrepeat(b);
			}
		else {
			if(b>1) {
				ScreenPart *part=subparts[active];
				subparts[active]= new Repeat(editor,part,b);
				subparts[active]->contain=this;
				}
			}
		calcsizes();
		return b;
		}
	return -1;
	}
