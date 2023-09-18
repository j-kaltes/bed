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
#ifndef GETFILE_H
#define GETFILE_H
#include <features.h>
#include <string.h>
#include "views.h"
#ifdef  __cplusplus
#include "config.h"
#include CURSES_HEADER 
#ifdef PDCURSES
#undef MOUSE_MOVED
#define MOUSE_MOVED WIN_MOUSE_MOVED
#endif
extern WINDOW *takescr;
class winsel {
	public:
	char al[80];
	int in;
	char **strings;
	int tot,beg,h,l,x,y,sel,icase;
	winsel(char **str,int nr,int xpos,int ypos,int begin=0,int hight=10,int len=20) {
		strings=str;
		tot=nr;
		h=hight;
		beg=begin-h/2;if(beg<0) beg=0;
		sel=begin-beg;
		l=len;
		x=xpos;
		in=0;
		y=ypos;
		icase=1;
		};
	~winsel() {}
	 char *current(void) {
		if(!tot)
			return NULL;
		return strings[beg+sel];
		};
	void leave(void) ;
	void show(void) ;
//	char *select(void) ;
	 char *select(int key) ;
	int selectindex(int key) ;
	void showsel(int) ;
	};


class line {
	public:
	char *buf;
	int end,&endptr;
	int size,pos,bij;
	int bufgegsize;
	int x,y;
	int start;
	line(const char *name,int xpos,int ypos,int s=38,int maxgegs=80) ;
	line(const char *name,int &p,int xpos,int ypos,int s=38,int maxgegs=80) ;
	~line(); 
	void show(void) ;
	void add(int key) ;
	void take(void) ;
inline	void cursor(void) {
		wmove(takescr,y,x+pos+1-bij);
		}
	char *change(int key) ;
	char *setvalue(char *name,int len) ;
	};

class item {
static	char nul;
	public:
	enum {maxtext=80};
	char text[maxtext],*after;
	int ch;
	char lowch;
	int x,y;
	public:

	item(void) {;};
	item(const char *txt1,int ya,int xb) ;
	char thekey(void) {return lowch;};
	virtual ~item() {};
	virtual void show(void) =0;
	virtual void select(void)=0; 
	virtual int switchvar(void) =0;
	virtual int save(void) {return 0;} ;
	virtual int saveall(void) {return 0;} ;
	virtual int change(int key) {if(key==' '||key=='x'||key=='*') return switchvar(); return 0;};

	};


class lineitem:public item {
	line l;
	char *var;
	public:
	lineitem(const char *txt,char *name,int ypos,int xpos,int s=35,int maxgeg=80): item(txt,ypos,xpos),l(name,xpos+strlen(txt),ypos,s,maxgeg),var(name) {
		}
	lineitem(const char *txt,int &len,char *name,int ypos,int xpos,int s=35,int maxgeg=80): item(txt,ypos,xpos),l(name,len,xpos+strlen(txt),ypos,s,maxgeg),var(name)  {
		}
	~lineitem() {};
	void show(void) override; 
	void select(void) override;

	int switchvar(void) override {l.change(' ');return 0;};
	int save(void) override {memmove(var,l.buf,l.end);var[l.end]='\0';l.endptr=l.end;return 0;};
	int change(int key) override { 
		const char *const res= l.change(key);
		return res!=NULL;
		};

	int saveall(void) override;
	};
#include "readconfig.h"
extern WINDOW *takescr;
#define optionscreen takescr
#define	SELCOLOR NOWARN (wattrset(takescr,SELATTR))
#define	KEYCOLOR NOWARN(wattrset(takescr,KEYATTR))
#define NOWARN(x) (void) x
//s/wattrset.*$/NOWARN(&)/
#define	TITLECOLOR NOWARN(wattrset(takescr,TITLEATTR))
#define	SELSELCOLOR  NOWARN(wattrset(takescr,SELSELATTR))
#define	BACK NOWARN(wattrset(takescr,BACKATTR))
#define	NORMALCOLOR BACK
#define	UNSELCOLOR    NOWARN(wattrset(takescr,UNSELATTR))
#include <type_traits>

#define SUBNEXT 1
template <typename T=bool, T min=static_cast<T>(0),T max=static_cast<T>(1),bool actret=false>
class check2: public item {
static	constexpr const  char *const options = " x#*%-+";
	public:
	T var;
	T &old;
	check2(void): old(var) {; };
	check2(const char * const txt,int y1,int x1,T &var1): item(txt,y1,x1),var(var1>max?max:var1), old(var1) {
		};
/*
void next(void) {
			if(var==max) var=min;
			else var++;
		};
typename std::enable_if< std::is_same<typename std::decay<T>::type, bool>::value,void>::type next(void) { 
			var=!var;
		}
typename std::enable_if<(min==0&&max==1),void>::type next(void) { 
			var=var?static_cast<T>(0):static_cast<T>(1);
		}

typename std::enable_if<((!std::is_same<typename std::decay<T>::type, bool>::value)&&(min!=0||max!=1)),T>::type   next(T x) {
typename std::enable_if<(!std::is_same<typename std::decay<T>::type, bool>::value),T>::type   next(T x) {
		if(var==max) return min;
		else
			return var+1;
		};

typename std::enable_if<(std::is_same<typename std::decay<T>::type, bool>::value),T>::type   next(T x) {return !x;};
typename std::enable_if<(!std::is_same<typename std::decay<T>::type, bool>::value&&min==0&&max==1),T>::type next(T x)  {
			return var?static_cast<T>(0):static_cast<T>(1);
			};
	void next(void) {
		var=next(var);
		};
*/
#if SUBNEXT
	void next(void) {
		if(var==max) var=min;
		else var++;
		};
#else
	void next(void) {
#if __cplusplus >= 201700L
		if constexpr ( std::is_same<typename std::decay<T>::type, bool>::value)
			var=!var;
	else {
		if constexpr (min==0&&max==1)
			var=var?static_cast<T>(0):static_cast<T>(1);

		else {
			if(var==max) var=min;
			else var++;
			};
	};
#else
		switch(var) {
			case max: var=min;break;
			case static_cast<T>(0): var= static_cast<T>(1);break;
			default: (*(reinterpret_cast<uint8_t *>(&var)))++;

		};
#endif
		};
#endif
	 int save(void)  override{
		old=var;
		return 0;
		};
	int saveall(void)  override{
		save();
		return 2; 
		};

	void show(void) override {

//		NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		SELCOLOR;
		wmove(optionscreen,y,x);
		waddch(optionscreen,'[');
		waddch(optionscreen,options[var]);
		waddch(optionscreen,']');
		waddch(optionscreen,' ');
		waddstr(optionscreen,text);
		KEYCOLOR;
//		NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELCOLOR;
//		NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		waddstr(optionscreen,after);
		};
	void select(void) override {
		SELSELCOLOR;
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		wmove(optionscreen,y,x);
		waddch(optionscreen,'[');
		waddch(optionscreen,options[var]);
		waddch(optionscreen,']');
		waddch(optionscreen,' ');
		waddstr(optionscreen,text);
		KEYCOLOR;
//		NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELSELCOLOR;
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x+1);
		};

 int switchvar(void) override {
	 next();
//		var=!var;
	SELSELCOLOR;
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
	wmove(optionscreen,y,x);
	waddch(optionscreen,'[');
	waddch(optionscreen,options[var]);
	waddch(optionscreen,']');
	wmove(optionscreen,y,x+1);
constexpr int ret  =		(actret?  0x7FFFFFFF:  0);
	return ret;
	};
	};

#if SUBNEXT
template <> inline  void check2<bool,false,true,true>::next(void) { var=!var;};
template <> inline  void check2<bool,false,true,false>::next(void) { var=!var;};
//template <typename T> inline  void check2<T,0,1,false>::next(void) { var=var?static_cast<T>(0):static_cast<T>(1);};
//template <typename T> inline  void check2<T,0,1,true>::next(void) { var=var?static_cast<T>(0):static_cast<T>(1);};
//template <> inline  void check2<uint8_t,0,1,true>::next(void) { var=var?0:1;};
#endif

typedef check2<bool,0,1,false> check;

template <typename T> class orcheck2;

template <typename T=int> class Common2 {
	public:
	int nr;
	T &old;
	orcheck2<T> *last;
	Common2(T &v,int n=1000):old(v) {
		nr=n;
		};
	orcheck2<T> *setlast(orcheck2<T> *l) {
		last=l;
		return l;
		}
inline	int save(void) ;
	};

template <typename T=int> class orcheck2: public item {
	public:
	T val;
	Common2<T> *com;
	orcheck2(const char *txt,int y1,int x1,T v,Common2<T> *c): item(txt,y1,x1), val(v),com(c) {
	c->nr++;
		};
	~orcheck2() {
		if(com->nr==1)
			delete com;
		else
			com->nr--;
		}
	void show(void) override ;
	void select(void) override ;
	 int switchvar(void) override ;
	 int save(void) override {
	 	return com->save();
		};
	void showvar(void) ;
	void selectvar(void) ;
	 void sel(void) ;
	int saveall(void) override {
			return 2; 
			};
	};
template <typename T>
inline	 void orcheck2<T>::sel(void) {
		wmove(optionscreen,y,x);
		waddch(optionscreen,'(');
		waddch(optionscreen,(com->last==this?'*':' '));
		waddch(optionscreen,')');
		}
template <typename T>
inline	 void orcheck2<T>::show(void) {
		SELCOLOR;
		sel();
		waddch(optionscreen,' ');
		waddstr(optionscreen,text);
		KEYCOLOR;
//		NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELCOLOR;
//		NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		waddstr(optionscreen,after);
		};
template <typename T>
inline	 void orcheck2<T>::select(void) {
//		SELCOLOR;
		sel();
		waddch(optionscreen,' ');
		SELSELCOLOR;
		waddstr(optionscreen,text);
		KEYCOLOR;
//		NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELSELCOLOR;
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x+1);
		};
 template <typename T>
	 void orcheck2<T>::showvar(void) {
//		NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		SELCOLOR;
		sel();
		wmove(optionscreen,y,x+1);
		}
template <typename T>
	 void orcheck2<T>::selectvar(void) {
		SELSELCOLOR;
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		sel();
		wmove(optionscreen,y,x+1);
		}

template <typename T>
	 int orcheck2<T>::switchvar(void) {
	 	orcheck2<T> *was=com->last;
	 	if(was!=this) {
			com->last=this;
			was->showvar();
			selectvar();
			return 1;
			}
		return 0;
		};

template <typename T>
inline	int Common2<T>::save(void) {
		old=last->val;
		return 0;
		}
using Common = Common2<int>;
using orcheck =orcheck2<int>;
class action: public item {
	protected:
	int &var;
	public:
//	action(void) {;};
	action(const char *txt,int y1,int x1,int &v): item(txt, y1, x1),var(v) {;};
	void show(void) override ;
	void select(void) override ;
	 virtual int switchvar(void) override {
		return 0;
		};
	virtual int saveall(void) override {
		var=1;
		return 1;
		};
	};

class saveaction: public action {
	public:
	saveaction(const char *txt,int y,int x,int &var): action(txt,y,x,var) {
		}
	int saveall(void) override {
		action::saveall();
		return 2;
		}
	};
class ok: public action {
	item **alles;
//	int nr;
	public:
	ok(item **al,int len,int y,int x,int &var): action("  ~Ok  ",y,x,var) {
		alles=al;
//		nr=len;
		}
	int saveall(void) override ;
	};
class itemlist:public item {
	int &ant;
	winsel wins;
	public:
	itemlist(const char *txt,char **str,int nr,int ypos,int xpos,int &a,int begin=0,int hight=10,int len=20);
	~itemlist() ;
	void show(void) override ;
	void select(void) override ;
	int switchvar(void) override ;
	int save(void) override ;
	int saveall(void) override ;
	int change(int key) override ;
	};

void label(int y,int x,const char *str) ;

int show(item **al,int showformsmaxar,int sel) ;
extern int zomaareenvar;
//s/showformsarray\[showformsiter++\]=/ITERTHROUGH /g
#define ITERTHROUGH showformsarray[showformsiter++]=
#define listall(txt,y,x,str,nr,var,beg,hi,len) (ITERTHROUGH new itemlist(txt,str,nr,y,x,var,beg,hi,len))
#define list(txt,y,x,str,nr,var) listall(txt,y,x,str,nr,var,0,10,20)
#define el(check,str,y,x,var) (ITERTHROUGH  new check(str,y,x,var))
//#define checker(str,y,x,var) (ITERTHROUGH  new check(str,y,x,var))
#define checker(str,y,x,var) (ITERTHROUGH  new check2<decltype(var),0,1,false>(str,y,x,var))
#define checkernr(str,y,x,var,nr) (ITERTHROUGH  new check2<decltype(var),0,nr,false>(str,y,x,var))
//#define checkertrue(str,y,x,var) (ITERTHROUGH  new check(str,y,x,var,true))
#define checkertrue(str,y,x,var) (ITERTHROUGH  new check2<decltype(var),static_cast<decltype(var)>(0),static_cast<decltype(var)>(1),true>(str,y,x,var))
#define saveact(str,y,x) (ITERTHROUGH  new saveaction(str,y,x,zomaareenvar))
#define saveacti(str,y,x,var) (ITERTHROUGH  new saveaction(str,y,x,var))
#define act(str,y,x) (ITERTHROUGH  new action(str,y,x,zomaareenvar))
#define acti(str,y,x,var) (ITERTHROUGH  new action(str,y,x,var))
//#define oker(showformsarray,showformsmaxar,y,x) (ITERTHROUGH  new ok(showformsarray,showformsmaxar,y,x))
//#define oke(y,x) (ITERTHROUGH  new ok(showformsarray,showformsmaxar,y,x,zomaareenvar))
#define oke(y,x) (ITERTHROUGH  new ok(showformsarray,0,y,x,zomaareenvar))
//#define okvar(y,x,var) (ITERTHROUGH  new ok(showformsarray,showformsmaxar,y,x,var))
#define liner(txt,name,x,y) (ITERTHROUGH  new lineitem(txt,name,x,y))
#define linerlen(txt,len,name,x,y) (ITERTHROUGH  new lineitem(txt,len,name,x,y))
#define linernr(txt,name,x,y,nr) (ITERTHROUGH  new lineitem(txt,name,x,y,nr))
#define linernrmaxaction(txt,name,x,y,nr,max) (ITERTHROUGH  new lineitem(txt,name,x,y,nr,max,true))
#define linernrmax(txt,name,x,y,nr,max) (ITERTHROUGH  new lineitem(txt,name,x,y,nr,max))
#define linerlennr(txt,len,name,x,y,nr) (ITERTHROUGH  new lineitem(txt,len,name,x,y,nr))
#define linerlennrmax(txt,len,name,x,y,nr,max) (ITERTHROUGH  new lineitem(txt,len,name,x,y,nr,max))
#define orel(txt,y,x,val)  (ITERTHROUGH new orcheck(txt,y,x,val,com))
#define oreen(txt,y,x,val)  (ITERTHROUGH new orcheck(txt,y,x,val,com=new Common(var)))

#define ore(txt,y,x,val,com)  (ITERTHROUGH new orcheck(txt,y,x,val,com))
#define orsel(txt,y,x,val,com)  (ITERTHROUGH com->setlast(new orcheck(txt,y,x,val,com)))
/*
#define ors(var,items...,item) {Common *com=new Common(var);int was=showformsiter;0,  ##items,com->last=showformsarray[was+val]}
#define ors(var,items...) {Common *com=new Common(var);int was=showformsiter;  ##items,com->last=showformsarray[was+val]}
*/

typedef item *itemshowptr;

extern WINDOW *getshowwin(int l,int c,int by,int bx) ;

#define initdialog(nr) {item **showformsarray;int showformsiter=0,showformsmaxar=(nr);  showformsarray=new itemshowptr[showformsmaxar];
#define dodialog(ant,l,c,sel) if((takescr=getshowwin((l),(c),(thelines-(l))/2,(thecols-(c))/2))){ (ant)=show(showformsarray,showformsiter,(sel));};for(int dodialogiter=0;dodialogiter<showformsiter;dodialogiter++) delete showformsarray[dodialogiter];delete[] showformsarray;};
#define dodialogpost(ant,l,c,sel,post) if((takescr=getshowwin((l),(c),(thelines-(l))/2,(thecols-(c))/2))){(post);(ant)=show(showformsarray,showformsiter,(sel));};for(int dodialogiter=0;dodialogiter<showformsiter;dodialogiter++) delete showformsarray[dodialogiter];delete[] showformsarray;};
#define showdialog(ant,l,c,sel) if((takescr=getshowwin((l),c,(thelines-(l))/2,(thecols-c)/2))){ant=show(showformsarray,showformsiter,sel);};
#define deldialog for(int dodialogiter=0;dodialogiter<showformsiter;dodialogiter++) delete showformsarray[dodialogiter];delete[] showformsarray;};

#define opt(name,y,x,val) ore(name,y,x,val,comptr) 

#endif


#define begindialog(l,c,nr) if((takescr=getshowwin((l),(c),(thelines-(l))/2,(thecols-(c))/2))){item **showformsarray;int showformsiter=0,showformsmaxar=nr;  showformsarray=new itemshowptr[showformsmaxar]; 

#define enddialog(ant,sel) ant=show(showformsarray,showformsiter,(sel));for(int dodialogiter=0;dodialogiter<showformsiter;dodialogiter++) delete showformsarray[dodialogiter];delete[] showformsarray;};

#define beginoptions(name)  {int startformsiter=showformsiter; Common *comptr=new Common(name,0);
#define endoptions(sel)	comptr->last=static_cast<orcheck*>(showformsarray[startformsiter+(sel)]);}; 

#include "varmacro.h"


#ifdef OLDGCCVARMACRO
#define lcconfig(l,c,items...) lcsconfig(l,c,1,items) VARMACRO
#define lcsconfig(l,c,sel,items...) plcsconfig(l,c,sel,(void)0,items) VARMACRO
#define plcsconfig(l,c,sel,post,items...) {int throwawayant;palcsconfig(throwawayant,l,c,sel,post,items) } VARMACRO
#define alcsconfig(ant,l,c,sel,items...) palcsconfig(ant,l,c,sel,(void)0,items)  VARMACRO
#define palcsconfig(ant,l,c,sel,post,items...) {item **showformsarray;int showformsiter=0; int showformsmaxar=sizeof((item* []){items})/sizeof(item *); if((takescr=getshowwin(l,c,(thelines-l)/2,(thecols-c)/2))) { showformsiter=0; showformsarray=new (item *)[showformsmaxar],(items);post;ant=show(showformsarray,showformsmaxar,sel);for(showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];delete[] showformsarray;};} VARMACRO
#define optselect(name,sel,items...)  {int startformsiter=showformsiter; Common *comptr=new Common(name,0);items,comptr->last=(orcheck*)showformsarray[startformsiter+sel];}; VARMACRO
#else
#define lcconfig(l,c,...) lcsconfig(l,c,1,__VA_ARGS__) 
#define lcsconfig(l,c,sel,...) plcsconfig(l,c,sel,(void)0,__VA_ARGS__) 
#define plcsconfig(l,c,sel,post,...) {int throwawayant;palcsconfig(throwawayant,l,c,sel,post,__VA_ARGS__) } 
#define alcsconfig(ant,l,c,sel,...) palcsconfig(ant,l,c,sel,(void)0,__VA_ARGS__)  
//#ifdef __FreeBSD__
#if 1
//#define palcsconfig(ant,l,c,sel,post,...) {item **showformsarray; if((takescr=getshowwin(l,c,(thelines-l)/2,(thecols-c)/2))) {int showformsiter=0; showformsarray=new itemshowptr [20],__VA_ARGS__;post; const int showformsmaxar=showformsiter; ant=show(showformsarray,showformsmaxar,sel);for(int showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];delete[] showformsarray;};} 
//#define palcsconfig(ant,l,c,sel,post,...) {if((takescr=getshowwin(l,c,(thelines-l)/2,(thecols-c)/2))) {int showformsiter=0; item *showformsarray[]={__VA_ARGS__};post; const int showformsmaxar=showformsiter; ant=show(showformsarray,showformsmaxar,sel);for(int showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];};} 
#define palcsconfig(ant,l,c,sel,post,...) {if((takescr=getshowwin(l,c,(thelines-l)/2,(thecols-c)/2))) {int showformsiter=0;item *showformsarray[]={__VA_ARGS__};post; const int showformsmaxar=sizeof(showformsarray)/sizeof(showformsarray[0]); ant=show(showformsarray,showformsmaxar,sel);for(int showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];};} 
#else
#define palcsconfig(ant,l,c,sel,post,...) {item **showformsarray;int showformsiter=0; \
	int showformsmaxar=sizeof((item* []){__VA_ARGS__})/sizeof(item *); if((takescr=getshowwin(l,c,(thelines-l)/2,(thecols-c)/2))) { showformsiter=0; showformsarray=new itemshowptr [showformsmaxar],__VA_ARGS__;post;ant=show(showformsarray,showformsmaxar,sel);for(showformsiter=0;showformsiter<showformsmaxar;showformsiter++) delete showformsarray[showformsiter];delete[] showformsarray;};} 
#endif
//#define optselect(name,sel,...)  {int startformsiter=showformsiter; Common *comptr=new Common(name,0);__VA_ARGS__,comptr->last=(orcheck*)showformsarray[startformsiter+sel];}; 

#define optselect(name,sel,...) beginoptions(name);__VA_ARGS__,endoptions(static_cast<int>(sel))



#define ore2(txt,y,x,val,com)  (ITERTHROUGH new orcheck2<decltype(val)>(txt,y,x,val,com))
#define opt2(name,y,x,val) ore2(name,y,x,val,comptr) 
#define beginoptions2(name,type)  {int startformsiter=showformsiter; Common2<type> *comptr=new Common2<type>(name,0);
#define endoptions2(sel,start,type)	comptr->last=static_cast<orcheck2<type>*>(showformsarray[startformsiter+(sel)-start]);}; 
#define optselect2t(type,name,sel,start,...) beginoptions2(name,type);__VA_ARGS__,endoptions2(sel,start,type)
#define optselect2(name,...) optselect2t(decltype(name),name, __VA_ARGS__)
#endif
#endif

