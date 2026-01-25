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
#include "../filter.h"
#include "../screenpart.h"
class Editor;
class Index2table: public Filter {
	public:
	int tableoffset;
	int ptrbase;
	int maxintable;
	int doedit;
	Editor *edit;
	Index2table(int fbytes,int sbytes,int off=0,int len=INT_MAX,int pbase=0);
	Index2table(int fb,int sb,const char *conv);
	Index2table(Index2table const &rev);
virtual	int setpart(ScreenPart *p)  override ;
	virtual char *convstr(void) const override ;
	~Index2table() ;
	virtual Filter *newone(void) const  override ;
	virtual int chbytes(int b)  override ;
	virtual int chbytesback(int b)  override ;
	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const override ;
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override ;
	virtual int  equal(const Filter *fil) const override ;
	};
extern char *filtername(Index2table) ;
#define index2tablefindfunction(nam,procedure) \
	extern char *filtername(nam);\
	Filter *get##nam(int fb,int sb,const char *opts,ScreenPart *part=NULL) {\
		int start,len,startpos;\
		if((start=part->editor->procedure(len,startpos))<0) return NULL;\
		Index2table *sym= new Index2table(fb,sb,start,len);\
		if(!sym) return NULL;\
		sym->name=sym->label=filtername(nam);\
		part->editor->toborderpos(startpos);\
		part->editor->postotop();\
		return sym;\
		}\
	char *filtername(nam)= initfilter(#nam,&get##nam);

//	char *nam##name= initfilter(#nam,&get##nam);
