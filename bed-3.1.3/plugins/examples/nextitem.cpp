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
#include "../screenpart.h"
#include "../hassub.h"
#include "../repeat.h"

#endif



#ifdef EDITOR
void	cursorpos(void);
int  nextitem(void) {
	ScreenPart *act=activemode();
	for(int i=0;i<act->maxres;i++)
		if(nextchar()<0)
			return -1;
	return 0;
	}
int  backitem(void) {
	ScreenPart *act=activemode();
	for(int i=0;i<act->maxres;i++)
		if(backchar()<0)
			return -1;
	return 0;
	}
int  backsubitem(void) {
	ScreenPart *super=activemode(),*act=super->getsubpart();;
	int end=act->maxres;
	if(super!=act) {
		 end+=act->spaceafter;
		if((super->half-end)<0)
			 end-=act->spaceafter;
		}
	for(int i=0;i<end;i++)
		if(backchar()<0)
			return -1;
	return 0;
	}
int  nextsubitem(void) {
	ScreenPart *super=activemode(),*act=super->getsubpart();;
///	ScreenPart *super=activemode(),*act=super->getsubpart(forstart,binstart,super->half);

	int end=act->maxres;//+act->apart;
	if(super!=act) {
		 end+=act->spaceafter;
		if((end+super->half)>super->maxres)
			 end-=act->spaceafter;
		}
	for(int i=0;i<end;i++)
		if(nextchar()<0)
			return -1;
	return 0;
	}
/*
int  tosubpart(ScreenPart) {
	ScreenPart *act=activemode(),*sub;
	sub=act->getsubpart();
	if(sub!=act) {
		int len=sub->maxres;


			
		HasSub *has=(HasSub *)sub->getparent();
		if(has&&has->activesubpart()==(has->nrsubparts-1)) {
			sub=(ScreenPart *)has;
			while((has=(HasSub *)sub->getparent())&& has&&has->activesubpart()==(has->nrsubparts-1)) {
				};
			if(has) {
				if(has_super(has,Repeat)) {
					has->half+=sub->maxres+has->spacebetween(sub);
					}
				else {
					int activ=has->activesubpart();
					has->half=has->subpart(activ+1)->left;
					}
				cursorpos();
				}
			else {
				sub->half=0;
				topos((((geteditpos()+ getfilepos()) /sub->bytes)+1)*sub->bytes);
				}
			return 0;
			}
		else
			len+=has->spacebetween(sub);
		for(int i=0;i<len;i++)
			if(nextchar()<0)
				return -1;
		}
	else
		return nextitem();
	}
// call(nextsubitem)
*/
#endif


call(backitem)
call(nextitem)
call(nextsubitem)
call(backsubitem)
