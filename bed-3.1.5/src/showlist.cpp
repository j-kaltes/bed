#include "defines.h"
/*     Bed a Binary EDitor for Linux and for Rxvt running under Linux.       */
/*     Copyright (C) 1998  Jaap Korthals Altes <binaryeditor@gmx.com>      */
/*                                                                           */
/*     Bed is free software; you can redistribute it and/or modify           */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     Bed is distributed in the hope that it will be useful,                */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with bed; if not, write to the Free Software                    */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/*                                                                           */
/* Sun Dec  6 18:34:27 1998                                                  */

#ifdef __CYGWIN__
#include <windows.h>
#endif
#include <features.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>
#include <alloca.h>
#undef isprint
#define isprint(x) ((x)>31&&(x)<127)
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef __MINGW32__
       #include <pwd.h>
             #include <grp.h>
              #include <sys/types.h>
#endif
	      #include <assert.h>
#include "dialog.h"
#include "screen.h"
#include "system.h"
#include "readconfig.h"
#include "onlykey.h"
#include "keyinput.h"
#include "myalloc.h"
#define trefresh(x)




extern WINDOW *rightwin(int l,int c,int by,int bx) ;
#include "minmax.h"
extern char spatiebuf[];
template <typename Init,typename Base>
class showlist :public Base{
public:
using typename Base::Pos;
private:
using Base::next;
using Base::prev;
using Base::startpos;
using Base::endpos;
using Base::startscreen;
using Base::endscreen;
using Base::data;
Pos *possel;
const char *prompt;
	int h,l,beg,x,y,sel,endscreenpos;
	void rewrite();
	void showsel() ;
void unshowsel(void) ;
	void start();
	void end();
	void up();
	void down();
	void pageup();
	void pagedown();

auto  selectindex(int key) -> Pos *;
//typename	Base::Pos  *selectindex(int key) ;

inline void printline(const int down,const char *str) {
		mvwaddnstr(takescr,down+y,x,str,l);
		};
public:

	showlist(const char *prompt,Init init,int xpos=1,int ypos=1,int begin=0,int height=15,int len=60) : Base(init),
		prompt(prompt),
		h(height-1),
		l(len-2),
		beg((begin>h)?(begin-h)/2:0),
		x(xpos),
		y(ypos),
		sel(begin-beg)

 {
		int begy=(thelines-height)/2;
		int begx=(thecols-len)/2;
		rightwin(height,len,maxnum(begy,0),maxnum(begx,0));
/*
		if(!takescr)
			return INT_MAX;
*/
		werase(takescr);
		 };
	~showlist() {}

	Pos  *selectloop(void) ;
	};




template <typename I,typename B>
void showlist<I,B>::unshowsel(void) {
	SELCOLOR;
	trefresh(takescr);
	printline(sel,spatiebuf); 
	printline(sel,data(possel)); 
	trefresh(takescr);
	};




template <typename I,typename B>
void showlist<I,B>::showsel(void) {
	SELSELCOLOR;
	trefresh(takescr);
	printline(sel,spatiebuf); 
	printline(sel,data(possel)); 
	trefresh(takescr);
	};
template <typename I,typename B>
void showlist<I,B>::rewrite(void) {	
	Pos *tmp=startscreen;
	SELCOLOR;
	trefresh(stdscr);
	mvwvline(takescr,y,x-1,0, h); /*Without these lines on ncurses error sometimes meshed up the screen. Showmarks after list search at the end of the file*/
	mvwvline(takescr,y,x+l,0, h);
	for(int i=0;i<h;i++) {
		printline(i,spatiebuf); 
		}
int i=0;
	for(;;i++,tmp=next(tmp)) {
		if(i==sel) {
			possel=tmp;
			SELSELCOLOR;
			trefresh(takescr);
			printline(i,spatiebuf); 
			printline(i,data(tmp)); 
			SELCOLOR;
			trefresh(stdscr);
			}
		else
			printline(i,data(tmp)); 
		if(tmp==endpos||i>=(h-1))
			break;
		}
	endscreen=tmp;
	endscreenpos=i;
	if(sel<0||sel>endscreenpos)			 {
		possel=endscreen;
		sel=endscreenpos;
		showsel();
		}
	}
template <typename I,typename B>
void showlist<I,B>::pagedown(void) {	
		startscreen=endscreen;
		rewrite();
		};
template <typename I,typename B>
void showlist<I,B>::pageup(void) {	
		Pos *tmp=startscreen;
		int endscr=h-1;	
		for(int i=0;i<endscr&&tmp!=startpos;i++)
			tmp=prev(tmp);
		startscreen=tmp;
		rewrite();
		}
template <typename I,typename B>
void showlist<I,B>::down(void) {
	if(possel==endpos) {
		beep();
		showsel();	
		return;
		}
	if(possel!=endscreen)   {
		possel=next(possel);
		sel++;
		showsel();	
		return ;
		}
	else {
		if(startscreen!=endpos) {
			startscreen=next(startscreen);
			rewrite();
			}
		else
			beep();
		}
	};
template <typename I,typename B>
void showlist<I,B>::up(void) {
	if(possel==startpos) {
		beep(); 
		showsel();
		return;
		}
	if(possel!=startscreen) {
		possel=prev(possel);
		sel--;
		showsel();
		return;
		}
	startscreen=prev(possel);
	rewrite();
	}
template <typename I,typename B>
void showlist<I,B>::end(void) {
	if(possel==endpos) { beep();showsel();return; }
	possel=endpos;
	if(endpos==endscreen) { 
		sel=endscreenpos;
		showsel();
		return; 
		}
	Pos *tmp=endscreen=endpos;
	for(int i=h-2;i>=0;i--) {
		tmp=prev(tmp);
		printline(i,spatiebuf); 
		printline(i,data(tmp)); 
		}
	startscreen=tmp;
	sel=h-1;
	showsel();
	}
template <typename I,typename B>
void showlist<I,B>::start(void) {
	sel=0;
	if(startpos==startscreen) {
		possel=startpos;
		showsel();
		return;
		};
	startscreen=startpos;
	rewrite();
	}
#include "getlabel.h"
#include "main.h"
#include "keylabels.h"
template <typename I,typename B>
//typename showlist<I,B>::Pos *showlist<I,B>::selectindex(int key) {
//typename B::Pos *showlist<I,B>::selectindex(int key) {
auto showlist<I,B>::selectindex(int key) -> Pos *{
	unshowsel() ;
	switch(key) {
		case RETURN:  return possel;
		case DOWN: down();break;
		case UP: up();break;
		case END: end();break;
		case BEG: start();break;
		case PGDN: pagedown();break;
		case PGUP: pageup();break;
		default: showsel();
		};
	return nullptr;
	}



#include <stdint.h>
extern keyinput inputkeys;

extern void helpwin(void);
template <typename I,typename B>
//typename B::Pos *showlist<I,B>::selectloop(void) {
auto showlist<I,B>::selectloop(void) -> Pos *{
	if(!takescr)
		return  nullptr;

	trefresh(takescr);
	TITLECOLOR ;
	 mvwaddnstr( takescr,y-1,x-1,spatiebuf,l+2);
	 mvwaddnstr( takescr,y-1,x,prompt,l+1);
	rewrite();	
	while(1){
		wrefresh(takescr);
		int key=ggetch(takescr);
		if(!isprint(key)) {
			while((key=inputkeys.keyseq(key))==-1)
					key=ggetch(takescr);
			switch(key) {
				case -2: beep();continue;
				case HELP:
					helpwin();
					continue;
				case 	QUIT: return nullptr;
				case NOP:continue;
			  };
			}
		switch(key) {
			case	REFRESH: return reinterpret_cast<Pos *>(UINTPTR_MAX);
			default: if(Pos *nameindex=selectindex(key)) {
				return nameindex;
				}
			};
		}
	};
#include "marks.h"
#include "screenpart.h"
struct marklister {
	ScreenPart *part;	
	keylookup *look;
	char *numstr;
	int maxnumstr;
	};
	/*
class showmark  {
ScreenPart *part;
char *numstr;
const int bymax;

protected:
using Pos = keylookup::mark;
char *data(Pos *m) {
static constexpr int maxbuf=256;
static char buf[maxbuf];
	int slen=part->bin2search((unsigned char *)m->des, buf, bymax)  ;
	for(int s=minnum(slen,bymax);s<bymax+3;s++)
			buf[s]=' ';
	snprintf(buf+bymax+3,maxbuf-bymax-3,numstr  ,m->pos); 
	return buf;
	};
Pos *next(Pos *m) {
	return m->next;
	};
Pos *prev(Pos *m) {
	return m->prev;
	};
Pos *startpos,*endpos,*startscreen,*endscreen;
	showmark(marklister gegs): part(gegs.part), numstr(gegs.numstr),bymax(MAXMARKALL-3-gegs.maxnumstr),
	startpos(gegs.look->begin.next), endpos(gegs.look->head),startscreen(gegs.look->iter->next) {};
};
*/

class showmark  {
ScreenPart *part;
//keylookup *look;
char *numstr;
const int bymax;

protected:
using Pos = keylookup::mark;
char *data(Pos *m) {
static constexpr int maxbuf=256;
static char buf[maxbuf];
	int slen=part->bin2search((unsigned char *)m->des, buf, bymax)  ;
	for(int s=minnum(slen,bymax);s<bymax+3;s++)
			buf[s]=' ';
	snprintf(buf+bymax+3,maxbuf-bymax-3,numstr  ,m->pos); 
	return buf;
	};
Pos *next(Pos *m) {
//	assert(m!=look->head);	
	return m->next;
	};
Pos *prev(Pos *m) {
//	assert(m->prev!=look->head);	
	return m->prev;
	};
Pos *startpos,*endpos,*startscreen,*endscreen;
//	showmark(marklister gegs): part(gegs.part), look(gegs.look),numstr(gegs.numstr),bymax(MAXMARKALL-3-gegs.maxnumstr),
	showmark(marklister gegs): part(gegs.part),numstr(gegs.numstr),bymax(MAXMARKALL-3-gegs.maxnumstr),
	startpos(gegs.look->begin.next), endpos(gegs.look->head),startscreen(gegs.look->iter==endpos?endpos:gegs.look->iter->next) {};
};



#include "editor.h"


template <> struct keylookup::mark *Editor::markselect<2>(const char *prompt) {
		
if(backgroundsearch)
	output("Background search waits before adding marks during marks viewing");
#ifdef USETHREADS
	sem_wait(&sema);
#endif

	ScreenPart *activepart=parts[mode]->getsubpart();
	showlist<marklister,showmark> shower(prompt,{activepart,marksptr,numstr,maxnumstr});
		
	struct keylookup::mark *mark=nullptr;
	while((mark=shower.selectloop()) == reinterpret_cast<struct keylookup::mark *>(UINTPTR_MAX)) {
		menuresized();
		wrefresh(editscreen);
		}
#ifdef USETHREADS
sem_post(&sema);
#endif
	editup();
	return mark;
	}
//template  struct keylookup::mark *Editor::markselect<1>(const char *prompt) ;
//
template <> struct keylookup::mark * Editor::markselect<0>(const char *prompt) {
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	unsigned int total= marksptr->gettotal();
#ifdef USETHREADS
sem_post(&sema);
#endif
	if(total<=0)  {
		output("0 marks, nothing to show");
		return  nullptr;
		}

extern unsigned long long showmarkswhole; 
if(backgroundsearch&& (total<showmarkswhole))

	return markselect<1>(prompt);
else
	return markselect<2>(prompt) ;
}

