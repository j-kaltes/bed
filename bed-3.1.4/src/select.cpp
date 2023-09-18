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
#include "desturbe.h"
#include "editor.h"
#include "main.h"
#include "system.h"
#include "editlimits.cpp"
#include "myalloc.h"
#include "screen.h"
#ifdef _WIN32
#include "win.h"
#endif
#include "misc.h"
OFFTYPE Editor::lensel=0;

int Editor::seloffset=0;
char *Editor::selbuf=NULL;
Editor *selectioneditor=NULL;
Editor *getselectioneditor(void) {
	return selectioneditor;
	}
Editor *setselectioneditor(Editor *ed) {
	Editor *olded=selectioneditor;
	selectioneditor=ed;
	return olded;
	}
#include "screenpart.h"
int Editor::selectstandout(void) {
	if( standout1!=standout2) {
		updated=0;
		selectioneditor=this;
		selectpos=standout1;
		return topos(standout2-1);
		}
	beep();
	return -1;
	}
OFFTYPE Editor::setselect(OFFTYPE pos) {
	if(pos==OFFTYPEINVALID) 
		updated=0;
	else
		selectioneditor=this;
	return selectpos=pos;
	}
//s/\<other\>/selectpos/g
int Editor::setselect(void) {
	if(selectpos==OFFTYPEINVALID) {
		setselect(filepos+(editpos/parts[mode]->bytes)*parts[mode]->bytes);
		selectioneditor=this;
		}
	else {
		setselect(OFFTYPEINVALID);
		updated=0;
		}
	return 0;
	}
//MAXSELBUF: can do without looking


 
OFFTYPE Editor::getselect(void) {
	return selectpos;
	}
OFFTYPE getselbuf(char **buf) {
	if(!Editor::selbuf)
		return -1;
	*buf=Editor::selbuf+Editor::seloffset;
	return Editor::lensel;
	}

OFFTYPE Editor::getselbuf(char **buf) {
	return ::getselbuf(buf);
	}
int putselbuf(char *buf,int len) {
	INCDEST();
	if(Editor::selbuf)
		alignedfree( Editor::selbuf);
	Editor::selbuf= myallocar( char,len);
	memmove(Editor::selbuf,buf,len);
	Editor::lensel=len;
	Editor::seloffset=0;
	DECDEST();
	return Editor::lensel;
	}
int Editor::putselbuf(char *buf,int len) {
	return ::putselbuf(buf,len);
	}
int Editor::pastselect(void) {
	if(selbuf) {
		putbuf(selbuf+seloffset,lensel) ;
	/*
		short bytes=parts[mode]->bytes;
		int pos=(editpos/bytes)*bytes;
		int len=minnum(lensel,mem.size()-(pos+filepos));
		memorize();
		posputbuf(pos+filepos,selbuf,len);
		dofromfile=1;
		topos(filepos+pos+len-1);
		*/
		return 0;
		}
	else {
		output("Nothing to past");
		beep();
		return -1;
		}
	}

int Editor::savesel(const char *ant) {
	BEDS_SIZE_T ret;
	FILEHANDLE handle;
	memorize();
	if((handle=openfile(ant))==MY_INVALID_HANDLE_VALUE) {
		return -2;
		}

	OFFTYPE first,len;
	if((len=getselregion(&first))) {
		lensel=len;
		message("Writing to file %s",ant);
		wrefresh(editscreen);
		if(mem.saveto(handle,first,lensel)!=0) {
			closehandle(handle);
			beep();
			message("Write to %s failed ",ant);
			wrefresh(editscreen);
			return -2;
			}
		updated=0;
		}
	else {
		ret=writetofile(handle,selbuf+seloffset,lensel);
		if(ret!=lensel) {
			closehandle(handle);
			beep();
//s/sys_errlist\[errno\]/strerror(errno)/g
#ifndef _WIN32
			message("write failed: %s",strerror(errno));
			wrefresh(editscreen);
#else
			showerror("write failed: %s");
#endif
			return -2;
			}
		}
	if(!closehandle(handle)){
#ifndef _WIN32
		message("close(handle) failed: %s",strerror(errno));
#else
		message("closehandle(handle) failed");
#endif
		return -1;
		}
/* during a selecton currently bed writeover the screen after every procedure
so this code has no effect: */
	message("Selection saved in %s",ant);
	return 0;
	}
int Editor::saveselect(void) {
	if(selectpos!=OFFTYPEINVALID||selbuf) {
		const char *ant;
		BEDS_SIZE_T ret;
		char buf[BEDSMAXPATH]="*";
	do {
		again:
		ant=getfile("Save Selection?",buf) ;
		editup();
		if(!ant)
			return -1;
		if((ret=savesel(ant))==-2) {
			strcpyn(buf,ant,BEDSMAXPATH);
			goto again;
			}
		} while(ret==-2);
	return ret;
	}
	message("No selection");
	return -1;
	}
int Editor::selected(OFFTYPE pos) {
		OFFTYPE start,len;
		return ((len=getselregion(&start))&&(pos>=start&&pos<(start+len)));
		};
#include "screenpart.h"

extern long long getfree(void) ;

#define XWINSELECT
#ifdef XWINSELECT
ScreenPart	*selectdatatype=NULL;
int	setselectdatatype(ScreenPart *part) {
	if(selectdatatype)
		delete selectdatatype;
	selectdatatype=part->newone();
	return 0;
	}
ScreenPart	*getselectdatatype(void) {
	return selectdatatype;
	}
#endif

OFFTYPE Editor::getselregion(OFFTYPE *start) {
	if(selectpos==OFFTYPEINVALID) {
		return 0;
		}
	int bytes=parts[mode]->getbytes();
	OFFTYPE posf=filepos+(editpos/bytes)*bytes,first,len;
	if(selectpos>posf) {
		len=selectpos-posf+1;
		first=posf;
		}
	else {
		len=posf-selectpos+1;
		first=selectpos;
		}
	len=((len+bytes-1)/bytes)*bytes;
	if((len+first)>filesize())
		len=filesize()-first;
	*start=first;
	return len;
	}
int Editor::copyselect(void) {
	OFFTYPE first,len;
	if(!(len=getselregion(&first))) {
		message("No selection");
		beep();	
		return -1;
		}
	if((len>MAXSELBUF)&&(usecopy(len)>getfree())) {
		output("Not enough memory");
		beep();
		return -1;
		}
	INCDEST();

	//char *newbuf=myallocar( char,len);

int blone=first/blocksize(),blend=(first+len-1)/blocksize()+1,blnr=blend-blone;
int size=blnr*blocksize();

	char *newbuf=(char *)alignedalloc(blocksize(),size*sizeof(char) );
	if(!newbuf) {
		DECDEST();
		message("Can't allocate %d of memory for selection",len);
		beep();
		return -1;
		}
	else {
		memorize();
		if( mem.getblocks(blone, newbuf,blnr)== OFFTYPEINVALID) {
//		if( mem.getpart(first, len, newbuf)== OFFTYPEINVALID) {
			DECDEST();
			message("Can not read offset " OFFPRINT  " length %d",first,len);
			beep();
			alignedfree( newbuf);
			return -1;
			}
		if(selbuf)
			alignedfree( selbuf);
		seloffset=first%blocksize();
		lensel=len;
		selbuf=newbuf;
//		updated=0; 
//		setselect(OFFTYPEINVALID);
#ifdef XWINSELECT
		setselectdatatype(parts[mode]);
#endif
		DECDEST();
		message("Copied");
		return 0;
		}
	DECDEST();
	beep();
	return -1;
	}
