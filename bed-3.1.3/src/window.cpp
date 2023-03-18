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
#include <string.h>
#include "editor.h"
#include "main.h"
#include "views.h"
#include "screen.h"

#include "myalloc.h"
	inline int Editor::switchcursor(void) {
			nocursor();
			wrefresh(editscreen);
			edits[editfocus]->cursorpos();
			return 0;
			}
int Editor::addfile(void) {
	if((askmenufile())==-1)
		return -1;
	if(addlast()>-1)
		redistribute=1;
	return 0;
	}

int Editor::zoomwindow(void) {
	for(int i=startactive;i<startnonactive;i++)
		edits[i]->windel();
	editfocus=startactive=startactive+fromtop;
	startnonactive=startactive+1;
	redistribute=1;
	return 0;
	};

int Editor::hidewindow(void) {
	Editor *ed;
	redistribute=1;
	if(nredit<2) {
		if(!argfile())
			return(askmenufile());
		else {
			return 0;
			}
		}
	if((startnonactive-startactive)<2) {
		if(startnonactive<nredit)
			startnonactive++;
		else
			startnonactive=startactive;
		startactive=startnonactive-1;
		editfocus=startactive;
		return 0;
		}
	if((editfocus+1)==startnonactive) {
		startnonactive--;
		editfocus--;
		}
	else {
		if(editfocus>startactive) {
			ed=edits[editfocus];
			memmove(edits+startactive+1,edits+startactive,sizeof(Editor *)*(editfocus-startactive));
			edits[startactive]=ed;		
			}
		else
			editfocus++;
		startactive++;
		}
	return 0;
	}

/*
int Editor::hidewindow(void) {
	Editor *ed;
	if(nredit<2) {
		if(!argfile())
			return(askmenufile());
		else {
			return 0;
			}

		}
	if(startnonactive<(startactive+2)||editfocus>=startnonactive||editfocus<startactive) {
		if(editfocus>0)
			editfocus--;
		else
			editfocus++;
		newscherm=1;
		return 0;
		}
	ed=edits[editfocus];
	startnonactive--;
	for(int i=editfocus;i<startnonactive;i++)
		edits[i]=edits[i+1];
	edits[startnonactive]=ed;
	if(editfocus>startactive)
		editfocus--;
	else
		editfocus++;
	redistribute=1;
	return 0;
	}
	*/
int Editor::lastfile(void) {
		if(editfocus==(nredit-1)) {
			beep();
			return -1;
			}
		editfocus=nredit-1;
		if(editfocus>=startnonactive)
			redistribute=1;
		else {
			switchcursor();
			}
		return LASTFILE;
		};
int Editor::firstfile(void) {
		if(!editfocus) {
			beep();
			return -1;
			}
		editfocus=0;
		if(editfocus<startactive)
			redistribute=1;
		else
			switchcursor();
		return FIRSTFILE;
		};

	int Editor::listfiles(void) {
		int i;
		char *files[nredit];
		int fil;

		for(i=0;i<nredit;i++) {
			files[i]=myallocar( char,MAXFILES);
			snprintf(files[i],MAXFILES,"%-53.53s  %c",edits[i]->mem.filename,((edits[i]->modified||edits[i]->changed)?'*':' '));
			}
		while((fil=selectitemindex("Select open buffer",files,nredit,editfocus))!=INT_MAX) {
			if(fil==(INT_MAX-1)) {
				menuresized();
				wrefresh(editscreen);
				}
			else {

				editfocus=fil;
				if(editfocus<startactive||editfocus>=startnonactive) {
						redistribute=1;
						addlast();
						}
				else {
					editup();
					nocursor();
					wrefresh(editscreen);
					edits[editfocus]->cursorpos();
					}

				for(i=0;i<nredit;i++)
					myfree (files[i]);
				return 0;
				 }
			}
		editup();
		for(i=0;i<nredit;i++)
			myfree (files[i]);
		return -1;
		};
int Editor::addnext(void) {
	if(nredit>startnonactive) {
		editfocus=startnonactive++;
		redistribute=1;
		return 0;
		}
	if(!argfile())  {
				beep();
				return -1;
				}
	if(nredit>startnonactive) 
		startnonactive++;
	redistribute=1;
	return 0;
	}

	int Editor::nextfile(void) {
		if((editfocus>=(nredit-1))) {
				if(!argfile())  {
					beep();
					return -1;
					}
				}
		else {
			editfocus++;
			if(editfocus>=startnonactive) {
				redistribute=1;
				}
			else {
				nocursor();
				wrefresh(editscreen);
				edits[editfocus]->cursorpos();
				}
			}

		return NEXTFILE;
		};

int Editor::addprev(void) {
	if(startactive>0) {
		startactive--;
		editfocus=startactive;
		redistribute=1;
		return 0;
		}
	beep();
	return -1;
	}
	int Editor::prevfile(void) {
		if(editfocus) {
			editfocus--;
			if(editfocus<startactive) {
				redistribute=1;
				}
			else {
				nocursor();
				wrefresh(editscreen);
				edits[editfocus]->cursorpos();
				}
			}
		else {
			beep();
			};
		return PREVFILE;
		};
int Editor::gotonrwin(int nr) {
	if(nr<0||nr>=nredit) {
		beep();
		return -1;
		}
	editfocus=nr;
	if(editfocus<startactive||editfocus>=startnonactive) {
			startactive=editfocus;
			startnonactive=editfocus+1;
			redistribute=1;
			return 0;
			}
	nocursor();
	wrefresh(editscreen);
	edits[editfocus]->cursorpos();
	return 0;
	}

#define gotowin(nr) int Editor::goto##nr(void) {return Editor::gotonrwin(nr-1);} 
gotowin(1)
gotowin(2)
gotowin(3)
gotowin(4)
gotowin(5)
gotowin(6)
gotowin(7)
gotowin(8)
gotowin(9)
#undef gotowin
