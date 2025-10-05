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
#ifndef REPEAT_H 
#define REPEAT_H 
#include "hassub.h"
class  Repeat : public HasSub {	
	public:	 
	ScreenPart *part;
	declareparent 
	Repeat ( Editor *ed,ScreenPart *p=NULL, int nr=0,int x1=0, int y1=0);
	 Repeat ( Repeat const &rep);
	~Repeat(); 
	void poschanged(int pos)  override; 
	void mklabel(void) override; 
	ScreenPart *getchild(int &forstart,int &binstart,int pos) override; 
	ScreenPart *newone(void) const override;
	int calcsizes(void) override; 
	int chrepeat(int nr) override; 
ScreenPart * freesubpart(int pos=-1)  override;
inline	virtual int spacebetween(const ScreenPart * const sub) const override {
			return part->apart;
			}
	ScreenPart *subpart(const int i) const override; 
	int isel(const int ch) const override;
	ScreenPart *setsubpart(ScreenPart *pa,int i=-1) override;
int  userconfig(int kind)  override;
#ifdef EDITOR_H
	friend int Editor::uplevel(void);
	friend int Editor::savecontain(void) ;
#endif
};
inline int isrepeat(ScreenPart *part) {
	return (part->isparent(Repeat::thename));
	}
#endif //REPEAT_H
