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
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../offtypes.h"
#include "index2table.h"
#define waslong int
class Index2table;
class Editor {
public:
#include "faked.h"

iscalled(index2table)
//int Editor::gettableposition(int &length, int &startpos) ;

int startindex2table(Filterprocptr func) ;
};
#define min(x,y) ((x)<(y)?(x):(y))
	int tableoffset;
	int ptrbase;
	int maxintable;
	int doedit;
	Editor *edit;
	Index2table::Index2table(int fbytes,int sbytes,int off,int len,int pbase):Filter(fbytes,sbytes,filtername(Index2table),filtername(Index2table)), tableoffset(off),ptrbase(pbase),maxintable(len),doedit(1),edit(NULL) {
		}
	Index2table::Index2table(int fb,int sb,const char *conv) : Filter(fb,sb,filtername(Index2table),filtername(Index2table)),doedit(1) {
		if(conv) {
			if(!sscanf(conv,"%d-%d-%d",&tableoffset,&maxintable,&ptrbase)) {
				sscanf(conv,"%d-%d",&tableoffset,&maxintable);
				ptrbase=0;
				}
			}
		}
//s/=\(.*\);/(\1),/g

	Index2table::Index2table(Index2table const &rev): Filter(rev), 
		tableoffset(rev.tableoffset),
		ptrbase(rev.ptrbase),
		maxintable(rev.maxintable),
		doedit(1),
		edit(rev.edit)
		{
		}
	int Index2table::setpart(ScreenPart *p) {
		Filter::setpart(p);
		edit=p->editor;
		if(maxintable==INT_MAX)
			maxintable=edit->filesize()-tableoffset;
		return 0;
		}
	char *Index2table::convstr(void) const {
	#define convmax 80
		static char *conv=new char[convmax];
		snprintf(conv,convmax,"%d-%d-%d",tableoffset,maxintable,ptrbase);
		return conv;
		}
	Index2table::~Index2table() {
		}
	 Filter *Index2table::newone(void) const  {
		return new Index2table(*this);
		}
	 int Index2table::chbytes(int b) {
		edit=part->editor;
		bytesfile=b;
		return bytesscreen;
		}
	 int Index2table::chbytesback(int b) {
		edit=part->editor;
		bytesscreen=b;
		err=0;
		return bytesfile;
		}
#define min(x,y) ((x)<(y)?(x):(y))
	 int Index2table::fromfile(const unsigned char *const input,unsigned char *const output) const  {
		if(edit) {
			waslong intable=*((waslong*)input)-ptrbase;
			char tmp[bytesscreen+1]; 
			memset(output,' ',bytesscreen);
			if(intable>=0&&intable<maxintable) {
				edit->getmem(	tableoffset +intable,(char *)tmp,bytesscreen);
				int len=min((int)strlen(tmp),bytesscreen);
				memcpy((char *)output,tmp,len);
				}
			}
		return bytesscreen;
		}
	 int Index2table::tofile(const unsigned char *const input,unsigned char *const output) {
/* How do you know if it was intented to endup in the file instead of for
some other purpose like search 
WHen stop reading input? 
If you stop with space no space is possible inside label
if you don't stop you overwrite the next label
Some special stop character?
*/
		if(doedit&&edit) {
			waslong intable=*((waslong*)output)-ptrbase;
			if(intable>=0&&intable<maxintable) {
				int len=strcspn((char *)input," ");
				char tmp[len+1];
				memcpy(tmp,input,len);
				tmp[len]='\0';
				edit->putmem(	tableoffset +intable,(char *)tmp,min(bytesscreen,len+1));
				}
			}

		return bytesfile;
		}
	int  Index2table::equal(const Filter *fil) const {
		if(!Filter::equal(fil))
			return 0;
		if(tableoffset!=((Index2table *)fil)->tableoffset)
			return 0;
		if(maxintable!=((Index2table *)fil)->maxintable)
			return 0;
		if(edit!=((Index2table *)fil)->edit)
			return 0;
		return 1;
		}

registerfilterclass(Index2table) 



#include "../screen.h"
#include "../dialog.h"
int Editor::index2table(void) {
	ScreenPart *super=activemode(),*act;
	act=super->getsubpart();
	char offset[50]="";
	char length[50]="";
	char ptrbase[50]="";
	Index2table *ad=NULL;
for(int i=act->firstfilter;i<act->lastfilter;i++) {
	if(act->filters[i]->name==filtername(Index2table)) {
		ad=(Index2table*)act->filters[i];
		sprintf(offset,"%d",ad->tableoffset);
		sprintf(length,"%d",ad->maxintable);
		sprintf(ptrbase,"%d",ad->ptrbase);
		break;
		}
	}
int res=0;

  begindialog(11,40,5);
    mvwaddstr(takescr,1,2,"Labels at");
    linernrmax("File Off~set",offset,3,2,17,50);
    linernrmax("~Length",length,5,2,17,50);
    linernrmax("~Pointer base",ptrbase,7,2,17,50);
    oke(9,2);
    act("~Cancel",9,10);
  enddialog(res,res);
filesup();
switch(res) {
	case 0:;
	case 4: 
		return -1;
	}
	waslong taboff=strtol(offset,NULL,0);
	waslong tablen=strtol(length,NULL,0);
	waslong pbase=strtol(ptrbase,NULL,0);
	if(ad) {
			
		ad->tableoffset=taboff;
		ad->maxintable=tablen;
		ad->ptrbase=pbase;
		}
	else  {
		ScreenPart *super=activemode(),*act;
		act=super->getsubpart();
		ad=new Index2table(4,act->datbytes(),taboff,tablen,pbase);
		if(ad->error()) {
			delete ad;
			message("Error adding add");
			return -1;
			}
		addfilter(ad);
		}
	modechanged();
	super->calcsizes();
	toscreen();	
	return 0;
	}

sethelp(index2table,"index2table\n\nindex2table interprets the file content as long's (4 bytes)\nwhich are indexes in a table at some offset in the current file.\nInstead of the index number it displays the content of that table\nat that position. index2table is a filter that should be applied\nto a datatype appropriate for the data in the table. For this\npurpose this procedure asks to specify at what offset in the file\nthe labels start, with what index the table starts and table length.\nIndex2table is for example used to show the names of procedures\nin a symbol table. index2table is then applied to\nascii strings that shows this label instead of the index.\ndynsym and symtab are sub classes of index2table.\nThey are used by misc/dynsym.bedda and misc/symtab.bedda.\nBecause of this use index2table was previously called symtable.") 
call(index2table)

/*
extern char *filtername(Section) ;
static char sectionnul[1]="";
class Section: public Index2table {
	public:	
	int setpart(ScreenPart *p) {
		Filter::setpart(p);
		edit=p->editor;
		tableoffset=edit->getsymposition(maxintable);
		}

	Section(int fb,int sb,char *conv=NULL,ScreenPart *p=NULL) : Index2table(fb,sb) {
		label=name=filtername(Section);
		if(part) {
			Filter::setpart(p);
			edit=part->editor;
			tableoffset=edit->getsymposition(maxintable);
			}
		else
			maxintable==INT_MAX;
		}
	Section(Index2table &rev): Index2table(rev) {
		label=rev.label;
		maxintable==INT_MAX;
		}
	char *convstr(void) {
		return sectionnul;
		}
	};
*/
int Editor::startindex2table(Filterprocptr func) {
	ScreenPart *super=activemode(),*act;
	act=super->getsubpart();
	Filter *ad=func(4,15,"",act);
	if(!ad) {
		message("Could not create filter");
		beep();
		return -1;
		}
	if(ad->error()) {
		delete ad;
		message("Error adding add");
		return -1;
		}
	addfilter(ad);
	act->screenbytes(ad->bytesscreen);
	toscreen();	
return 0;
	}



