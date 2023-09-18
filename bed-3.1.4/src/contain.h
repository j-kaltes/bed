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
#ifndef CONTAIN_H
#define  CONTAIN_H
#include "screenpart.h"
#include "hassub.h"
class  Contains : public HasSub {	
	ScreenPart  **subparts;
	int maxsubparts;
	public:	
	declareparent 
	Contains ( Editor *ed);
	Contains ( Contains const &con);
	~Contains();
	ScreenPart *subpart(const int i) const override{
		return subparts[i];
		}
	virtual ScreenPart *setsubpart(ScreenPart *part,int i=-1) override;
	virtual int addsubpart(General *part,int pos=-1) ;
	virtual ScreenPart *rmsubpart(int pos=-1); 
	virtual ScreenPart *newone(void) const override{
		return new Contains(*this);
		};
	virtual int chrepeat(int b)  override;
	virtual void mklabel(void) override;
	virtual void poschanged(int pos) override;
	ScreenPart * freesubpart(int pos=-1) override;
int  userconfig(int kind)  override;
#ifdef EDITOR_H
friend int Editor::searcher(int);
friend int Editor::addtocontain(ScreenPart *part);
friend int Editor::savecontain(void) ;
friend int Editor::loaddatatype(char *gegs,int endf,int addfield) ;
#endif
	};

inline int iscontain(const ScreenPart *part) {
	return (part->isparent(Contains::thename));
	}
#endif
