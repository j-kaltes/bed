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
#endif
              #include <sys/types.h>
#include "dialog.h"
#include "screen.h"
#include "system.h"
#include "readconfig.h"
#include "onlykey.h"
#include "keyinput.h"
#include "myalloc.h"
#define trefresh(x)
extern char spatiebuf[];
char spatiebuf[240];

int zomaareenvar;

#define minnum( x,y) ((x)>(y)?(y):(x))
#define maxnum( een, twee) (((een)>(twee))?(een):(twee))
#ifdef CURSES

#if defined(TEST) || defined(TEST2) || defined(TEST3)
#define EENTEST
#endif

#ifdef EENTEST



Fieldcol fieldcolor[] = {
{"text",0,1},{"command",A_REVERSE,2},{"bar",A_REVERSE,3},
{"barkey",A_NORMAL,4},{"menu",A_REVERSE,5},{"menukey",A_NORMAL,6},
{"menusel",A_BOLD|A_UNDERLINE,7},
{"opentitle",A_BOLD,8}, {"openkey",A_BOLD,9},
{"opennormal",0,10},
{"openlist",A_REVERSE,11},{"openlistsel",A_NORMAL,12},
{"openlistact",A_NORMAL|A_BOLD,13},{"showback",A_BOLD,14},{"selection",A_REVERSE,15}, {"cursor",A_REVERSE,16}, {"selcursor",A_UNDERLINE|A_BOLD,17} };


#define REFRESH 12
int ggetch(WINDOW *w) {
	static int ch=0;
	int key;
				cbreak();
	if(ch) {
		if(ch==27) {
			ch=0;

#ifndef EENTEST
			if(keystrokeout<keystrokein)
				key=keystrokes[keystrokein++];
			else 
#endif
{
				halfdelay(1);
				if((key=wgetch(w))== (-1) )
					key= secondescape ;
				nocbreak();
				cbreak();
			}
				}
		else {
			key=ch;
			ch=0;
			}
		}
	else {
#ifndef EENTEST
			if(keystrokeout<keystrokein)
				key=keystrokes[keystrokein++];
			else
#endif
				key=wgetch(w);
			if((key)&0x80) {
				ch=key&0x7f;
				return 27;
				}
			else
				if(key==27)
					ch=27;
		}
	return key;
	}




//#define ggetch(x) wgetch(x)
#define thelines LINES
#define thecols COLS
int initscreen(void) {
	initscr();
	clearok(stdscr,FALSE);
	clearok(curscr,FALSE);

		noecho();
	   	cbreak();
	   	nonl();

	if(has_colors()) {
			short f,b;
			start_color();
			getfilecolors();
			}
		scrollok(stdscr,0);
	   	};
//#define SELSELATTR         SELATTR|A_REVERSE|A_BOLD
void getfilecolors(void) {
			init_pair(4,COLOR_RED,COLOR_BLUE);
			SELATTR=COLOR_PAIR(4)|A_BOLD;
			init_pair(5,COLOR_BLACK,COLOR_YELLOW);
			KEYATTR=COLOR_PAIR(5)|A_STANDOUT|A_BOLD;
			init_pair(6,COLOR_BLACK,COLOR_GREEN);
			TITLEATTR=COLOR_PAIR(6);
			init_pair(7,COLOR_WHITE,COLOR_BLACK);
			BACKATTR=COLOR_PAIR(7);
		}
#else
extern int thelines,thecols;
extern int ggetch(WINDOW *w);
#endif
/*
inline void cursoraway(void) {
	HIDECURSOR;
		move(thelines-1,thecols);
		refresh();
		}
*/
#define cursoraway() HIDECURSOR

	#define winb 48
	#define winh 15
WINDOW *takescr;
//#define optionscreen takescr
#endif
#include "keylabels.h"

#define keylabel(label,edit,menu,open) open,
int getfilesym[]= {
#include "keylist.h"
};
#undef keylabel

#include "glob.h"

void	attrall(WINDOW *takescr,int attrs) {
	int y=0;
	int x=0;
	getmaxyx(takescr,y,x);
//	char line[x+1];
	char *line=(char *)alloca(x+1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
	memset(line,' ',x);
#pragma GCC diagnostic pop
	line[x]='\0';
	NOWARN(wattrset(takescr, attrs );)
	wmove(takescr,0,0);
	for(int i=0;i<y;i++) 
		waddnstr(takescr,line,x);
	}

void winsel::show(void) {
	int last=minnum(h,tot-beg);
	SELCOLOR;
	trefresh(stdscr);
	for(int i=0;i<h;i++) {
		 mvwaddnstr(takescr,i+y,x,spatiebuf,l);
		}
	for(int i=0;i<last;i++) {
		 mvwaddnstr(takescr,i+y,x,strings[beg+i],(l));
		}
	mvwvline(takescr,y,x-1,0, h); /*Without these lines on ncurses error sometimes meshed up the screen. Showmarks after list search at the end of the file*/
	mvwvline(takescr,y,x+l,0, h);
	if(tot<1)
		return;
	if(sel>=last)
		sel=last-1;
	if(sel<0)
		sel=0;
	SELSELCOLOR;
	trefresh(takescr);
	mvwaddnstr(takescr,y+sel,x,spatiebuf,l);
	mvwaddnstr(takescr,y+sel,x,strings[beg+sel],l);
	trefresh(takescr);
	}
void winsel::showsel(int dir) {
	if(tot<1)
		return;
	int newsel=sel+dir;
	int last=minnum(h,tot-beg);
	if(newsel>=last)  {
		if((newsel+beg)>=tot)  {
			beg=tot-last;
			sel=tot-1-beg;
			}
		else  {
			beg+=dir;
			}
		 show();
		 return;
		}
	if(newsel<0) {
		if(beg) {
			sel=beg+newsel;
			beg=maxnum(0,beg+dir);
			sel=maxnum(0,sel-beg);
			}
		else
			sel=0;
		show();
		return;
		}
	if((newsel+beg)>=(tot-1))  
		newsel=tot-beg-1;

		SELCOLOR;
		mvwaddnstr(takescr,y+sel,x ,spatiebuf,l);
	//	wmove(takescr,y+sel,x);
	//	waddnstr(takescr,spatiebuf,l);
		mvwaddnstr(takescr,y+sel,x,strings[beg+sel],l);
		sel=newsel;
		SELSELCOLOR;
//		s/\<refresh(/wrefresh(takescr
		trefresh(takescr);
		mvwaddnstr(takescr,y+sel,x,spatiebuf,l);
		mvwaddnstr(takescr,y+sel,x ,strings[beg+sel],l);
		trefresh(takescr);
	}
	void winsel::leave(void) {
		if(tot<1)
			return;
		UNSELCOLOR;
		trefresh(takescr);
		mvwaddnstr(takescr,y+sel,x,spatiebuf,l);
		mvwaddnstr(takescr,y+sel,x,strings[beg+sel],l);
		trefresh(takescr);
		}	
#include "getlabel.h"
char *winsel::select(int key) {
	int ant;	
	if((ant=selectindex( key) )<0)
		return NULL;
	return strings[ant];
	}
int winsel::selectindex(int key) {
		switch(key) {
			case RETURN: in=0; 
			if(tot>0)
					return beg+sel;
			break;
			case DOWN: showsel(1);;break;
			case UP: showsel(-1);;break;
			case END: showsel(tot);break;
			case BEG: showsel(-tot);break;
			case PGDN: showsel(h-1);break;
			case PGUP: showsel(1-h);break;
			default:
			if((key<0xff)&&isprint(key)) {
				int start,pos;
				if(!in)
					start=0;
				else
					start=sel+beg;
				al[in++]=key;
				al[in]='\0';
				pos=getunderlabeli(strings+start,tot-start,al,icase);
				showsel(pos+start-(sel+beg));
				return -1;
				}
			};
		in=0;
		return -1;
		}

		/*
char *winsel::select(void) {
	int key;
	while(1) {
		while((key=input.keyseq(ggetch(takescr)))==-1)
			trefresh(takescr);
		switch(key) {
			case RETURN: return strings[beg+sel];
			case NEXT: showsel(1);;break;
			case PREV: showsel(-1);;break;
			case END: showsel(tot);break;
			case BEG: showsel(-tot);break;
			case PGDN: showsel(h-1);break;
			case PGUP: showsel(1-h);break;
			};
		wmove(takescr,-1,-1);
		trefresh(takescr);
		}
	}
	*/

	itemlist::itemlist(const char *txt,char **str,int nr,int ypos,int xpos,int &a,int begin,int hight,int len):
	item(txt,ypos,xpos),
	ant(a),
	wins(str,nr,xpos+1,ypos+1,begin, hight,len)
 {
		};
	itemlist::~itemlist() {
		}
	void itemlist::show(void) {
		SELCOLOR;
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
		waddch(optionscreen,ch);
		SELCOLOR;
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		wins.show();
		}
	void itemlist::select(void) {
		SELSELCOLOR;
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
		waddch(optionscreen,ch);
		SELSELCOLOR;
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		wins.showsel(0);
		}
	int itemlist::switchvar(void) {
		return wins.selectindex(' ');
		}
	int itemlist::save(void) {
		ant= wins.beg+wins.sel;
		return 0;
		}
	int itemlist::saveall(void) {
			save();
			return 2;
			;}
	int itemlist::change(int key) {
		return wins.selectindex(key);
		};
#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define	keydef(sym,keys...) inputkeys.putseq(sym,sizeof((unsigned char [])##keys),(unsigned char []) ##keys)  VARMACRO
#define	seqput(sym,keys...) keyinp->putseq(sym,sizeof((unsigned char [])##keys),(unsigned char []) ##keys) VARMACRO
#else
#define	keydef(sym,...) {unsigned char tmpar[]=__VA_ARGS__;inputkeys.putseq(sym,sizeof(tmpar),tmpar);  }
#define	seqput(sym,...) {unsigned char tmpar[]=__VA_ARGS__;keyinp->putseq(sym,sizeof(tmpar),tmpar); }
#endif
void putkeys(class keyinput *keyinp) {
		seqput(RETURN,{13});
		seqput(RETURN,{10});
#ifdef NCURSES_VERSION
		seqput(END,{27,91,52,126});
		seqput(BEG,{27,91,49,126});
		seqput(NEXT,{27,91,67});
		seqput(PREV,{27,91,68});
		seqput(SHIFTAB,{27,91,90});
		seqput(DELETEKEY,{27,91,51,126});
#endif

		seqput(QUIT,{27,secondescape});
		seqput(QUIT,{27,120});
		seqput(REFRESH,{REFRESH});
		seqput(BACKSPACE,{BACKSPACE});

		seqput(CLEAR,  {21});
		seqput(TAB,{9});

#ifdef STATICKEYS
		seqput(FILEPART,{27,102});
		seqput(DIR,{27,100});
		seqput(FIL,{27,110});
#endif
		memset(spatiebuf,' ',240);
		spatiebuf[239]='\0';
		}
#ifdef STATICKEYS
keyinput inputkeys(putkeys);
#endif

/*
	char *buf;
	int end,&endptr;
	int size,pos,bij;
	int bufgegsize;
	int x,y;
	int start;
*/
	line::line(const char *name,int xpos,int ypos,int s,int maxgegs):end(strlen(name)),endptr(end),size(s),bij(0),bufgegsize(maxgegs-1),x(xpos),y(ypos) {
		buf=new char[maxgegs];
		pos=end=(end>bufgegsize)?bufgegsize:end;	
		memmove(buf,name,end);
		buf[end]='\0';
		};

	line::line(const char *name,int &p, int xpos,int ypos,int s,int maxgegs):endptr(p),size(s),bij(0),bufgegsize(maxgegs-1),x(xpos),y(ypos) {
		buf=new char[maxgegs];
		if(p>bufgegsize)
			p=bufgegsize;
		pos=end=p;
		memmove(buf,name,end);
		buf[end]='\0';
		};
	char *line::setvalue(char *name,int len) {
		end=len;
		if(end>bufgegsize)
			end=bufgegsize;
		memmove(buf,name,end);
		buf[end]='\0';
		bij=0;
		pos=end;
		return buf;
		}
	line::~line() {
		delete[] buf;
		};
	void line::show(void) {
		SELCOLOR ;
		wmove(takescr,y,x);
		for(int i=0;i<(size+2);i++)
			waddch(takescr,' ');
		wmove(takescr,y,x+1);
		for(char *i=buf+bij;i<(buf+bij+minnum(end-bij,size));i++)
			waddch(takescr,*i);
		
	//	waddnstr(takescr,buf+bij,minnum(end-bij,size));
		wmove(takescr,y,x+pos-bij+1);
		}
	void line::add(int key) {
		SELCOLOR ;
		cursor();
		pos++;
		waddch(takescr,key);
		}
	void line::take(void) {
		start=1;
		pos=0;
		show();
		KEYCOLOR;
		wmove(takescr,y,x+1);

		for(char *i=buf+bij;i<(buf+bij+minnum(end-bij,size));i++)
			waddch(takescr,*i);
//		waddnstr(takescr,buf+bij,minnum(end-bij,size));
		cursor();
		}
	char *line::change(int key) {
		switch(key) {
			case RETURN: buf[end]='\0';endptr=end;return buf;
			case NEXT: 
				if(start)
					show();
				if(pos<end) {
					pos++;
					if(pos>=(bij+size)) {
						bij++;
						show();
						}
					else
						cursor();
					}
				else
					beep();
				break;
			case PREV: 
				if(pos>0) {
					pos--; 
					if(pos<bij) {
						bij--;
						show();
						}
					else {
						if(start)
							show();
						else
							cursor();
						}

					}
				else {
					beep();
					if(start)
						show();
					}
				break;
			case BEG:  bij=0;pos=0; show();break;
			case END:  
				if((end-bij)>=size)
				   {
					bij=end-size+1;
					}
				pos=end; 
				show();
				break;
			case BACKSPACE: 
					if(pos>0) {
						end--;
						pos--;

						memmove(buf+pos,buf+pos+1,end-pos);
						if(pos<bij) {
							bij--;
							}
						show();
						}
					else {
						if(start)
							show();
						};
					break;
			case DELETEKEY: 
				if(!start) {
					if(pos<end) {
							end--;
							memmove(buf+pos,buf+pos+1,end-pos);
							cursor();
							show();
							}
					break;
					}
			case CLEAR:
				memmove(buf,buf+pos,end-pos);
				end-=pos;
				pos=0;bij=0;
				show();
			default:
			if(start) {
				bij=pos=end=0;
				start=0;
				show();
				}
			if((key<0xff)&&isprint(key)) {
				if(end<bufgegsize) {
					if(pos<end) {
						memmove(buf+pos+1,buf+pos,end-pos);
						end++;
						buf[pos++]=key;
						if(pos>=(bij+size))
							bij++;
						show();
						}
					else {
						buf[pos]=key;
						pos++;
						if(pos>end)
							end=pos;
						if(pos>=(bij+size)) {
							bij++;
							show();
							}
						else {
							pos--;
							add(key);
							}
						}
					}
				return NULL;
				}
		}
		start=0;
		return NULL;
	}


#define perms(x,el) ((x&(1<<el))?(permsym[el]):'-')

                     #include <time.h>
//char timestr[]="%b %d %Y %X";
#ifdef _WIN32
#include "win.h"
#include "inout.h"
/*
typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;
*/

struct filesym { DWORD mask; char show;}; 
const struct filesym fsym[]=
{{FILE_ATTRIBUTE_DIRECTORY,'d'},
{FILE_ATTRIBUTE_ARCHIVE,'a'},
{FILE_ATTRIBUTE_READONLY,'r'},
{FILE_ATTRIBUTE_HIDDEN,'h'},
{FILE_ATTRIBUTE_SYSTEM,'s'}};
//const int nrfsym=sizeof(fsym)/sizeof(struct filesym);
const int nrfsym=sizeof(fsym)/sizeof(fsym[0]);
//#define DATE_LOCALE MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL)
//#define DATE_LOCALE MAKELANGID(LANG_DUTCH,SUBLANG_DUTCH)
//#define DATE_LOCALE MAKELANGID(LANG_RUSSIAN,SUBLANG_RUSSIAN_RUSSIA)
//#define DATE_LOCALE MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_HONGKONG)
#define DATE_LOCALE LOCALE_USER_DEFAULT
//#define DATE_LOCALE MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL)


//#if CYGWIN_VERSION_DLL_MAJOR<1007
//#define RUNONWIN95 1
//#endif
int filegeg(char *name,char *buf) {
int j=0;

//if((Attr=GetFileAttributesEx(name, GetFileExInfoStandard,&attr)) {
#ifndef RUNONWIN95
WIN32_FILE_ATTRIBUTE_DATA attr;
if(!GetFileAttributesEx(name, GetFileExInfoStandard,&attr)) {
		buf[0]='\0';
		return 0;
		}

const DWORD fileattr=attr.dwFileAttributes;
const   FILETIME lastwrite=attr.ftLastWriteTime;
const DWORD FileSizeHigh=attr.nFileSizeHigh;
const DWORD FileSizeLow=attr.nFileSizeLow;
#else
            WIN32_FIND_DATA wfd;
     HANDLE       hfind = FindFirstFile(name, &wfd);
            if (hfind == INVALID_HANDLE_VALUE) {
		buf[0]='\0';
		return 0;
		}

const DWORD fileattr=wfd.dwFileAttributes;
const   FILETIME lastwrite=wfd.ftLastWriteTime;
const DWORD FileSizeHigh=wfd.nFileSizeHigh;
const DWORD FileSizeLow=wfd.nFileSizeLow;

#endif
for(int i=0;i<nrfsym;i++) 
	if((fsym[i].mask&fileattr))
			buf[j++]=fsym[i].show;
		else
			buf[j++]='-';
const char *datestr="MMM dd yyyy";
const char *timestr="HH':'mm";
FILETIME localfiletime;
SYSTEMTIME stLocal;
if(FileTimeToLocalFileTime( &lastwrite, &localfiletime)) {
/*
SYSTEMTIME stUTC, stLocal;
 if(FileTimeToSystemTime(&lastwrite, &stUTC)) {
extern OSVERSIONINFO WindowsVersion;
#ifdef RUNONWIN95
if(WindowsVersion.dwOSVersionInfoSize&&WindowsVersion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
FileTimeToLocalFileTime
#endif

    if(SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal)) {
*/
 if(FileTimeToSystemTime(&localfiletime, &stLocal)) {
	buf[j++]=' ';
	int res;
	if(!(res=GetDateFormat(DATE_LOCALE ,0, &stLocal, datestr, buf+j, PATH_MAX-j)))
	 	winerrormessage("GetDateFormat: %s"); 
	j+=res;
	buf[j-1]=' ';
	if(!(res=GetTimeFormat( DATE_LOCALE,TIME_NOSECONDS| TIME_FORCE24HOURFORMAT, &stLocal, timestr, buf+j, PATH_MAX-j)))
	 	winerrormessage("GetTimeFormat: %s"); 
	res--;
	j+=res;
	}
	else {	
	 	winerrormessage("FileTimeToSystemTime failed: %s"); 
		}
	}
	else
	 	winerrormessage("FileTimeToLocalFileTime failed: %s"); 
/*

	else {	
	 	winerrormessage("SystemTimeToTzSpecificLocalTime failed: %s"); 
		}
	}
	else
	 	winerrormessage("FileTimeToSystemTime failed: %s"); 
*/
j+=sprintf(buf+j," %lld", ((long long)FileSizeHigh)<<32|   FileSizeLow);
return j;
}
#else

constexpr const char *sizeformat(const long) { return " %ld";} ;
constexpr const char *sizeformat(const long long) { return " %lld";} ;
constexpr const char *unsignedformat(const unsigned int) {return "%u ";};
constexpr const char *unsignedformat(const unsigned long) {return "%lu ";};
constexpr const char *unsignedformat(const unsigned long long) {return "%llu ";};
int filegeg(char *name,char *buf) {
	struct tm *tp;
	int j=0;
	struct stat st;
	char timestr[]=" %b %d %Y %H:%M",	*ptr;
	if(stat(name,&st)<0) {
		buf[0]='\0';
		return 0;
		}
#ifndef WIN32
	snprintf(buf,BEDSMAXPATH, unsignedformat(st.st_nlink),st.st_nlink);
//	snprintf(buf,BEDSMAXPATH, SIZE_TPRINT " ",st.st_nlink);
	j=strlen(buf);
#endif
	if(S_ISDIR(st.st_mode))
		buf[j++]='d';
	else
		buf[j++]='-';
	char permsym[]="xwrxwrxwr";
#ifndef _WIN32
	int endperm=0;
#else
	int endperm=6;
#endif
	for(int i=8;i>=endperm;i--)
		buf[j++]=perms(st.st_mode,i);

#ifndef _WIN32
	const char *user;
	struct passwd *pas;
	if((pas=getpwuid(st.st_uid)))
		user=pas->pw_name;
	else
		user="????";
#else
	buf[j++]=' ';
#endif

	tp=localtime(&st.st_mtime);

	ptr=buf+j;
	strftime(ptr,81,timestr,tp);
	ptr=strlen(ptr)+ptr;
#ifndef _WIN32
	*ptr++=' ';

#ifdef HAVE_STPCPY
	ptr=stpcpy(ptr,user);
#else
	int ulen=strlen(user);
	memcpy(ptr,user,ulen+1);
	ptr+=ulen;
#endif
#endif

	if(!S_ISDIR(st.st_mode))
		sprintf(ptr,sizeformat(st.st_size),st.st_size);
	return 1;
	}
#endif
#undef perms
WINDOW *rightwin(int l,int c,int by,int bx) {
		if(l>LINES) {
			l=LINES;
			by=0;
			}
		if(c>COLS)	 {
			c=COLS;
			bx=0;
			}
		if(!takescr)
			takescr=newwin(l,c,by,bx);
		else {
			int x,y;
			 getbegyx(takescr,y,x);
			if(((y!=by)||(x!=bx))||(getmaxyx(takescr,y,x), (y!=l)||(x!=c))) {
				delwin(takescr);
				takescr=newwin(l,c,by,bx);
				}
			else
				touchwin(takescr);
			}
		return takescr;
		}

WINDOW *getshowwin(int l,int c,int by,int bx) {
	WINDOW *awin=rightwin(l,c,by,bx);
	if(awin) {
		attrall(awin,SHOWBACK); 
		}
	return awin;
	}

extern int lasthelp(void) ;
void helpwin(void) {
  WINDOW *tmpwin=takescr;
  takescr=NULL;
  lasthelp();
  if(takescr) delwin(takescr);
  takescr=tmpwin;
  touchwin(takescr);
 }
int  selectitemindexhelp(const char *prompt,char **files,int nr,int start,int &givehelp,int c,int l) {
#ifndef STATICKEYS
keyinput inputkeys(putkeys);
#endif
		int key;
		int nameindex;

		int xpos=1;
		int ypos=1;
		int begy=(thelines-l)/2;
		int begx=(thecols-c)/2;
		rightwin(l,c,maxnum(begy,0),maxnum(begx,0));
		if(!takescr)
			return INT_MAX;
		givehelp=0;
		werase(takescr);
		trefresh(takescr);
		winsel fil(files,nr,xpos,ypos,start,l-1,c-2);
		TITLECOLOR ;
		 mvwaddnstr( takescr,ypos-1,xpos-1,spatiebuf,c+2);
		 mvwaddnstr( takescr,ypos-1,xpos,prompt,c+1);
		fil.show();
		while(1){
			wrefresh(takescr);
			key=ggetch(takescr);
			if(!isprint(key)) {
				while((key=inputkeys.keyseq(key))==-1)
						key=ggetch(takescr);
				switch(key) {
					case -2: beep();continue;
					case HELP:
						  if(nr>0) {
							givehelp=1;
							 key=RETURN;
							 break;
						  	}
						  else {
							helpwin();
							continue;
						  }
					case 	QUIT: return INT_MAX;
					case NOP:continue;
				  };
				}
			switch(key) {
				case	REFRESH: return (INT_MAX-1);
				default: if((nameindex=fil.selectindex(key))>=0) {
					return nameindex;
					}
				};
			}
		}
int  selectitemindex(const char *prompt,char **files,int nr,int start,int col,int line) {
	int givehelp;
	for(;;) {
		 start= selectitemindexhelp(prompt,files, nr,start,givehelp,col,line) ;
		if(!givehelp)
			return start;
		helpwin();
		}
	}

//	qsort(files,len,sizeof(char *),strcmp);
void arraysorti(char **files,int len,int icase) {
	if(len<2)
		return;
	int half=len/2;
	char *pivot=files[half];
	int it1=0,it2=len-1,i;
	char **naar=myallocar(char *,len); 
	memmove(naar,files,len*sizeof(char *));
	for(i=0;i<half;i++){
		if((icase?strcmp(naar[i],pivot):strcasecmp(naar[i],pivot))<0) 
			files[it1++]=naar[i];
		else
			files[it2--]=naar[i];
		}
	for(i=half+1;i<len;i++){
		if((icase?strcmp(naar[i],pivot):strcasecmp(naar[i],pivot))<0) 
			files[it1++]=naar[i];
		else
			files[it2--]=naar[i];
		}
	 myfree(naar);
	arraysorti(files,it1,icase);
	arraysorti(files+it2+1,len-it1-1,icase);
	files[it1]=pivot;
	}
void arraysort(char **files,int len) {
 	arraysorti(files, len,1) ;
	}
/*
#define titlemaak(waar) \
	switch(waar) { \
		case 0:\
			wmove(takescr,ypos+1,xpos+1);\
			SELSELCOLOR ;\
			waddch(takescr,'F');\
			KEYCOLOR ;\
			waddstr(takescr,"iles: ");\
			break;\
		case 1:\
			wmove(takescr,ypos+1,xpos+25);\
			SELSELCOLOR ;\
			waddch(takescr,'D');\
			KEYCOLOR ;\
			waddstr(takescr,"irectories: ");\
			break;\
		case 2:\
		wmove(takescr,ypos+winh-2,xpos+1);\
			SELSELCOLOR ;\
			waddch(takescr,'N');\
			KEYCOLOR ;\
			waddstr(takescr,"ame: ");\
			break;\
		}

#define titleweg(waar) \
	switch(waar) { \
		case 0:\
			wmove(takescr,ypos+1,xpos+1);\
			KEYCOLOR ;\
			waddch(takescr,'F');\
			NORMALCOLOR ;\
			waddstr(takescr,"iles: ");\
			break;\
		case 1:\
			wmove(takescr,ypos+1,xpos+25);\
			KEYCOLOR ;\
			waddch(takescr,'D');\
			NORMALCOLOR ;\
			waddstr(takescr,"irectories: ");\
			break;\
		case 2:\
		wmove(takescr,ypos+winh-2,xpos+1);\
			KEYCOLOR ;\
			waddch(takescr,'N');\
			NORMALCOLOR ;\
			waddstr(takescr,"ame: ");\
			break;\
		}

		*/
#define titlemaak(waar) \
	switch(waar) { \
		case 0:\
			wmove(takescr,ypos+1,xpos+2);\
			SELSELCOLOR ;\
			waddstr(takescr,"iles");\
			break;\
		case 1:\
			wmove(takescr,ypos+1,xpos+26);\
			SELSELCOLOR ;\
			waddstr(takescr,"irectories");\
			break;\
		case 2:\
		wmove(takescr,ypos+winh-2,xpos+2);\
			SELSELCOLOR ;\
			waddstr(takescr,"ame");\
			break;\
		}
#define titleweg(waar) \
	switch(waar) { \
		case 0:\
			wmove(takescr,ypos+1,xpos+1);\
			KEYCOLOR ;\
			waddch(takescr,'F');\
			NORMALCOLOR ;\
			waddstr(takescr,"iles");\
			break;\
		case 1:\
			wmove(takescr,ypos+1,xpos+25);\
			KEYCOLOR ;\
			waddch(takescr,'D');\
			NORMALCOLOR ;\
			waddstr(takescr,"irectories");\
			break;\
		case 2:\
		wmove(takescr,ypos+winh-2,xpos+1);\
			KEYCOLOR ;\
			waddch(takescr,'N');\
			NORMALCOLOR ;\
			waddstr(takescr,"ame");\
			break;\
		}

void mkfilescreen(int ypos,int xpos,const char *com) {

	int len=strlen(com);

/*
	input.putseq(FILEPART,2,((unsigned char []){27,102}));
	input.putseq(DIR,2,((unsigned char []){27,100}));
	input.putseq(FIL,2,((unsigned char []){27,110}));
*/
//	trefresh(takescr);
	TITLECOLOR ;
	wmove(takescr,ypos+0,xpos+0);
	waddnstr(takescr,spatiebuf,winb);
	wmove(takescr,ypos+0,xpos+(23-(len/2)));
	waddstr(takescr,(char *)com);

	wmove(takescr,ypos+1,xpos+1);
	KEYCOLOR ;
	waddch(takescr,'F');
	NORMALCOLOR ;
	waddstr(takescr,"iles: ");
	wmove(takescr,ypos+1,xpos+25);
	KEYCOLOR ;
	waddch(takescr,'D');
	NORMALCOLOR ;
	waddstr(takescr,"irectories: ");

	wmove(takescr,ypos+winh-2,xpos+1);
	KEYCOLOR ;
	waddch(takescr,'N');
	NORMALCOLOR ;
	waddstr(takescr,"ame: ");
	}


extern char	*mygetcwd(char *name,int len) ;
char	*mygetcwd(char *name,int len) {
#ifdef __CYGWIN__
	GetCurrentDirectory(len,name);return name;
#else
	return getcwd(name,len);
#endif
	}

extern void			mychdir(const char *path);
void			mychdir(const char *path) {
#ifdef _WIN32
SetCurrentDirectory(path);
#else

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	 chdir(path);
#pragma GCC diagnostic pop
#endif
}

#ifdef _WIN32
inline void	fullpathname(char *outfile,ssize_t len,const char *infile) {
	char *ptr;
	GetFullPathName(infile,len,outfile,&ptr);
	}
#else
#define fullpathname(out,len,in) expandfilename(out,in)
#endif
const char *mvtakefile(int ypos,int xpos,const char *com,const char *pat) {
		static char *resfilename=NULL;
	char *showstr=NULL;
	int eerste=1;
	char *fil;
	char filebuf[PATH_MAX];
	char *lekt;
	int waar=2;
	
#ifndef STATICKEYS
keyinput inputkeys(putkeys);
#endif
keydef( FILEPART,{27,102});
keydef(DIR,{27,100});
keydef(FIL, {27,110});
	mkfilescreen(ypos, xpos,com);
	line lin("",xpos+7,ypos+winh-2,35);

	const char *patin=pat;
 back: {
	lekt=NULL;
	fullpathname(filebuf,MAX_PATH,patin);
	char *path=filebuf;
	char star[2]={'*','\0'};
		fil=star;
if(isdir(path)) {
			mychdir(path);


	}
else {
		if((fil=strrchr(path,FILENAMESEP[0]))) {
			*fil++='\0';
			int plen=strlen(fil);
			char *tmpstr=fil;
			fil=(char *)alloca(plen+1);
			memcpy(fil,tmpstr,plen+1);
			}
		nasplit:
		if(*path)
			mychdir(path);
		else
			mychdir(FILENAMESEP);
		}
	TITLECOLOR ;
	wmove(takescr,ypos+winh-1,xpos);
	waddnstr(takescr,spatiebuf,winb);
	wmove(takescr,ypos+winh-1,xpos+1);
	mygetcwd(filebuf,BEDSMAXPATH);
	waddnstr(takescr,filebuf,winb-1);
int fileslen;
char **files;
#ifdef SPLITGETFILES
int dirslen;
char **dirs;
fileslen=getfilessplit(fil,&files,&dirs,&dirslen) ;
	if(!eerste&&!fileslen&&!dirslen) {
#else
int	alllen=getfiles(fil,&files,&fileslen);
	if(!eerste&&!fileslen) {
#endif

		int l=strlen(fil)+1;
		if(resfilename)
			myfree( resfilename);
		resfilename=myallocar( char,l);

		strcpyn(resfilename,fil,l);
		return resfilename;
		};
#ifdef SPLITGETFILES
	arraysorti(files,fileslen,0);
	arraysorti(dirs,dirslen,0);
#else
	arraysort(files,fileslen);
	arraysort(&files[fileslen],alllen-fileslen);
#endif
	winsel sel(files,fileslen,xpos+2,ypos+2);

#ifdef SPLITGETFILES
	winsel dir(dirs,dirslen,xpos+26,ypos+2);
#else
	winsel dir(&files[fileslen],alllen-fileslen,xpos+26,ypos+2);
#endif
	winsel *vorig;
#ifdef _WIN32
	sel.icase=0;
	dir.icase=0;
#endif
	lin.setvalue(fil,strlen(fil));
	sel.show();
	dir.show();
	terug:
	lin.show();
	eerste=0;
	switch(waar) {
		case 0:
			dir.leave();
			vorig=&sel;
			sel.showsel(0);
			break;
		case 1:
			sel.leave();
			vorig=&dir;
			dir.showsel(0);
			break;
		case 2:
		default: 
			sel.leave();
			dir.leave();
			vorig=&sel;
			lin.take();
		};
	titlemaak(waar);
//	trefresh(takescr);
	while(1) {
		showstr=vorig->current();
		NORMALCOLOR;
		wmove(takescr,ypos+winh-3,xpos+1);
		waddnstr(takescr,spatiebuf,winb-2);
		wmove(takescr,ypos+winh-3,xpos+1);
		if(showstr)
			filegeg(showstr,filebuf);
		waddnstr(takescr,filebuf,winb-2);
		lin.cursor();
		int key;
	keyagain:
		wrefresh(takescr);
		key=ggetch(takescr);
	DEBUGGING("Start: %d=ggetch()\n",key)
		if(!(isprint(key)) ) {
			while((key=inputkeys.keyseq(key))==-1) {
				key=ggetch(takescr);
				DEBUGGING("%d=ggetch()\n",key)
				}
			switch(key) {
				case HELP:helpwin();goto keyagain;
				case NOP:goto keyagain;
				case -2: beep(); goto keyagain;
				case QUIT: return NULL;
				case FILEPART:
					if(vorig!=&sel) {
						vorig->leave();
						vorig=&sel;
						}
					sel.showsel(0);
					titleweg(waar);
					waar=0;
					titlemaak(waar);
					continue;
				case DIR:
					if(vorig!=&dir) {
						vorig->leave();
						vorig=&dir;
						}
					titleweg(waar);
					dir.showsel(0);
					waar=1;
					titlemaak(waar);
					continue;


				case FIL: 

					titleweg(waar);
					vorig->leave();
					waar=2;lin.take();
					titlemaak(waar);
					continue;
				case SHIFTAB: titleweg(waar);waar--; if(waar<0) waar=2; goto terug;
				};
			}
	switch(key) {
		case REFRESH: return pat;
		case TAB: titleweg(waar);waar++;
			if(waar>2) 
				waar=0;
			goto terug;
			break;
		default: {
			lekt=NULL;
			switch(waar) {
				case 0: lekt=sel.select(key); 
					if(lekt)  {
						return lekt;
						}
					break;
				case 1: lekt=dir.select(key); 
					if(lekt)  {
							path=lekt;
							fil=star;
							goto nasplit;
							};
					break;
				case 2: lekt=lin.change(key); 
					if(lekt) {
						titleweg(waar);
						     waar=0;
						     /*
						     	if(lekt[0]=='~') {
								memmove(lekt+1,lekt,strlen(lekt)+1);
								lekt[0]='\\';
								}
								*/
							patin=lekt;
							goto back;
							};
					break;
				};
			};
		};
	}
	}	
	}

void resetinput(void) {
				nocbreak();
				noecho();
				cbreak();
				nonl();
	}
const char *takefile(const char *com,const char *pat) {
	int l=winh,c=winb;
	int begy=(thelines-l)/2;
	int begx=(thecols-c)/2;
	rightwin(l,c,maxnum(begy,0),maxnum(begx,0));
	if(takescr) {
		attrall(takescr,BACKATTR);
		const char  *ret;
	SHOWCURSOR;
		ret=mvtakefile(0,0,com,pat);
		cursoraway();
		return ret;
		}
	return NULL;
	}


char item::nul=0;

	item::item(const char *txt1,int ya,int xb) {
		strcpyn(text,txt1,maxtext);
                text[maxtext-1]=0;
		if((after=strchr(text,'~'))) {
			*after++='\0';
			ch=*after++;
			}
		else {
			after=&nul;
	//		after="";
			ch=' ';
			}
		lowch=tolower(ch);
		x=xb;
		y=ya;
		};


//template <typename T=bool, T min,T max,bool actret>  const char  *const check2<T,min,max,actret>::options = " x#*%-+";


//template <typename T=bool, T min,T max,bool actret> constexpr  static	const char  check2<T,min,max,actret>::options[] = " x#*%-+";

	void action::show(void) {
		SELCOLOR;
	//	NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
	//	NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELCOLOR;
	//NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		};
	void action::select(void) {
		SELSELCOLOR;
	//	NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
	//	NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		SELSELCOLOR;
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		};

	int ok::saveall(void) {
		var=1;
		return 2;
		}


	void lineitem::show(void) {
		SELCOLOR;
	//	NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
	//	NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
		SELCOLOR;
	//NOWARN(wattrset(optionscreen,CHECKCOLOR);)
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		l.show();
		}
	void lineitem::select(void) { 
		SELSELCOLOR;
	//	NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		wmove(optionscreen,y,x);
		waddstr(optionscreen,text);
		KEYCOLOR;
	//	NOWARN(wattrset(optionscreen,KEYCHECKCOLOR);)
		waddch(optionscreen,ch);
//		NOWARN(wattrset(optionscreen,SELCHECKCOLOR);)
		SELSELCOLOR;
		waddstr(optionscreen,after);
		wmove(optionscreen,y,x);
		l.take();
		}

	int lineitem::saveall(void) {
		return 2;
		}

void label(int y,int x,const char *str) {
		SELCOLOR;
		wmove(takescr,y,x);
		waddstr(takescr,(char *)str);
		};

int show(item **al,int maxar,int inold) {
	int i,news=inold,ch,res,old=inold;
	if(inold>=maxar) {
		news=0;old=maxar-1;
		};

//	NOWARN(wattrset(takescr,SHOWBACK);)
#ifdef LINEMENUS
	box(takescr,0,0);
#endif

#ifndef STATICKEYS
keyinput inputkeys(putkeys);
#endif
startover:	
	unsigned char k[2];
	k[0]=27;
	for(i=0;i<maxar;i++) {
		al[i]->show();
		k[1]=al[i]->thekey();

		inputkeys.putseq(i|0xfff000,2,k);
		}
	k[1]=al[old]->thekey();
	inputkeys.putseq(old|0xfff000,2,k); /*Waarom? old buiten maxar?*/
	al[old]->select();

	SHOWCURSOR;
	wrefresh(takescr);

bool	nextreturn=false;
	while(1){
	//	keyagain:
		ch=ggetch(takescr);
		if(!(isprint(ch)) ) {
			while((ch=inputkeys.keyseq(ch))==-1)
				ch=ggetch(takescr);
			 switch(ch) {
				 case HELP:helpwin();goto startover;
				case NOP: goto foundone;
				case -2: beep(); goto foundone;
				case QUIT : cursoraway(); return 0;
				case SHIFTAB: news--;if(news<0) news=maxar-1; goto foundone;
				case TAB:	news++;if(news>=maxar) news=0; ;
goto foundone;
				case RETURN: {
						     
DORETURN:
					  if((res=al[old]->saveall())) {
						if(res==2) {
							for( i=0;i<maxar;i++) {
								al[i]->save();
	//							delete al[i];
								}

							cursoraway();
				//			return old+1;
							return news+1;
							}
						cursoraway();
						return news+1;
			//			return old+1;
						}
					     };
					goto foundone;
				case REFRESH: 
					cursoraway();
					return INT_MAX;
					  };
			  }
			if((0xfff000&ch)==0xfff000) {
				const int key=ch&0xfff;
				if(key>=0&&key<maxar) {
					news=key;
					goto foundone;
					}
				}
			if(al[old]->change(ch)==0x7FFFFFFF)
	//			nextreturn=true;
			  goto DORETURN;

foundone:
		if(old!=news) {
			if(nextreturn) {
				nextreturn=false;
				goto DORETURN;
				}
			al[old]->show();
			al[news]->select();
			old=news;
			}
		wrefresh(takescr);
		}
	}


#ifdef TEST5
void main(void) {
	int i,reg=0,cas=1,maxar=4;
	char get[256]="*";

	initscreen() ;

	int wlin=50,wcol=80;
	char *str[4];
	str[0]="een";
	str[1]="twee";
	str[2]="drie";
	char ant[80];
	while(1) {
		lcconfig(wlin,wcol,
			oke(2,5),
			act("C~ancel", 2,15),
			list("~File",4,5,str,3,ant));
		
		move(0,0);
		addstr(ant);
		refresh();
		}
	getch();
	endwin();
	}

#endif
#ifdef TEST2
void main(void) {
	int i,reg=0,cas=1,maxar=4;
	char get[256]="*";

	initscreen() ;

	int wlin=50,wcol=80;
	while(1) {

		Common combase(reg),*com=&combase;
		plcsconfig(wlin,wcol,2,com->last=(orcheck*)showformsarray[reg], ore("~Een",2,3,0,com),ore("~Twee",3,3,1,com), 
		ore("~Drie",4,3,2,com), 
		ore("~Vier",5,3,3,com), 
			liner("~Name",get,7,3),
			oke(12,5),
			act("C~ancel", 12,15)); 

		printw("reg=%d, cas=%d get=%s\n",reg,cas,get);
		refresh();
		}
	getch();
	endwin();
	}
#endif

#ifdef TEST
void main(int argc,char **argv) {
	int f,b;
	const char *file,*prompt,*orig;
	if(argc<3) {
		file="*";
		if(argc<2)
			prompt="Select file";
		else
			prompt=argv[1];
		}
	else  {
		 file=argv[2];
		 prompt=argv[1];
		 }
	orig=file;
	do {
		initscreen();
		file=takefile(prompt,file);
		endwin();
		} while(file==orig);
	if(file)
		write(3,file,strlen(file));
	}
#endif
#ifdef TEST3

const char *getfile(const char *prompt,const char *ask) {
	const char *file=ask;
			while(1) {
				if(!(file=takefile(prompt,ask))) {
					return NULL;
					}
				if(file!=ask)  {
					return file;
					}
				erase();
				}
			}
#include "offtypes.h"
int main(int argc,char **argv) {
	const char *file,*prompt;
	STATTYPE st;
	if(argc<3) {
		file="*";
		if(argc<2)
			prompt="Select file";
		else
			prompt=argv[1];
		}
	else  {
		 file=argv[2];
		 prompt=argv[1];
		 }
	char buf[BEDSMAXPATH]="*";

	initscreen();
	strcpyn(buf,file,BEDSMAXPATH);
		while(1) {
			if((file=getfile("Open file",buf))) {
				if(STAT(file,&st)==-1|| !(S_ISREG(st.st_mode))) {
					printw("Can not edit %s",file);
					wrefresh(stdscr);
					strcpyn(buf,file,BEDSMAXPATH);
					continue;
					}
				else {
					if(file)
						write(3,file,strlen(file));
					endwin();
					return 0;
					}
				}
			else {
					endwin();
				return -1;
				}
			}
	endwin();
	return 0;
	}
#endif

