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
#ifndef HASSUB_H
#define HASSUB_H
#include "screenpart.h"

class  HasSub : public ScreenPart {	
	public:	
	int active,nrsubparts;
	declareparent 
	HasSub( Editor *ed,int nr=0,int act=-1);
	HasSub ( HasSub const &con);
	~HasSub();
//	virtual int format2bin(char *formal, unsigned char *bin, int len) ;
virtual int form2bin(const char *formal, unsigned char *bin, int &len) const  override;
	virtual int bin2formal(unsigned char *bin, char *formal, int len)  override;	
inline	virtual int spacebetween(const ScreenPart *const sub) const {
			return sub->spaceafter;
			}
	virtual ScreenPart * freesubpart(int pos=-1) =0;
	virtual int chbytes(int bytes) override;
	virtual ScreenPart *subpart(const int i) const =0;
	virtual ScreenPart *setsubpart(ScreenPart *part,int i=-1) =0;
	virtual ScreenPart *newone(void) const  override =0;
	virtual short chbase(short base) override;
	virtual int chrepeat(int b) =0;
	virtual int activesubpart(short newact=-1) ;
	virtual int calcsizes(void)  override;
	virtual int isel(const int ch) const override;
	virtual int addkey(unsigned char *bin,int binlen, int key, int pos )  override;
	virtual void mklabel(void) override =0;
	virtual void poschanged(int pos) override=0;
	virtual ScreenPart *getchild(int &forstart,int &binstart,int pos) override;
	virtual inline int issubindex(void)  {
		return ((active>=0)&&(active<nrsubparts));
		};
#ifdef EDITOR_H
friend	int Editor::uplevel(void) ;
friend int Editor::addtocontain(ScreenPart *part) ;
#endif
	};
inline int isleaf(ScreenPart *part) {
	return !(part->isparent(HasSub::thename));
	}
#endif //HASSUB_H
