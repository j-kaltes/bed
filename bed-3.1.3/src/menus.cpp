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
#include <ctype.h>
#include <strings.h>
#include "editor.h"
#include "main.h"
#include "inout.h"

#include "screen.h"
#include "cursesinit.h"
#include "readconfig.h"
#include "system.h"
#include "keyproc.h"
#include "trace.h"
#include "varmacro.h"
#include "desturbe.h"
#ifdef OLDGCCVARMACRO
#define menumake(under...) 'x',
#else
#define menumake(...) 'x',
#endif
static const int standardmenusize=sizeof((char []){
#include "menu.h"
	});
const int maxtomenu=maxmenus;
#ifdef PRINTMAXSHOWMENUS
#include <stdio.h>
int main(void) {
	printf("%d",maxtomenu-standardmenusize);
	return 1;
	}
#else
static WINDOW *barwin;
//#define EDITFROMMENU 1
#include "varmacro.h"
	#ifdef OLDGCCVARMACRO
	#define hideprocedure(a...) VARMACRO
	#else
	#define hideprocedure(...) 
	#endif	
	#ifdef EDITFROMMENU
	#define setkeymapedsecond hideprocedure
	#else
	#define setkeymapedsecond setkeymaped
	#endif
#define inbarkey(m) tolower((m)->name[(unsigned char)((m)->keypos)])
struct Editor::menu *Editor::menulist[maxmenus];
int Editor::menuiter=0;
int Editor::menunu=-1;
int startbar;
int maxmenusize=0;
static int keyinmenu='q';
int tomenu[maxtomenu];
int tomenunr=0;
extern int therewrite(void) ;
int therewrite(void) {
	curses_init_curses.reset();
	if(MainScreenwidth>startbar){
	#ifdef HAS_WRESIZE
			wresize(barwin,1,MainScreenwidth);
	#endif
		OldMainScreenwidth=MainScreenwidth;
		}
	redistribute=1;
	return 0;
	}
int Editor::rewrite(void) {
	return therewrite();
	}
static int showsmenu=0;
void Editor::editontop(void) {
	#ifdef wegPDCURSES
		if(curses_init_curses.isresized()) 
			resizeflag=1;
	#endif
	if(showsmenu) {
		for(int i=startactive;i<startnonactive;i++) {
			if(edits[i]->edittop>(maxmenusize+3))
				break;
			touchwin(edits[i]->editscreen);
			wrefresh(edits[i]->editscreen);
			}
		showsmenu=0;
		}
	}

void delmenus(void) {
INCDEST();
	if(barwin) {
		WINDOW *tmpbar=barwin;
		barwin=NULL;
		delwin(tmpbar);
		}
	for(int i=0;i<Editor::menuiter;i++) {
		if(Editor::menulist[i]) {
			if(Editor::menulist[i]->win) {
				WINDOW *tmpwin= Editor::menulist[i]->win;
				Editor::menulist[i]->win=NULL;
				delwin(tmpwin);
				}
			}
		/*
		myfree(Editor::menulist[i]->el);
		myfree(Editor::menulist[i]);
		*/
		}
//	Editor::menuiter=0;
	//
DECDEST();
	}
#define savefree(x) {void *tmp=(void *)x;x=NULL;free(tmp);}
int upmenus(void) {
	return Editor::menus() ;
	}
#undef menumake

#define menuprocbody(value) {\
	int get=tomenu[(value)];\
	if(get>=0&&get<menuiter)	{\
		menunu=get;\
		return showmenu();\
		}\
	return -1;\
	}

#define standardmenuproc(name)\
	short name##menupos;\
	int Editor::name ## show(void) menuprocbody(name##menupos)

#undef menumake
	#ifdef OLDGCCVARMACRO
	#define menumake(name,under...) standardmenuproc(name)
	#else
	#define menumake(name,...) standardmenuproc(name)
	#endif
#include "menu.h"

#undef menumake


#define mkmenunum(num) int Editor::menu##num##show(void) menuprocbody(standardmenusize+num)
#include "mkmenus.h"
#undef standardmenuproc
#define standardmenuproc(name) funcptr(Editor,name ## show),
#undef menumake
	#ifdef OLDGCCVARMACRO
	#define menumake(name,under...) standardmenuproc(name)
	#else
	#define menumake(name,...) standardmenuproc(name)
	#endif
#undef mkmenunum
#define mkmenunum(num) funcptr(Editor,menu##num##show),
static int (Editor::*showmenus[maxtomenu])(void)={
#include "menu.h"
#include "mkmenus.h"
};
static int statepos=0,statestart;
char statesafter[100];

#define UNUSEDVALUE(x) (void) x
//s/wattrset([^)]*)/UNUSEDVALUE(&)/g
int showstatestate(const char *str,short len,short pos) {
	if(pos<0)
		pos=statepos;
	UNUSEDVALUE(wattrset(barwin,BARATTR));
	mvwaddnstr(barwin,0,statestart+pos,str,len);
	memcpy(statesafter+pos,str,len);
	wrefresh(barwin);
	statepos+=len;
	return statepos;
	}
#define menuline(menuitem) ((menuitem).key==0)

int Editor::menus(void) {
INCDEST();
	delmenus();
	startbar=0;
	barwin=newwin(1,MainScreenwidth,0,0);
	if(!barwin) {
		DECDEST();
		return -1;
		}
	//wresize(barwin,1,MainScreenwidth);
	HIDECURSOR;
	OldMainScreenwidth=MainScreenwidth;
	attrall(barwin,BARATTR);
	wmove(barwin,0,0);
	for(int i=0;i<menuiter;i++)	{
		if(menulist[i]) {
			rewritemenu(menulist[i]);
			addtobar(menulist[i]) ;
			}
		}
	statestart	=startbar;
	UNUSEDVALUE(wattrset(barwin,BARATTR));
	mvwaddnstr(barwin,0,statestart, statesafter,statepos);
	UNUSEDVALUE(wattrset(barwin,BARATTR));
	wbkgdset(barwin,' '|BARATTR);
	wrefresh(barwin);
	DECDEST();
	return 0;
	}
int Editor::menustructs(void) {
	startbar=0;
	maxmenusize=0;
	menuiter=-1;
	#ifdef OLDGCCVARMACRO
	#undef menumake
	#undef elm
	#undef elk
	#undef LINE
	#define menumake(name,under,items...)  char start##name[] __attribute__ ((unused)) ={'a', ##items };

	#define elm(name,proc)    'b'},* proc##var=name,proc##nonsensevar[] __attribute__ ((unused)) ={'a'
	#define elk(name,key,proc)  'b'},* proc##var=name,proc##nonsensevar[] __attribute__ ((unused)) ={'a'
	#define LINE 'c'
	#include "menu.h"
	#undef elm
	#undef elk
	#undef LINE
	#undef menumake
	//#define menumake(name,under,items...) if(!(menulist[++menuiter]=menumk(startbar,sizeof((struct Menugeg []){##items})/sizeof(struct Menugeg),((struct Menugeg []){##items}),under))) return -1; setkeymaped(keyedit,funcptr(Editor,name ## show),2,27,inbarkey(menulist[menuiter]));name##menupos=menuiter;
	#endif

	#ifdef OLDGCCVARMACRO
	#define menumake(name,under,item,items...) struct Menugeg name##menugegs[]={item,##items}; int name##menusize=sizeof(name##menugegs)/sizeof(struct Menugeg); if(!(menulist[++menuiter]=menumk(startbar,name##menusize,name##menugegs,under))) return -1; setkeymaped(keyedit,funcptr(Editor,name ## show),2,27,keyinmenu=inbarkey(menulist[menuiter]));setkeymapedsecond(keyedit,funcptr(Editor,name ## show),3,253,27,keyinmenu); ;name##menupos=menuiter; VARMACRO
	#define elm(name,proc) {proc##var,"",funcptr(Editor,proc)}
	#define elk(name,key,proc) {proc##var,key, funcptr(Editor,proc) }
	#else

	#undef menumake
	#define menumake(name,under,...) struct Menugeg name##menugegs[]={__VA_ARGS__}; int name##menusize=sizeof(name##menugegs)/sizeof(struct Menugeg); if(!(menulist[++menuiter]=menumk(startbar,name##menusize,name##menugegs,under))) return -1; setkeymaped(keyedit,funcptr(Editor,name ## show),2,27,keyinmenu=inbarkey(menulist[menuiter]));setkeymapedsecond(keyedit,funcptr(Editor,name ## show),3,253,27,keyinmenu); ;name##menupos=menuiter; 

	#define elm(name,proc) {name,"",funcptr(Editor,proc)}
	#define elk(name,key,proc) {name,key, funcptr(Editor,proc) }
	#endif

#define LINE {NULL,"",funcptr(Editor,nop)}


#include "menu.h"
#undef elm
#undef elk
#undef LINE
#undef menumake
	statestart	=startbar;
	
	menuiter++;
	tomenunr=menuiter;
	for(int i=0;i<tomenunr;i++)
		tomenu[i]=i;
	return 0;
	}


int Editor::addtobar(struct Editor::menu *m) {
	char *name=m->name;
	int keypos=m->keypos;
	wmove(barwin,0,m->barpos);
	waddnstr(barwin,name,keypos);
	UNUSEDVALUE(wattrset(barwin,BARSTANDATTR));
	int key=name[keypos];
	waddch(barwin,key);
	UNUSEDVALUE(wattrset(barwin,BARATTR));
	waddstr(barwin,	name+keypos+1);
	return tolower(key);
	}
void Editor::selectmenu(struct menu *m,int dir) {
	int maxx=getmaxx(m->win)-2;
	UNUSEDVALUE(wattrset(m->win,MENUATTR));
	wmove(m->win,m->nu+1,1);
	int old=m->nu;
	m->nu+=dir;
	if(dir>0)
		while(!(m->el[m->nu].key))
			m->nu++;
	else
		while(!(m->el[m->nu].key))
			m->nu--;
	if(m->nu>=0&&m->nu<m->len) {
		for(int i=0;i<maxx;i++) {
			chtype ch=winch(m->win);
			if((ch&A_ATTRIBUTES)!=(chtype) MENUKEYATTR)
				waddch(m->win,(ch&A_CHARTEXT));
			else {
				UNUSEDVALUE(wattrset(m->win,MENUKEYATTR));
				waddch(m->win,(ch&A_CHARTEXT));
				UNUSEDVALUE(wattrset(m->win,MENUATTR));
				}

			}
		wmove(m->win,m->nu+1,1);
		UNUSEDVALUE(wattrset(m->win,MENUSELATTR));
		for(int i=0;i<maxx;i++) {
			chtype ch=winch(m->win);
			if((ch&A_ATTRIBUTES)!=(chtype)MENUKEYATTR)
				waddch(m->win,(ch&A_CHARTEXT));
			else {
				UNUSEDVALUE(wattrset(m->win,MENUKEYATTR));
				waddch(m->win,(ch&A_CHARTEXT));
				UNUSEDVALUE(wattrset(m->win,MENUSELATTR));
				}
			}
		}
	else
		m->nu=old;
	}

WINDOW *mkmenuwindow(int lines,int cols,int x) {
	if(((lines+1)<LINES)&&((x+cols)<COLS))
		return newwin(lines,cols,1,x);
	else {
		int bx,h,w;
		if(x>(COLS-(cols)))
			bx=(COLS-(cols));
		else
			bx=x;
		w=cols;
		if(w>COLS) {
			bx=0;
			w=COLS;
			}
		if((lines+1)>LINES)
			h=LINES-1;	
		else
			h=lines;
		return newwin(h,w,1,bx);
		}
	}
//int ls=0,rs=0,ts=0,bs=0,tl=0,tr=0,bl=0,br=0;
//
WINDOW *Editor::rewritemenu(struct Editor::menu *m) {
	int maxname=m->maxname;
	m->barpos=startbar;
	startbar+=strlen(m->name)+2;
	int x=m->barpos;
//	char barkey=toupper(m->key);

	int el=m->len;
	m->win=mkmenuwindow(el+2,maxname+2,x);
	if(!m->win) {
		myfree(m->el);
		myfree(m);
		return NULL;
		}
	attrall(m->win,MENUATTR);
	for(int i=0;i<el;i++) {
		char *iter,*str=m->el[i].name;
		if(!menuline(m->el[i])) {
			int beglen=m->el[i].keypos,klen;
			iter=str+beglen;
			wmove(m->win,i+1,1);
			waddnstr(m->win,str,beglen);
			UNUSEDVALUE(wattrset(m->win,MENUKEYATTR));
			waddch(m->win,*iter);
			UNUSEDVALUE(wattrset(m->win,MENUATTR));
			waddstr(m->win,	iter+1);
			klen=strlen(m->el[i].alt);
			wmove(m->win,i+1,maxname-klen+1);
			waddstr(m->win,	m->el[i].alt);
		//setkeymaped(keyedit,m->el[i].proc,3,27,barkey,tolower(m->el[i].key));
			}
		else {
#ifdef LINEMENUS
			wmove(m->win,i+1,1);
			whline(m->win,0,maxname);
#endif
			}

		}

#ifdef LINEMENUS
//	wborder(m->win,ls,rs,ts,bs,tl,tr,bl,br);
	box(m->win,0,0);
#endif
	selectmenu(m,0);
	return m->win;
	}
static int lastmenunr=standardmenusize-1;

//static int Editor::addmenuitem(int menunr,char *name,int pos, int ( Editor::*proc)(void),int insert=0,char *keystr="") ;
int Editor::addmenuitem(int menunr,const char *name,int pos, int ( Editor::*proc)(void),int insert,const char *keystr) {
	int ret=-1;
	const char *iter=NULL;
	if(name) {
		iter=strchr(name,'~');
		if(!iter)
			return -1;
		}
	if(menunr<0)
		menunr=lastmenunr;
	else
		lastmenunr=menunr;
	if(menunr>=maxmenus)
		return -1;
	if(menunr>=menuiter||!menulist[menunr]) {
		char noname[15];
		sprintf(noname,"menu~%d",menunr);
		if((ret=addmenu(noname, menunr))<0)
			return ret;
		}
	struct menu *m=menulist[menunr];
	if(pos<0) {
		pos=m->len;
		}
INCDEST();
	if(pos>=m->len) {
		m->el=myrealloc(struct menuel*,m->el,struct menuel,(pos+1),m->len);
		for(int i=m->len;i<(pos-1);i++) {
			m->el[i].key=0;
			m->el[i].name=NULL;
			}
		m->len=pos+1;
		}
	else {
		if(insert) {
			m->el=myrealloc(struct menuel*,m->el,struct menuel,(m->len+1),m->len);
			memmove(m->el+pos+1,m->el+pos,sizeof(m->el[0])*(m->len-pos));
			m->len++;
			}
		else {
			if(m->el[pos].key) {
				myfree(m->el[pos].name);
				myfree(m->el[pos].alt);
				}
			}
		}
	if(name) {
		int i=pos;
		int len=strlen(keystr);
		m->el[i].alt=(char *)myallocar(char,len+1);
		strcpy(m->el[i].alt,keystr);
		if(len)
			len+=2;
		m->maxname=maxof(m->maxname,strlen(name)+len);
		const char *str=name;
		if(isupper(*(iter+1)))
			m->el[i].key=tolower(*(iter+1));
		else
			m->el[i].key=(*(iter+1));
		m->el[i].proc=proc;
		int beglen=(iter-str);
		len=strlen(str);
		char *newstr=m->el[i].name=myallocar(char,len);
		memcpy(newstr,str,beglen);
		strcpy(newstr+beglen,iter+1);
		m->el[i].keypos=beglen;
		}
	else {
		m->el[pos].key=0;
		m->el[pos].name=NULL;
		}
DECDEST();
	return 0;
	}
int Editor::deletemenuitem(int menunr,int pos) {
	if(menunr<0)
		menunr=lastmenunr;
	else
		lastmenunr=menunr;
	if(menunr>=menuiter)
		return -1;
	if(!menulist[menunr])
		return -1;
	struct menu *m=menulist[menunr];
	if(pos<0)
		pos=(m->len-1);
	if(pos>=	m->len||pos<0)
		return -1;

INCDEST();
	if(m->el[pos].key) {
		myfree(m->el[pos].name);
		myfree(m->el[pos].alt);
		}
	memmove(m->el+pos,m->el+pos+1,sizeof(m->el[0])*(m->len-pos-1));
	m->len--;
DECDEST();
	return 0;

	}
int Editor::deletemenu(int menunr) {
	if(menunr<0)
		menunr=menuiter-1;
	if(menunr<0||menunr>=menuiter)
		return -1;
	struct menu *m=menulist[menunr];
	if(!m)
		return -1;
INCDEST();
	for(int i=0;i<m->len;i++) {
		if(m->el[i].key) {
			myfree(m->el[i].name);
			myfree(m->el[i].alt);
			}
		}
	myfree(m->el);
	myfree(m->name);
	if(m->win)
		delwin(m->win);
	myfree(m);
	if(menunr<(menuiter-1))	 {
		memmove(menulist+menunr,menulist+menunr+1,sizeof(menulist[0])*(menuiter-menunr-1));
		}
	for(int i=0;i<tomenunr;i++) {
		if(tomenu[i]>=menunr) {
			if(tomenu[i]>menunr) 
				tomenu[i]--;
			else
				tomenu[i]=-1;
			}
		}
	while(tomenu[tomenunr-1]<0)	
		tomenunr--;
	menuiter--;
DECDEST();
	return 0;
	}
int (Editor::*getnewmenuproc(int menunr)) (void) {
	int use=-1;
	for(int i=0;i<tomenunr;i++) {
		if(tomenu[i]<0) {
			use=i;
			break;
			}
		}
	if(use<0) {
		if(tomenunr<maxtomenu) {
			use=tomenunr++;

			}
		else
			return (int (Editor::*)(void))NULL;
		}
	tomenu[use]=menunr;
	return showmenus[use];
	}
int Editor::addmenu(const char *name,int menunr,int insert) {
	if(menunr<0)
		menunr=menuiter;
	if(menunr>=maxmenus) {
		return -1;
		}
INCDEST();
	lastmenunr=menunr;
	struct menu *m;

	if(menunr>=menuiter) {
		for(int i=menuiter;i<=menunr;i++)
			menulist[i]=NULL;
		menuiter=menunr+1;
		}
	else {
		if(insert) {
			if(menuiter>=maxmenus)
				return -1;
			memmove(menulist+menunr+1,menulist+menunr,sizeof(menulist[0])*(menuiter-menunr));
			menulist[menunr]=NULL;
			for(int i=0;i<tomenunr;i++) {
				if(tomenu[i]>=menunr)
					tomenu[i]++;
				}
			menuiter++;
			}
		}
	int (Editor::*proc)(void)=NULL;
	if(menulist[menunr]==NULL) {
		m=menulist[menunr]=myallocp(struct menu *,struct menu);
		m->len=0;
		m->el=myrealloc(struct menuel*,NULL,struct menuel,1,0);
		m->el[0].key=0;
		m->el[0].name=NULL;
		m->nu=0;
		m->maxname=0;
		m->win=NULL;
		}
	else {
		void *procedure=NULL;
		m=menulist[menunr];
		myfree(m->name);
		resetkey(keyedit);
		procedure=lookkey(keyedit,27);
		procedure=lookkey(keyedit,m->key);
		resetkey(keyedit);
		proc=procproc(keyedit,procedure);
		}
	if(!proc)
		proc=getnewmenuproc( menunr);
	const char *iter= strchr(name,'~')+1;
	if(isupper(*iter))
		m->key=tolower(*iter);
	else
		m->key=(*iter);
	m->keypos=iter-name-1;
	int len=strlen(iter)+m->keypos;;
	m->name=(char *)myallocar(char ,len+1);
	memcpy(m->name,name,m->keypos);
	strcpy(m->name+m->keypos,iter);
	if(proc) {
		setkeymaped(keyedit,proc,2,27,m->key);
		setkeymapedsecond(keyedit,proc,3,253,27,m->key);
		}

DECDEST();
	return 0;
	}
static const int MAXKEYNAME=10;
struct Editor::menu *Editor::menumk(int x,int el,struct Menugeg pro[],const char *under) {

	struct menu *m;
	int i;
	int klen;
	int maxname=0;
	m=myallocp(struct menu *,struct menu);
	const char *iter= strchr(under,'~')+1;
	m->key=tolower(*iter);
	m->keypos=iter-under-1;
	int len=strlen(iter)+m->keypos;;
	m->name=(char *)myallocar(char,len+1);
	memcpy(m->name,under,m->keypos);
	strcpy(m->name+m->keypos,iter);

//	char barkey=m->key&0xDF;
	m->len=el;
	m->nu=0;
	m->el=myallocarp(struct menuel*,struct menuel,el);
	m->barpos=x;
	for(i=0;i<el;i++) {
		const char *iter;
		const char *str=pro[i].name;
		if(str) {
			const char *k=pro[i].key;
			m->el[i].key=klen=strlen(k);
			if(klen)
				klen+=2;
			maxname=maxof(maxname,strlen(str)+klen);
			m->el[i].proc=pro[i].proc;
			iter=strchr(str,'~');
			klen=m->el[i].key;
			m->el[i].key=tolower(*(iter+1));
			int beglen=(iter-str);
			m->el[i].alt=(char *)myallocar(char,MAXKEYNAME);
			strcpy(m->el[i].alt,pro[i].key);
			int len=strlen(str);
			char *newstr=m->el[i].name=myallocar(char,(len+1));
			memcpy(newstr,str,beglen);
			strcpy(newstr+beglen,iter+1);
			m->el[i].keypos=beglen;
			}
		else {
			m->el[i].key=0;
			m->el[i].name=NULL;
			}
		}
	maxname--;	
	m->maxname=maxname;
	m->win=NULL;
	maxmenusize=maxof(el,maxmenusize);
	startbar+=(len+2);
	return m;
	}
int Editor::staymenu(void) {
	return -3;
	}
extern int showhelp( int (Editor::*proc)(void)) ;
int  Editor::showhelp(void) {
	struct menu *m=menulist[menunu];
	if(!m)
		return -1;
	::showhelp(m->el[m->nu].proc);
	editup();
	touchwin(m->win);
	return -3;
	}
int Editor::showmenu(void) {
	if(!editscreen)
		return -1;
	editontop();
	int keys[20];
	void *procedure;
	char ch;
	int i,key;
	startover:
	struct menu *m=menulist[menunu];
	if(!m)
		return -1;

	touchwin(m->win);
	wrefresh(m->win);
	showsmenu++;
	goon:
	key=ggetch(m->win);
	wrefresh(m->win);

	if(key==13) {
			editontop();
			traceproc(this,m->el[m->nu].proc);
			return callproc(m->el[m->nu].proc);
			}
	if(isprint(key)) { /*zero can match with line */
		ch=tolower(key);
		for(i=0;i<m->len;i++)
			if(ch==m->el[i].key) {
				editontop();
				traceproc(this,m->el[i].proc);
				return callproc(m->el[i].proc);
				}
		}

	lookkey(keyedit,menukey);
	keys[0]=key;	
DEBUG("showmenu before loop 1 ");
	for(i=0;!(procedure=lookkey(keyedit,keys[i++]));) {
		if(endkey(keyedit)) {
#ifdef EDITFROMMENU
			if(isprint(key)) {
				flushinp();
				goto goon;
				}
DEBUG("before loop 2 ");
			for(int j=0;j<i;j++) {
				if((procedure=lookkey(keyedit,keys[j]))) {
DEBUG("without ggetch ");
					goto away;
	}
				if(endkey(keyedit)) {
					flushinp();
					goto goon;
					}
				}
			while(!(procedure=lookkey(keyedit,ggetch(m->win)))) {
				if(endkey(keyedit)) {
					flushinp();
					goto goon;
					}
				}
DEBUG("found procedure ");
			goto away;
#else
				flushinp();
				goto goon;
#endif
			}
		keys[i]=ggetch(m->win);
		}
	int ret;		
	traceproc(this, procproc(keyedit,procedure) );
DEBUG("before switch ");
	switch((ret=callproc(procproc(keyedit,procedure) ))) {
		case -3: 
			wrefresh(m->win);
			goto goon;
		
		case -4:
			editontop();
			goto startover;
		default:
			editontop();
			return ret;
		};
#ifdef EDITFROMMENU
	away:
	editontop();
	traceproc(this, procproc(keyedit,procedure) );
	return	callproc(procproc(keyedit,procedure) );
#endif
	}

extern char endmenustring[];
extern char showmenustring[];
extern const int endmenulen;
extern const int showmenulen;

#ifdef USE_WITH_XBED
int ismenu=-1;
int Editor::xbedmenu(void) {
	menuon=0;
	ismenu=1;
	redistribute=1;
	return 0;
	}
#endif
int Editor::switchmenu(void) {
	redistribute=1;
#ifdef USE_WITH_XBED
	if(ismenu>=0) {
		if(ismenu==1) {
			menuon=1;ismenu=0;
			write(STDOUT_FILENO,endmenustring,endmenulen);
			return 0;
			}
		else if(ismenu==0&&menuon==0) {
			write(STDOUT_FILENO,showmenustring,showmenulen);
			ismenu=1;
			return 0;
			}
		}
#endif
	menuon=!menuon;
	if(!menuon)
		message("Press Alt-t,u to show menu");
	return 0;
	}
int Editor::prevmenu(void) {
	do {
		menunu--;
		if(menunu<0) 
			menunu=menuiter-1;
		} while(!menulist[menunu]);
	return -4;
	}

int Editor::menuresized(void){			
	resized();
	rewriteall();
	return -4;
	}
int Editor::firstmenuitem(void) {
	selectmenu(menulist[menunu],-(menulist[menunu]->nu));
	return -3;
	}
int Editor::lastmenuitem(void) {
	selectmenu(menulist[menunu],menulist[menunu]->len-menulist[menunu]->nu-1);
	return -3;
	}
int Editor::nextmenuitem(void) {
	if(menulist[menunu]->nu>(menulist[menunu]->len-2))
		selectmenu(menulist[menunu],-(menulist[menunu]->nu));
	else
		selectmenu(menulist[menunu],1);
	return -3;
	}
int Editor::prevmenuitem(void) {
	if(menulist[menunu]->nu==0)
		selectmenu(menulist[menunu],menulist[menunu]->len-1);
	else
		selectmenu(menulist[menunu],-1);
	return -3;
	}

int Editor::nextmenu(void) {
	do {
		menunu++;
		if(menunu>=menuiter)
			menunu=0;
	} while(!menulist[menunu]);
	return -4;
	}


void putmenus(void) {
	if(menuon) {
		touchwin(barwin);
		wrefresh(barwin);
//		prefresh(barwin,0,0,0,0,MainScreenwidth,1);
		}
	}

int resized(void) {
	curses_init_curses.setsize();
	resizeflag=0;
		if(MainScreenwidth>startbar){
	#ifdef HAS_WRESIZE
			wresize(barwin,1,MainScreenwidth);
	#endif
			OldMainScreenwidth=MainScreenwidth;
			}
		redistribute=1;
	return 0;
	}
#endif
