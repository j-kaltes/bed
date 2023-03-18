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
#include "editor.h"
#include "screenpart.h"
#include "screen.h"
inline void Editor::resetpos(void) {
	if((filepos+editpos)>=(mem.size())) {
		beep();
		if(filepos>=(mem.size())) {
			filepos=mem.size()-1;
			}
		editpos=mem.size()-filepos-1;
		}
	editpos=lowborder();
	}

int Editor::middlepage(void) {
	editpos%=cols();
	editpos+=(screenmax()/2)-cols();
	resetpos();
	cursorpos();
	return 0;
	}
int Editor::beginpage(void) {
	editpos%=cols();
	cursorpos();
	return 0;
	}
int Editor::endpage(void) {
	editpos%=cols();
	editpos+=screenmax()-cols();
	resetpos();
	cursorpos();

	return 0;
	}
int Editor::edscholldown(void) {
	if(filepos>0) {
		memorize();
		if(filepos<cols()) {
			beep();
			filepos=filepos%parts[mode]->bytes;
			}
		else {
			filepos-=cols();
		#ifndef SETSCRREGBUG
			fromfile();
				if((Screenheight)>1) {
					scrollon(editscreen,1);
					wscrl(editscreen,-1);
					scrollon(editscreen,0);
					oldpos+=cols();
					}
				putline(0);
				cursorpos();
			return 0;
			#endif
			}
	
		fromfile();
#ifdef oneline
		if((Screenheight)>1) 
#endif
		       writebuf();	
#ifdef oneline
		else {
			putline(0);
			cursorpos();
			}
#endif
		return 0;
		}
	beep();
	return -1;
	}

int Editor::lowborder(void) {return (editpos/parts[mode]->getbytes())*parts[mode]->getbytes();};
int Editor::geteditpos(void) {return editpos;};

	OFFTYPE Editor::getfilepos(void) {return filepos;};

int Editor::scrollup(void) {
	memorize();
	filepos+=cols();
	if(filepos<mem.size()) {
		if((filepos+editpos)>=mem.size()) {
			editpos-=cols();
			if(editpos<0)
				editpos=0;
			}
		else {
		#ifndef SETSCRREGBUG
		fromfile();
				if((Screenheight)>1) {
					scrollon(editscreen,1);
					wscrl(editscreen,1);
					scrollon(editscreen,0);
					oldpos-=cols();
					}
				putline(rows()-1);
				cursorpos();
		return 0;
		#endif
		   }
		fromfile();

#ifdef oneline
		if((Screenheight)>1) 
#endif
		       writebuf();	
#ifdef oneline
		else {
			putline(0);
			cursorpos();
			}
#endif
		return 0;
		}
	filepos-=cols();
	cursorpos();
	beep();
	return -1;
	}


int Editor::lineup(void) {
	if(y()>0) {
		editpos-=cols();
		cursorpos();
		}
	else	{
		return edscholldown();
		}
	return 0;
	};
int Editor::linedown(void) {
	if(y()<(rows()-1)) {
		editpos+=cols();
		if((editpos+filepos)<mem.size()) {
			cursorpos();
			return 0;
			}
		editpos-=cols();
		}
	return scrollup();
	};
int Editor::pageup(void) {
	memorize();
	if(filepos>screenmax())
		filepos-=screenmax();
	else {
		filepos=filepos%parts[mode]->bytes;
		beep();
		}
	updated=0;
	fromfile();
	return 0;
	}

int Editor::pagedown(void) {
	if(filepos>(mem.size()-screenmax()-1)) {
		beep();
		return -1;
		}
	memorize();
	filepos+=screenmax();
	if(filepos+editpos>=(mem.size()))
		editpos=mem.size()-filepos-1;
	updated=0;	
	fromfile();
	return 0;
	}

int Editor::toborderpos(OFFTYPE pos) {
	int ret=pretopos(pos);
	toborder( pos);
	if(dofromfile) {
		fromfile();
		updated=0;
		}
	else
		cursorpos();
	return ret;
	}

int Editor::topos(OFFTYPE pos) {
	OFFTYPE oldfilepos=filepos;
	if(pretopos(pos)!=-1)	{
		toborder(oldfilepos) ;
		if(dofromfile) {
			fromfile();
			updated=0;
			}
		else
			cursorpos();
		return 0;
		}
	beep();
	return -1;
	}
int Editor::pretopos(OFFTYPE pos) {
	if(pos>=0&&pos<mem.size()) {
			if(pos<filepos||pos>(filepos+screenmax()-1)) {
				memorize();
				splitpos((pos<mem.size())?pos:(mem.size()-1));
				dofromfile=1;
				return 0;
				}
			else {
				editpos=pos-filepos;
				return 0;
				}
			}
	return -1;
	}
/*
editpos>=0 editpos<maxedit
filepos>=0
filepos+editpos<maxfile



over pos
filepos-=over
editpos+=over

filepos+=(by-over)
editpos-=(by-over)
over neg
filepos-=over
editpos+=over

filepos+=(by-over)
editpos-=(by-over)

not when editpos>maxedit 

filepos<0

not when filepos+editpos>maxfile
filepos+=(by-over)
editpos-=(by-over)
*/
#define posok(edpos,filpos) ((edpos>=0)&&(filpos>=0)&&(edpos+filpos)<filesize()&&edpos<view.getshown())
#define testreturn	if(posok(neweditpos,newfilepos)) {editpos=neweditpos;if(filepos!=newfilepos) { filepos=newfilepos; dofromfile=1; };editpos=lowborder();return 0;}
#define OLDTOBORDERPOS

int Editor::toborder(OFFTYPE posi) {
	int by=parts[mode]->bytes;
	int over=filepos%by-posi%by;
#ifdef OLDTOBORDERPOS
	OFFTYPE newfilepos=filepos;
	int neweditpos=editpos;
	if(over) {
		memorize();
		newfilepos=filepos-over;
		neweditpos=editpos+over;
		testreturn;
		newfilepos=filepos+(by-over);
		neweditpos=editpos-(by-over);
		testreturn;
		newfilepos=filepos-(by+over);
		neweditpos=editpos+(by+over);
		testreturn;
		int pos,neg;
		if(over>0) {
			pos=over;
			neg=over-by;
			}
		else{
			pos=over+by;
			neg=over;
			}
		if(filepos>=pos&&editpos<(view.getshown()-pos)) {
			filepos-=pos;
			editpos+=pos;
			editpos=lowborder();
			return 0;
			}
		filepos-=neg;
		editpos+=neg;
		if(editpos>=0) {
			editpos=lowborder();
			return 0;
			}
		editpos=0;
		if(filepos>=filesize())
			filepos=filesize()-1;
		beep();
		return -1;
		}
	editpos=lowborder();
	return 0;
#else	
if(!posok(editpos,filepos) ) {
	filepos=posi;
	editpos=0;
	beep();
	beep();
	beep();
	beep();
	beep();
	beep();
	beep();
	beep();
	message("System error: illegal offset"); 
	return -1;
	}
if(!over)
	return 0;
if(over>0) {
	pos=over;
	neg=over-by;
	}
else{
	pos=over+by;
	neg=over;
	}
if(filepos>=pos&&editpos<(view.getshown()-pos)) {
	filepos-=pos;
	editpos+=pos;
	return 0;
	}
filepos-=neg;
editpos+=neg;
if(editpos>=0)
	return 0;
editpos=0;
if(filepos>=filesize())
	filepos=filesize()-1;
beep();
return -1;
#endif
	}


/*
int Editor::toborder(OFFTYPE pos) {
	int by=parts[mode]->bytes;
	int over=filepos%by-pos%by;
	if(over) {
		memorize();
		dofromfile=1;
		filepos-=over;	
		if(filepos<0) {
			filepos+=by;
			editpos-=over;
			if(editpos<0) {
				editpos=0;
				return 0;
				}
			}
		else {
			editpos+=over;
			if(editpos<0) {
				editpos+=by;
				if(editpos<0) {
					editpos=0;
					return 0;
					}
				}
			}
		}
	return 0;
	}
*/
int Editor::postotop(void) {
	memorize();
	int bytes=parts[mode]->bytes;
	filepos=(editpos/bytes)*bytes+filepos;
	editpos=0;
	fromfile();
	updated=0;
	return 0;
	}
int Editor::scrollbackbyte(void) {
	memorize();
	filepos--;
	if(filepos<0) {
		filepos=0;
		beep();
		return -1;
		}
//	editpos++;
//	parts[mode]->half++;
	updated=0;
	fromfile();
	return -1;
	}

int Editor::scrollbyte(void) {
	memorize();
	filepos++;
	if(filepos>=mem.size()) {
		filepos--;
		beep();
		return -1;
		}
//	editpos--;

	if((filepos+editpos)>=mem.size()) 
		editpos=mem.size()-filepos-1;

	updated=0;
	fromfile();
	return 0;
	}

int Editor::scrollbackitem(void) {
	memorize();
	filepos-=parts[mode]->bytes;
	if(filepos<0) {
		filepos+=parts[mode]->bytes;
		beep();
		return -1;
		}
//	editpos+=parts[mode]->bytes;
//	Check borders
	updated=0;
	fromfile();
	return -1;
	}
int Editor::scrollitem(void) {
	memorize();
	filepos+=parts[mode]->bytes;
	if((filepos+editpos)>=mem.size()) {
		filepos-= parts[mode]->bytes;
		beep();
		return -1;
		}
//	editpos-=parts[mode]->bytes;
//	Check borders
	updated=0;
	fromfile();
	return 0;
	}
int Editor::endfile(void) {
	return topos(mem.size()-1);
	}
int Editor::beginfile(void) {
	return topos(0);
	}

//extern int visualwidth,visualoffset,virtualwidth,visualunit;

extern int makevisible(ScreenPart *part,int editpos);
int Editor::endline(void) {
	editpos=(editpos/cols())*cols()+cols()-1;
	resetpos();
	parts[mode]->end();
	if(makevisible(parts[mode],editpos)<0) {
		updated=0;
		return 0;
		}
	cursorpos();
	return 0;
	}
int Editor::beginline(void) {
	editpos=(editpos/cols())*cols();
	parts[mode]->begin();
	if(makevisible(parts[mode],editpos)<0) {
		updated=0;
		return 0;
		}
	cursorpos();
	return 0;
	}
int Editor::backchar(void) {
	if(parts[mode]->backchar()) {
		cursorpos();
		}
	else {
		if(editpos>=parts[mode]->bytes) {
			editpos-=parts[mode]->bytes;
			cursorpos();
			}
		else {
			return scrollbackitem();
			}
		}
	if(makevisible(parts[mode],editpos)<0) {
		updated=0;
		return 0;
		}
	return 0;
	};
int Editor::nextchar(void) {
	if(parts[mode]->nextchar()) {
		cursorpos();
		}
	else {
		if(editpos<(view.getshown()-parts[mode]->bytes)) {
			editpos+=parts[mode]->bytes;
			cursorpos();
			}
		else {
			return scrollitem() ;
			}
		}
	if(makevisible(parts[mode],editpos)<0) {
		updated=0;
		return 0;
		}
	return 0;
	}
int Editor::nextfast(void) {
	if(parts[mode]->nextchar()) {
		}
	else {
		if(editpos<(view.getshown()-parts[mode]->bytes)) {
			editpos+=parts[mode]->bytes;
			}
		else {
			if((editpos+filepos+parts[mode]->bytes)>=mem.size()){
				parts[mode]->half=parts[mode]->maxres-1;
			/*	editpos--;
				cursorpos();
				editpos++;
*/
				cursorpos();
				return -1;
				}
			else
				if(!scrollup()) {
					editpos-=(cols()-1);
					}
				else
					return -1;
			}
		}
	cursorpos();
	if(makevisible(parts[mode],editpos)<0) {
		updated=0;
		toscreen();
		return 0;
		}
	return 0;
	}



