#include "defines.h"
#ifdef _WIN32

#endif
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
#ifdef _WIN32
#include <windows.h>
#endif
#include <time.h>
#include "desturbe.h"
#include "editor.h"
#include "screenpart.h" 
#include "datatypes.h"
#include "trace.h"
#include "main.h"
#include "system.h"
#include "marks.h"
#include "inout.h"
#ifdef USETHREADS
#define __GNU_VISIBLE 1
#include <pthread.h>
#endif
#include "readconfig.h"
#include "keyproc.h"

#define editerase()   werase(editscreen)


inline int Editor::printchar(int pos) { 
	if(shouldreinit)
		return -1;
	for(int i=0;i<nrparts;i++) parts[i]->unittoscreen(pos); 
	return 0;
	}
 Editor::~Editor() {
 	if(editpos>=0) {
	INCDEST();
#ifdef USETHREADS
		if(backgroundsearch) {
			stopthread();
			}
	sem_destroy(&sema);
#endif
	//	endedit();
	//	dosave();
		for(int i=0;i<nrparts;i++)
			delete parts[i];
		myfree(parts);
		if(replacestr)
			 myfree(replacestr);
#ifdef USE_RE2
		if(re2replacestr)
			 free(re2replacestr);
#endif
		if(searchdataname)
			myfree(searchdataname);
		delundo();
		windel();
		if(searchpart)
			delete searchpart;
		myfree(previouspos);
	DECDEST();
		}
	delete marksptr;
	}

void Editor::splitpos(OFFTYPE pos) {
		editpos=(pos%mem.blocksize())%screenmax();
		filepos=pos-editpos;
		};

extern int makevisible(ScreenPart *part,int editpos);
int Editor::reinit(void) {
	Screenheight=allheight-(!nomessage);
#ifndef SETSCRREGBUG
	wsetscrreg(editscreen,0,Screenheight-1);
#endif
	memorize();
	for(int i=0;i<nrparts;i++)
		parts[i]->calcsizes();

	OFFTYPE pos=editpos+filepos;
	int lines=editpos/cols();
	while(distribute()<0|| (!( view.setshown(screenmax())))) {
	
		if(nrparts>1)
			delete parts[--nrparts];
		else {
			if(nrparts==1 &&parts[0]->getbytes()!=1) {
				delete parts[0];
				parts[0]= new Ascii(this) ;
				nrparts=1;
				}
			else {
				if(nrnumber())
					putnumbase(0);
				else
					return -1;
				}
			}
		}
	if(editpos<0||editpos>=view.getshown()) {
		if(lines>=rows())
			lines=rows()-1;
		for(filepos=pos-lines*cols();filepos<0;filepos+=cols())	
			;
		editpos=pos-filepos;
		if(editpos<0||editpos>=view.getshown()) {
			editpos=0;
			}
		}
	if(filepos<0||(filepos+editpos)<0||((filepos+editpos)>=filesize())) {
		filepos=filesize()-1;editpos=0;
		}
	editpos=lowborder();
	shouldreinit=0;
	dofromfile=1;
	visualwidth=Screenwidth;
	visualunit=visualwidth/8+1;
	visualoffset=(visualoffset/visualunit);
	visualoffset*=visualunit;
	makevisible(parts[mode],editpos);
	fromfile();
	parts[mode]->select();
	for(int i=0;i<nrparts;i++)
		parts[i]->mklabel();
//	editerase();
	makevisible(parts[mode],editpos);
	writebuf();
	shouldreinit=0;
	wrefresh(editscreen);
	return 0;

	}


void Editor::cursorpos(void) {
	if(shouldreinit)
		return;
	if((startactive+fromtop)==editfocus) 
		{
		for(int i=0;i<nrparts;i++)
			parts[i]->cursorpos(editpos);
		}
	else {
		nocursor();
		}
	oldpos=editpos;
	};

extern int leftvis(ScreenPart *part);
extern int rightvis(ScreenPart *part);

const int spaceafternum=1;
int Editor::nrnumber(void) {if(numbase)
				return maxnumstr+spaceafternum;
			else
				return 0;
		};
#define UNUSEDVALUE(x) (void) x
void Editor::number(void) {
	if(numbase&&visualoffset==0) {
		NORMAAL;
//s/shownbufsize/view.getshown()/g
		int endy=(view.getshown()+cols()-1)/cols()+1;
		int y;
		for(y=0;y<endy;y++) {
			wmove(editscreen,y,startnum());
			wprintw(editscreen,numstr,filepos+y*cols());

#ifdef LINESBETWEEN
		        UNUSEDVALUE(wattrset(editscreen,BETWEENPARTSATTR));
#endif
#if defined(SPACEBETWEEN) || defined(LINESBETWEEN)
			for(int i=0;i<spaceafternum;i++)
				waddch(editscreen,' ');
#endif
#ifdef LINESBETWEEN
			NORMAAL;
#endif
			}
		}
	}
void Editor::numline(int y) {
	if(numbase&&visualoffset==0) {
		wmove(editscreen,y,startnum());
		wprintw(editscreen,numstr,filepos+y*cols());

#ifdef LINESBETWEEN
		        UNUSEDVALUE(wattrset(editscreen,BETWEENPARTSATTR));
#endif
#if defined(SPACEBETWEEN) || defined(LINESBETWEEN)
			for(int i=0;i<spaceafternum;i++)
				waddch(editscreen,' ');
#endif
#ifdef LINESBETWEEN
			NORMAAL;
#endif
	}
	}
int Editor::writebuf(void) {
	makevisible(parts[mode],editpos);
	editerase();
	int co=cols();
#ifdef SPACEBETWEEN
	const int endy=minnum(((view.getshown()+co-1)/co)+1,rows());
#endif
#ifdef NOBACKGROUND
	int starty=(view.getshown()/co);
	normaal(editscreen); 
	for(int y=starty;y<nry;y++) {
		wmove(editscreen,y,0);
		for(int x=0;x<Screenwidth;x++)
			waddch(editscreen,' ');
		}
#endif
	
	number(); 
	int i;			
	for(i=0;i<nrparts;i++) {
		ScreenPart *part=parts[i];
			if(part->left>=(visualoffset+visualwidth))
				goto leave;
			if((part->left +((part->nrx()/part->getbytes())*(part->maxres+part->apart))-part->apart+part->spaceafter) <visualoffset)
				continue;

			DEBUGGING("writing %d ",i);
#ifdef SPACEBETWEEN
			if(i>0&&(part->left-visualoffset)>0&&(part->left-visualoffset)<visualwidth) {
				const int startx=maxnum(part->left-visualoffset-parts[i-1]->spaceafter,0);
				const int endx=minnum(part->left-visualoffset,visualwidth);
#ifdef LINESBETWEEN
		UNUSEDVALUE(wattrset(editscreen,BETWEENPARTSATTR));
#endif
				for(int y=0;y<endy;y++) {
					wmove(editscreen,y,startx);
					for(int x=startx;x<endx;x++)
						waddch(editscreen,' ');
					}

				}
#endif
			part->rewritepart( view.getshown()) ;
			}
	leave:

#ifdef NOBACKGROUND
	if((i-1)>=0) {
		ScreenPart *part=parts[i-1];
		int end=((nrx/part->bytes)*(long long)(part->maxres+part->apart))+part->left-visualoffset-part->apart
;
		normaal(editscreen); 
		if(end<visualwidth) {
#if defined(SPACEBETWEEN)&&defined(LINEAFTER)
#ifdef NOLASTLINE
int lastend=minnum(end+((i<nrparts)?part->spaceafter:0),visualwidth);
#else
int lastend=minnum(end+part->spaceafter,visualwidth);
#endif
#endif
//			for(int y=0;y<rows();y++) {
			for(int y=0;y<endy;y++) {
				int x;
				wmove(editscreen,y,end);
#if defined(SPACEBETWEEN)&&defined(LINEAFTER)
#ifdef LINESBETWEEN

		UNUSEDVALUE(wattrset(editscreen,BETWEENPARTSATTR));

#endif
		if(y<endy) {
			for(x=end;x<lastend;x++)
				waddch(editscreen,' ');
			}
		else
		  x=lastend;
#ifdef LINESBETWEEN

		normaal(editscreen); 

#endif
#else
			x=end;
#endif
				for(;x<visualwidth;x++)
					waddch(editscreen,' ');
				}
			}
		}
#endif
	cursorpos();
	if(!nomessage)
		showmessage();
	updated=1;
	return 0;
	}

extern int  partaddch(ScreenPart *part,int ch) ;
void Editor::putline(int y) {
	wmove(editscreen,y,0);
	normaal(editscreen); 
#ifdef NOBACKGROUND
	for(int x=0;x<visualwidth;x++)
		waddch(editscreen,' ');
#else
	wclrtoeol(editscreen);
#endif
	if(numbase)
		numline(y);
	int i;
	for(i=0;i<nrparts;i++) {
		ScreenPart *part=parts[i];
		if(part->left>=(visualoffset+visualwidth))
			break;
		if((part->left +((part->nrx()/part->getbytes())*(part->maxres+part->apart))-part->apart+part->spaceafter) <visualoffset)
				continue;
		part->putline(y);
#ifdef SPACEBETWEEN
		if(i<(nrparts-1)&&((y+1)*nrx)<=view.getshown()) {
	#endif
	#ifdef LINESBETWEEN
			UNUSEDVALUE(wattrset(editscreen,BETWEENPARTSATTR));
	#endif
	#ifdef SPACEBETWEEN
			for(int x=0;x<part->spaceafter;x++)
				partaddch(part,' ');
			}
#endif
		   }
/*	
#ifdef NOBACKGROUND
	if((i-1)>=0) {
		ScreenPart *part=parts[i-1];
		int end=((nrx/part->bytes)*(long long)(part->maxres+part->apart))+part->left-visualoffset-part->apart;
		normaal(editscreen); 
		if(end<visualwidth) {
			wmove(editscreen,y,end);
			for(int x=end;x<visualwidth;x++)
				waddch(editscreen,' ');

			}
		}
#endif
*/
	}



extern char showfileinfo;
	#include "execproc.h"

static int previousposmax=50;
extern int wantsignal;

#ifndef NOSIGNALS
extern void omnihandler(int signum) ;
extern void actionsignal(int sig) ;
#else
#define actionsignal( sig) 
#endif
#define testmarks
int Editor::processkeyonce(void) {
	int key,ret;
	void  *procedure;
		key=ggetch(editscreen);
		testmarks
		if(!commandmode) {
			while(((parts[mode]->addchar(key))>=0))  {
				changed=1;
				if(showfileinfo) {
					datainfo();
					showfileinfo=0;
					}
			 	showposition();
				wrefresh(editscreen);
				return key;
			//	key=ggetch(editscreen);
				}
		if(previouspos[previouspositer%previousposmax]!=(editpos+filepos)) {
			previousposhead=++previouspositer;
			previouspos[(previouspositer%previousposmax)]=editpos+filepos;
			}
			}
		while(!(procedure=lookkey(keyedit,key))) {
		testmarks
		DEBUGGING("once voor endkey");
			if(endkey(keyedit)) {
				beep();
				flushinp();
				return -1;
				}
		DEBUGGING("NA");
			key=ggetch(editscreen);
			}
		testmarks
		traceproc(this,procproc(keyedit,procedure) );
		ret= callproc(procproc(keyedit,procedure) );	
	toscreen();
if(wantsignal) {
	int oldsig	=wantsignal;
	wantsignal=0;
	actionsignal(oldsig) ;
	}
		if(justopened==1) {
			justopened=2;
			if(execones) {
				execones->executeinitproc(this);
				delete execones;
				execones=NULL;
				}
			executeinitproc();
			toscreen();
			}

		if(previouspos[previouspositer%previousposmax]!=(editpos+filepos)) {
			previousposhead=++previouspositer;
			previouspos[(previouspositer%previousposmax)]=editpos+filepos;
			}
	return ret;
	}


extern char *macrofile;

int Editor::processkey(void) {
	int key,ret;
	void  *procedure;

		toscreen();
		if(execswitch)
			execswitch->executeinitproc(this);
		if(justopened==1) {
			justopened=2;
			if(execones) {
				execones->executeinitproc(this);
				delete execones;
				execones=NULL;
				}
			if(execopen)
				execopen->executeinitproc(this);
			if(macrofile) {
				playmacrofile(macrofile); 
				macrofile=NULL;
				}
			}
	do {
if(wantsignal) {
	int oldsig	=wantsignal;
	wantsignal=0;
	actionsignal(oldsig) ;
	}
		toscreen();

		if(previouspos[previouspositer%previousposmax]!=(editpos+filepos)) {
			previousposhead=++previouspositer;
			previouspos[(previouspositer%previousposmax)]=editpos+filepos;
			}
		key=ggetch(editscreen);
		if(!commandmode) {
			while(((parts[mode]->addchar(key))>=0))  {
				changed=1;
				if(showfileinfo) {
					datainfo();
					showfileinfo=0;
					}
			 	showposition();
				wrefresh(editscreen);
				key=ggetch(editscreen);
				}
		if(previouspos[previouspositer%previousposmax]!=(editpos+filepos)) {
			previousposhead=++previouspositer;
			previouspos[(previouspositer%previousposmax)]=editpos+filepos;
			}
			}

		testmarks
		while(!(procedure=lookkey(keyedit,key))) {
DEBUGGING("voor endkey ");
		testmarks
			if(endkey(keyedit)) {
				beep();
				flushinp();
				return -1;
				}

DEBUGGING("na ");
			key=ggetch(editscreen);
			}
		testmarks
		traceproc(this,procproc(keyedit,procedure) );
DEBUGGING("voor callproc");
		ret= callproc(procproc(keyedit,procedure) );	
DEBUGGING("NA");
		testmarks
		} while(edits[editfocus]==this);
	return ret;
	}


int  Editor::altatkey(void) {
	void  *procedure;
	unsigned char keys[2]={27,0};
	output("Alt-");
	keys[1]=ggetch(editscreen);
	message("Alt-%c",keys[1]);
	erefresh();
DEBUGGING("altatkey ");
	for(int i=0;!(procedure=lookkey(keyedit,keys[i++]));) {
		if(endkey(keyedit)||i==2) {
			DEBUGGING("endkey ");
			beep();
			flushinp();
			return -1;
			}
		DEBUGGING("n");
		}
	DEBUGGING("callproc ");
traceproc(this,procproc(keyedit,procedure) );
return callproc(procproc(keyedit,procedure) );	
	}
Editor::Editor(const char *filename): mem(filename),view(mem),nrx(1)
	
	,searchpos(0)
	,table(tablebase-SCHAR_MIN)
#if defined(USETHREADS)
,backgroundsearch(0)

	
#endif	
#ifdef USE_RE2
	,re2pattern(NULL)
#endif
#ifdef USE_HYPERSCAN	
,hyperdata(NULL)
,hyperscratch (NULL)
#endif
	
	{
	editscreen=NULL;
	if(!mem.error()) {
		searchpart=NULL;
#ifdef USETHREADS
		backgroundsearch=0;
		found=0;
		sem_init(&sema,0,1);
#endif
		justopened=1;
		replacestr=NULL;
		replacestrlen=0;
		indatatype=0;
		commandmode=0;
		nomessage=0;
		fromtop=0;
		nrparts=0;
		maxparts=10;
//		parts=new (ScreenPart *)[maxparts];
 		parts=myallocar(ScreenPart *,maxparts);
		parts[nrparts++]= new MultiDigit(this,16,1);
		parts[nrparts++]= new Ascii(this) ;
		mode=nrparts-1;

		initundo();
		oldpos=0;
		putnumbase(10);
		filepos=0;
		modified=0;
		changed=0;
		editpos=0;
		updated=0;
		lastgotopos=filesize()-1;

		resetkey(keyedit)	;
		waitinput=0;

		//selects[0]=selects[1]=0;
		setselect(OFFTYPEINVALID);
		el=0;
		shouldreinit=1;
	//	viewbuf=NULL;
		formatsearch=NULL;
		searchdataname=NULL;
		visualwidth=40,visualoffset=0,virtualwidth=80, visualunit=20;
		previouspos=myallocar(OFFTYPE,previousposmax);
		previouspos[0]=0;
		previouspositer=0;
		previousposhead=0;
		}
	else {
		editpos=-1;
		}
	};

OFFTYPE Editor::getpreviouspos(void) {
	return previouspos[--previouspositer];
	}
int Editor::prevpos(void) {
	if(previouspositer>0&&previouspositer>(previousposhead-previousposmax+1)) {
		return toborderpos(previouspos[(--previouspositer)%previousposmax]);
		}
	beep();
	return -1;
	}
int Editor::nextpos(void) {
	if(previouspositer<previousposhead) 
		return toborderpos(previouspos[(++previouspositer)%previousposmax]);
	beep();
	return -1;
	}
int Editor::distribute(void) {
	int i;
	int start=nrnumber();
	nrx=getcols();
	if(nrx<1)
		return -1;
	if(mode>=nrparts)
		mode=nrparts-1;
	nry=Screenheight;
	for(i=0;i<(nrparts-1);i++) {
		 parts[i]->placepart(start, top());
		int items=(nrx/parts[i]->bytes);
		 start+=parts[i]->maxres*items+parts[i]->apart*(items-1)+parts[i]->spaceafter;
//		 start+=(parts[i]->maxres+parts[i]->apart)*(nrx/parts[i]->bytes)+parts[i]->spaceafter;
		 }
	parts[i]->placepart(start, top());
	return 0;	
	}
int Editor::changestr(int inpos,unsigned char *str,int size) {
	int half=parts[mode]->half;
	OFFTYPE pos=filepos+inpos;
	char *viewpos= view.view()+inpos;
	if(!(undopos()==(pos+size)&&half>undohalf)) 
		appendstrtoundo(pos,viewpos,size);
//		appendstrtoundo(pos,viewbuf+inpos,size);
	undohalf=half;
	changed=1;
	memmove(viewpos,str,size);
	if(shouldreinit)
			return 1;
	for(int i=0;i<nrparts;i++) 	{
		ScreenPart *part=parts[i];
		if(part->left>=(visualoffset+visualwidth))
			return 1;
		if((part->left +(((long long)part->nrx()*(part->maxres+part->apart))/part->bytes)) <visualoffset)
			continue;
		part->oppos(inpos, size) ;
		}

	return 1;
	}


void Editor::nocursor(void) {
	for(int i=0;i<nrparts;i++)
		parts[i]->nocursor();
	}

int Editor::datainfo(void) {
	char str[256];
	int strend=0;
#ifdef USETHREADS
	if(backgroundsearch)
		sprintf(str+strend,"%s %s (%d)",parts[mode]->label,formatsearch,found);
	else
#endif
		strcpy(str+strend,parts[mode]->label);

	realoutput(str);
	return 0;
	}

int Editor::fileinfo(void) {
	const int maxstr=256;
	char str[maxstr];
	const int filenamelen=mem.filenamelen;
	int strend;
	memcpy(str,mem.filename,filenamelen);
	str[filenamelen]=' ';
	strend=snprintf(str+filenamelen+1,maxstr-filenamelen-1,numstr,mem.size())+filenamelen+1;
	str[strend++]=' ';
#ifdef USETHREADS
	if(backgroundsearch)
		snprintf(str+strend,maxstr-strend,"%s %s (%d)",parts[mode]->label,formatsearch,found);
	else
#endif
		strcpyn(str+strend,parts[mode]->label,maxstr-strend);

	realoutput(str);
	return 0;
	}

