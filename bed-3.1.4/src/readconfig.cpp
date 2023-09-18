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


#include <alloca.h>
#include <features.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "screen.h"
#include "system.h"
#include "keyproc.h"
#include "myalloc.h"
#include "readconfig.h"
#include "execproc.h"
#include "config.h"
#ifndef HAVE_SETENV
#include "setenv.h"
#endif
const char *replaceversion(char *tmp,const char *infile) ;
char *versionabsolutefilename(char *uitfile,const char *infile);
extern int newfilesize;
extern char macrodirectory[];
extern char datatypedirectory[]	;
extern char bedrcbin[];
#ifndef NOATTRIBUTES
struct nametonum {char lab[8];int num;} colo[] = { {"black",COLOR_BLACK}, {"red",COLOR_RED}, {"green",COLOR_GREEN}, {"yellow",COLOR_YELLOW}, {"blue",COLOR_BLUE}, {"magentA",COLOR_MAGENTA}, {"cyan",COLOR_CYAN}, {"white",COLOR_WHITE},{"default",-1}};
int nrcolors=sizeof(colo)/sizeof(struct nametonum);

struct attr {
	const char lab[12];
	const chtype num;
	const char des[80];
	};
struct attr at[] = {
{"normal",A_NORMAL,"Normal display (no highlight)"},
{"standout",A_STANDOUT,"Best highlighting mode of the terminal."},
{"underline",A_UNDERLINE,"Underlining"},
{"reverse",A_REVERSE,"Reverse video"},
{"blink",A_BLINK,"Blinking"},
{"dim",A_DIM,"Half bright"},
{"bold",A_BOLD,"Extra bright or bold"},
{"protect",A_PROTECT,"Protected mode"},
{"invis",A_INVIS,"Invisible or blank mode"},
{"altcharset",A_ALTCHARSET,"Alternate character set"},
{"chartext",A_CHARTEXT,"Bit-mask to extract a character"}};
int nrattr=sizeof(at)/sizeof(struct attr);
//s/{\([^}]*\)}/setfield(\1)/g
#define setfield(name,var,col) {name,var,col,-1,-1}
#ifdef COMPILECOLORS
#define setcolorfield(name,forg,backg,attr,col) {name,attr,col,forg,backg}
Fieldcol fieldcolor[] = {
setfield("text",0,1),
setfield("command",A_REVERSE,2),

#ifdef PDCURSES
//setcolorfield("bar",COLOR_RED,COLOR_WHITE|A_REVERSE,0,3),
setcolorfield("bar",COLOR_WHITE,COLOR_BLUE,A_BOLD,3),
#else
setcolorfield("bar",COLOR_WHITE,COLOR_BLUE,0,3),
#endif
#ifdef PDCURSES
setcolorfield("barkey",COLOR_BLACK,COLOR_WHITE,A_BOLD|A_UNDERLINE,4),

#else
setcolorfield("barkey",COLOR_BLUE,COLOR_YELLOW,A_BOLD|A_REVERSE,4),
#endif

#ifdef PDCURSES
setcolorfield("menu",COLOR_WHITE,COLOR_BLUE, A_BOLD ,5),
#else
setcolorfield("menu",COLOR_WHITE,COLOR_BLUE, A_NORMAL ,5),
#endif

#ifdef PDCURSES
setcolorfield("menukey",COLOR_GREEN,COLOR_BLACK,A_REVERSE,6),
#else
setcolorfield("menukey",COLOR_BLUE,COLOR_YELLOW,A_BOLD|A_REVERSE,6),
#endif
setcolorfield("menusel",COLOR_WHITE,COLOR_BLUE,A_REVERSE,7),

setcolorfield("opentitle",COLOR_BLACK,COLOR_WHITE,A_BOLD,8), 
setcolorfield("openkey",COLOR_WHITE,COLOR_GREEN,A_BOLD|A_REVERSE,9),
#ifdef _WIN32
setcolorfield("opennormal",COLOR_MAGENTA,COLOR_YELLOW,A_REVERSE|A_BOLD,10),
//setcolorfield("openlist",COLOR_CYAN,COLOR_MAGENTA,A_BOLD,11),
setcolorfield("openlist",COLOR_RED,COLOR_BLACK,A_BOLD|A_REVERSE,11),

setcolorfield("openlistsel",COLOR_BLACK,COLOR_YELLOW,A_NORMAL,12),
//setcolorfield("openlistact",COLOR_YELLOW,COLOR_GREEN,A_BOLD,13),
setcolorfield("openlistact",COLOR_BLACK,COLOR_CYAN,A_NORMAL,13),

#else
setcolorfield("opennormal",COLOR_WHITE,COLOR_MAGENTA,A_BOLD,10),
setcolorfield("openlist",COLOR_BLACK,COLOR_CYAN,A_BOLD,11),
setcolorfield("openlistsel",COLOR_GREEN,COLOR_MAGENTA,A_NORMAL,12),
setcolorfield("openlistact",COLOR_RED,COLOR_WHITE,A_BOLD|A_REVERSE,13),
#endif
setcolorfield("showback",COLOR_BLUE,COLOR_WHITE,0,14),
setfield("selection",A_REVERSE,15), 
setcolorfield("cursor",COLOR_RED, COLOR_YELLOW,A_STANDOUT|A_BOLD|A_BLINK,16), 
setfield("selcursor",A_REVERSE|A_BLINK,17), 
setfield("spaceafter",A_REVERSE,18),
setfield("apart",A_BOLD,19),
setcolorfield("cursorpas",COLOR_CYAN,COLOR_MAGENTA,A_REVERSE,20) 
};
#else
Fieldcol fieldcolor[] = {
setfield("text",0,1),
setfield("command",A_REVERSE,2),setfield("bar",A_REVERSE,3),
setfield("barkey",A_NORMAL,4),setfield("menu",A_REVERSE,5),setfield("menukey",A_NORMAL,6),
setfield("menusel",A_BOLD|A_UNDERLINE,7),
setfield("opentitle",A_BOLD,8), setfield("openkey",A_BOLD,9),
setfield("opennormal",0,10),
setfield("openlist",A_REVERSE,11),setfield("openlistsel",A_NORMAL,12),
setfield("openlistact",A_NORMAL|A_BOLD,13),setfield("showback",A_BOLD,14),setfield("selection",A_REVERSE,15), setfield("cursor",A_REVERSE|A_BOLD,16), setfield("selcursor",A_UNDERLINE|A_BOLD,17),
setfield("spaceafter",A_REVERSE,18),
setfield("apart",A_BOLD,19),
setfield("cursorpas",A_REVERSE,20) 
};
#endif
int colortypes=sizeof(fieldcolor)/sizeof(Fieldcol);
int defaultcolor=colortypes+1;
#endif
char *toendline(char *str) {
	if(!str)
		return NULL;
	char *end=strpbrk(str,"\n\012\015#");
	if(!end||!*end)
		return NULL;
	if(*end=='#') {
		*end='\0';
		return NULL;
		}
	*end++='\0';
	while(isspace(*end))
		end++;
	if(!*end||*end=='#') {
		*end='\0';
		return NULL;
		}
	return end;
	}
char *next(char *str) {
	if(!str)
		return NULL;
	char *end=strpbrk(str," 	\n\012\015#");
	if(!end||!*end)
		return NULL;
	if(*end=='#') {
		*end='\0';
		return NULL;
		}
	*end++='\0';
	while(isspace(*end))
		end++;
	if(!*end||*end=='#') {
		*end='\0';
		return NULL;
		}
	return end;
	}

int colornr(char *name) {
	int i;
	for(i=0;i<nrcolors;i++) {
		if(!strcasecmp(colo[i].lab,name))
			return colo[i].num;
		}
	return -2;
	}
#define NOATTR 0x4fff

chtype attrnr(char *attr){
	for(int i=0;i<nrattr;i++)
		if(!strcasecmp(at[i].lab,attr))
			return at[i].num;
	return 0;
	}
chtype getattr(char *str) {
	if(!str)
		return 0;
	char *at=next(str);
	chtype val=getattr(at);
	chtype att=attrnr(str);
	return val+((att==NOATTR)?0:att);
	}

#define MESSSIZE 256
char mess[MESSSIZE];
#include "varmacro.h"

#ifdef OLDGCCVARMACRO
#define configmessage(for...) snprintf(mess,MESSSIZE,##for) VARMACRO
#else
#define configmessage(...) snprintf(mess,MESSSIZE,__VA_ARGS__) 

#endif
#ifdef STATIC
#undef DLDEFAULTCOLORS
#endif
	
#if defined(NODEFAULTCOLORS) || defined(DLDEFAULTCOLORS)
int  giveback=COLOR_WHITE,givefore=COLOR_BLACK;
int defaultbackground=COLOR_WHITE, defaultforeground=COLOR_BLACK;
#else
int defaultbackground=-1, defaultforeground=-1;
#endif
//int textsameback;
#ifdef COMPILECOLORS
extern int blackwhite;
int blackwhite=STARTBLACKWHITE;
#endif
#ifdef DLDEFAULTCOLORS
#include "dlfcn.h"
int (*assume_default_colorsptr)(int, int)=NULL;
int get_assume_default_colors(void) {
	const char *error=NULL;
		assume_default_colorsptr =(int (*)(int,int)) dlsym( RTLD_DEFAULT, "assume_default_colors");
		if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "dlsym: %s\n", error);
			assume_default_colorsptr=NULL;
			return -1;
			}
		return 0;
}
/*
int get_assume_default_colors(void) {
	const char *error=NULL;
	void *han=dlopen(NCURSESLIB,RTLD_LAZY|RTLD_GLOBAL);
	if(han) {
		assume_default_colorsptr =(int (*)(int,int)) dlsym(han, "assume_default_colors");
		if ((error = dlerror()) != NULL)  {
//			fprintf (stderr, "dlsym: %s\n", error);
			assume_default_colorsptr=NULL;
			return -1;
			}
		return 0;
		}
	else {
		if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "dlopen: %s\015\012", error);
			}
		}
	return -1;
	}
*/
int lukt=get_assume_default_colors();
#endif
int resetcolors(void) {
#ifdef COMPILECOLORS
	int hasc=((!blackwhite)?has_colors():0);
	if(hasc) {
		start_color();
#ifdef DLDEFAULTCOLORS
		if(assume_default_colorsptr)
			assume_default_colorsptr(-1,-1);
#else
#ifndef NODEFAULTCOLORS
	assume_default_colors(defaultforeground,defaultbackground);
#endif
#endif
		}
//	init_pair(defaultcolor,defaultforeground,defaultbackground);
	for(int i=0;i<colortypes;i++) 
		{
			fieldcolor[i].var=((~A_COLOR) & fieldcolor[i].var);
			if(hasc
#ifdef DLDEFAULTCOLORS
			&&(fieldcolor[i].forg!=-1||
(assume_default_colorsptr&& fieldcolor[i].back!=-1))
#else
#if defined(NODEFAULTCOLORS)

			&&fieldcolor[i].forg!=-1
#else
&&(fieldcolor[i].forg!=-1 || fieldcolor[i].back!=-1)
#endif
#endif
					) {
					init_pair(fieldcolor[i].colornr,fieldcolor[i].forg,fieldcolor[i].back);
					fieldcolor[i].var|= COLOR_PAIR(fieldcolor[i].colornr);
					}
		}
/*
	int colornr= defaultcolor+1;
//	init_pair(colornr,COLOR_GREEN,fieldcolor[0].back);
	init_pair(colornr,COLOR_GREEN,COLOR_BLUE);
	textsameback=(fieldcolor[0].var&(~A_COLOR)) |COLOR_PAIR(colornr); 
*/
#endif
	return 0;
	}
int makeattr(char *str) {
	if(!str)
		return -1;
	char *fore,*back;
	chtype val;
	int i;
	for(;isspace(*str);str++)
		;
	if(!*str||*str=='#') {
		return 1;
		}
	if(!(fore=next(str))) {
		configmessage("Error after %s",str);
		return -1;
		}
	for(i=0;i<colortypes;i++) {
	 	if(!strcasecmp(str,fieldcolor[i].text)) {
			back=next(fore);
			if(!back||(fieldcolor[i].forg=colornr(fore))==-2) {
				fieldcolor[i].forg=-1;
				fieldcolor[i].back=-1;
				fieldcolor[i].var=attrnr(fore)+getattr(back);
				return 0;
				}
			val=getattr(next(back));
			if((fieldcolor[i].back=colornr(back))==-2) {
				fieldcolor[i].forg=-1;
				fieldcolor[i].back=-1;
				configmessage("Error don't recognise %s",back);
				return -1;
				}

#ifdef DLDEFAULTCOLORS
		if(!assume_default_colorsptr) {
#endif
#if defined(NODEFAULTCOLORS)||defined(DLDEFAULTCOLORS)

			if(fieldcolor[i].forg==-1)
				fieldcolor[i].forg=defaultforeground;
			if(fieldcolor[i].back==-1)
				fieldcolor[i].back=defaultbackground;
#endif
#ifdef DLDEFAULTCOLORS
		}
#endif
			
			fieldcolor[i].var=val;
			return 0;
			}
		}
	configmessage("Don't recognise %s",str);
	return -1;
	}

int mkground(char *forg) {
	if(!forg)
		return -1;
	char *back=next(forg);
	if(!back) {
		configmessage("Missing background after %s",forg); return -1;
		}
	int fg=	colornr(forg);
	if(fg==-2) {
		configmessage("Unknown color name %s",forg); return -1;
		}
#if defined(NODEFAULTCOLORS)||defined(DLDEFAULTCOLORS)
#ifdef DLDEFAULTCOLORS
		if(!assume_default_colorsptr&&fg==-1)
#else
	if(fg==-1)
#endif
		defaultforeground=givefore	;
	else
		
#endif
	defaultforeground=	fg;
	char *end=next(back);
	int bg=	colornr(back);
	if(bg==-2) {
		configmessage("Unknown color name %s",back); return -1;
		}
#if defined(NODEFAULTCOLORS)||defined(DLDEFAULTCOLORS)
#ifdef DLDEFAULTCOLORS
	if(!assume_default_colorsptr&&bg==-1)
#else
	if(bg==-1)
#endif
		defaultbackground=giveback	;
	else
#endif
	defaultbackground=	bg;
	return 0;
	}









constexpr int MAXLINE=400;

void strup(char *gegs) {
	char *ptr;
	for(ptr=gegs;*ptr;ptr++)
		*ptr=tolower(*ptr);
	}
extern const int MAXKEYS;
const int MAXKEYS=100;
int putproc(const char *str,int nr,unsigned char keys[]) ;
extern int getfilesym[];
#define keylabel(label,edit,menu,open) label,
const char *keysyms[]= { 
#include "keylist.h"
};
int keysnr=sizeof(keysyms)/sizeof(char *);
#include "editor.h"
#include "keyinput.h"
#include "getlabel.h"
//#ifndef __CYGWIN__
#if defined(__linux__)
#ifdef  HAVE_NCURSES_NCURSES_H 
       #include <ncurses/term.h>
#else
       #include <term.h>
#endif
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
int setkeystring(unsigned char *keybase,int nr,const char *str) {

#ifdef STATICKEYS
extern  keyinput inputkeys;
#else
extern void putkeys(class keyinput *keyinp) ;
keyinput inputkeys(putkeys);
#endif

	int i=nr-1;
	unsigned char *keys=keybase+1;
	int sym;
	if(i==1&&keys[0]==27)
		keys[i++]=secondescape;
	int j;
	if((j=getlabel(keysyms,keysnr,str))>=0) {
		int (Editor::*proc)(void);
		if((proc=Editor::ineditproc[j])) 
		       setkeymap(keyedit,keys,i,proc);
		 if((proc=Editor::inmenuproc[j])) 
		       setkeymap(keyedit,keybase,i+1,proc);
		 if((sym=getfilesym[j]))
			inputkeys.putseq(sym,i,keys);
		return 0;
		};
	switch(putproc(str,i,keys)) {
		case -1: configmessage("Unknown procedure %s",str); return -1;
		case 1:  {
			int j;
			char *ptr;
			strcpyn(mess,"Defined Prefix of   " ,MESSSIZE); 
			for(j=0,ptr=mess+18;j<i;j++) {
				snprintf(ptr,4,"%d ",keys[i]);
				ptr=ptr+strlen(ptr);
				}
			return -1;
			};
		default: return 0;
		}
	}
#pragma GCC diagnostic pop
int gettermcap(const char *input,unsigned char *output) {
#ifdef HAS_TIGETSTR
	char *out=tigetstr(const_cast<char *>(input));
	if(!out||out==(char *)-1L) {
		return (ssize_t)out;
		}
	int len=strlen(	out);
	memmove(output,out,len);
	return len;
#else
return 0;
#endif
	}

extern int (Editor::*nameproc(const char *str))(void) ;
static const char  inserttoken[]="+";
static const int inserttokenlen=(sizeof(inserttoken)-1);
int deletemenu(char *strin) {
	int menunum=-1;
	if(strin) {
		char *endptr,*str=strin;
		while(isspace(*str))
			str++;
		menunum=strtol(str, &endptr, 0); 
		if(str==endptr) {
			menunum=-1;
			}
		else
			str=endptr;
		while(isspace(*str))
			str++;
		if(*str) {
			configmessage("extra characters %s",str); 			return -1;

			}
		}
	return Editor::deletemenu(menunum);
	}
int deletemenuitem(char *strin) {
	int menunum=-1,menuitemnum=-1;
	if(strin) {
		char *endptr,*str=strin;
		while(isspace(*str))
			str++;
		if(*str) {
			menunum=strtol(str, &endptr, 0); 
			if(str==endptr) {
				menunum=-1;
				}
			else
				str=endptr;
			while(isspace(*str))
				str++;
			if(*str) {
				menuitemnum=strtol(str, &endptr, 0); 
				if(str==endptr) {
					menunum=-1;
					}
				else
					str=endptr;
				while(isspace(*str))
					str++;
				if(*str) {
					configmessage("extra characters %s",str); 			return -1;

					}
				}
			else {
				menuitemnum=menunum;
				menunum=-1;
				}
			}
		}
	return Editor::deletemenuitem(menunum,menuitemnum);
	}
int makemenuitem(char *strin) {
	char *str=strin,*endptr,*name=NULL,*endzero;
	char nul=0;
	char *keystr=&nul;
	int (Editor::*proc)(void)=NULL;
	int menunum=-1,menuitemnum=-1,insert=0;;
	while(isspace(*str))
		str++;
	if(*str!='\"') {
		menunum=strtol(str, &endptr, 0); 
		if(endptr==str) {
			menunum=-1;
			}
		else {
			str=endptr;
			while(isspace(*str))
				str++;
			}
		}
	else
		menunum=-1;
	if(*str!='\"') {
			if(strncasecmp(str,"LINE",4)) {
				configmessage("menu string %s should be surrounded by \"\"",str); 			return -1;
				}
			name=str;
			int end=strcspn(str, "	 \n\r");
			endzero=name+end;
			str=endzero;
			}
	else {
		str++;
		name=str;
		endzero=strchr(str,'\"');
		if(!endzero)	{
			configmessage("missing last \" in %s",str); 			return -1;
			}
		str=endzero+1;
//		*endzero='\0';
		}
	while(isspace(*str))
		str++;
	if(*str) {
		if(*str=='\"') {
			str++;
			keystr=str;
			endptr=strchr(str,'\"');
			if(!endptr)	{
				configmessage("missing last \" in %s",str); 			return -1;
				}
			str=endptr+1;
			*endptr='\0';
			while(isspace(*str))
				str++;
			}
			menuitemnum=strtol(str, &endptr, 0); 
			if(endptr==str)
				menuitemnum=-1;
			else {
				str=endptr;
				while(isspace(*str))
					str++;
				}
			if(!memcmp(str,inserttoken,inserttokenlen)) {
				insert=1;
				str+=inserttokenlen;
				while(isspace(*str))
					str++;
				}
			if(str[0]=='=') {
				str++;
				while(isspace(*str))
					str++;
				int end=strcspn(str, "	 \n\r");
				char *procname=str;
				if(str[end]) {
					str[end]='\0';
					str+=(end+1);
					while(isspace(*str))
						str++;
					if(*str) {
						configmessage("characters after %s",procname); 			return -1;
						}
					}
				
				proc=nameproc(procname);
				}
			}
	*endzero='\0';
	if(!proc) {
		if(strncasecmp(name,"LINE",4)) {
			configmessage("nothing after %s\nmenuitem [menunum] [A-Za-z]*~[a-zA-Z][a-zA-Z]* [menupos]=procedure\nor menuitem [menunum] LINE [menupos]",name); 			return -1;
			}
		name=NULL;
		}
	else {
		char *has=strchr(name,'~');
		if(!has) {

			configmessage("one character in '%s' should be prefixed with '~'",name); 			return -1;
		}
	if(!has[1]) {
		configmessage("one character after '~' in '%s'",name);return -1; 
		}
		
		}
	return Editor::addmenuitem(menunum,name,menuitemnum, proc,insert,keystr);
	}
int makemenu(char *strin) {
	char *str=strin,*endptr,*name;
	int menunum=-1,insert=0;
	if(!str)
		return -1;
	while(isspace(*str))
		str++;

	if(*str!='\"') {
		menunum=strtol(str, &endptr, 0); 
		if(endptr==str) {
			menunum=-1;
			}
		else {
			str=endptr;
			while(isspace(*str))
				str++;
			}
		}
	else
		menunum=-1;

	if(*str!='\"') {
		configmessage("menu string %s should be surrounded by \"\"",str); 			return -1;
		}
	else {
		str++;
		name=str;
		endptr=strchr(str,'\"');
		if(!endptr)	{
			configmessage("missing last \" in %s",str); 			return -1;
			}
		str=endptr+1;
		*endptr='\0';
		}
	while(isspace(*str))
		str++;

	if(!memcmp(str,inserttoken,inserttokenlen)) {
		insert=1;
		str+=inserttokenlen;
		while(isspace(*str))
			str++;
		}
	if(*str)	{
			if(*str) {
				configmessage("extra characters %s",str); 			return -1;
				}
			}
	char *has=strchr(name,'~');
	if(!has) {

		configmessage("one character in '%s' should be prefixed with '~'",name); 			return -1;
		}
	if(!has[1]) {
		configmessage("one character after '~' in '%s'",name);return -1; 
		}
	return Editor::addmenu(name, menunum,insert);
	}
#include "cursesinit.h"
extern int after_initscr;
extern nocurses_init curses_init_curses;
int makekeys(char *str) {
	if(!after_initscr) {
		curses_init_curses.curses_init();
		defaultkeys(keyedit);
		}
	if(!str)
		return -1;
	unsigned char keybase[MAXKEYS],*keys=keybase+1;
	keybase[0]=menukey;
	int i=0;
	char *endptr;
	while(isspace(*str))
		str++;
	do {
		int get=strtol(str, &endptr, 0); 
		if(endptr==str) {
			while(isspace(*str))
				str++;
			break;
			}
		keys[i++]=get;
		str=endptr;
		while(isspace(*str)||*str==',')
			str++;
		} while(*str!='=');
	if(i==0) {
		int  end=strcspn(str, " 	=");
		char endch=str[end];
		str[end]='\0';
		if((i=gettermcap(str,keys) )<=0) {
			if(i==-1)  {
				configmessage("%s no terminfo capability",str); 			
				return -1;
				}
			return 0;
			}

		str[end]=endch;
		str+=end;
		while(isspace(*str))
			str++;
		}
	if(*str!='='){
		configmessage("Missing =");
		return -1;
		};
	str++;
	while(isspace(*str))
		str++;
	if(!*str||*str=='#') {
			configmessage("Missing function after = ");
			return -1;
			}
	char *func=next(str);
	if(func) {
		configmessage("Characters after %s",str);
		return -1;
		}

	return setkeystring(keybase,i+1,str);
	}

int dynlink(const char *name) ;

extern int doargs(int beg,int margc,char **margv) ;
extern int addprocedure(int (Editor::*proc)(void)) ;
extern int			straddprocedure(char *name) ;
extern char confdir[];
char  confdir[]=CONFDIR "/plugins/examples";
char *plugindir=confdir;
int plugindirlen=sizeof(CONFDIR);
#define keywordargs char *gegs,char *ptr,int l
#define keywordreturn void


#undef initwordproc 
#undef initwordname
#undef initword 
#define realprocname(name) name##_initprocedure
#define initwordname(namne,proc) 
#define initwordproc(name) keywordreturn realprocname(name)(keywordargs);
#define initword(name) initwordproc(name)

#include "bedrcproc.h"
#undef initwordproc 
#undef initwordname
#undef initword 
#define initwordproc(proc)
#define initword(name) #name,
#define initwordname(name,proc)  initword(name)
const char *keywordname[] ={
#include "bedrcproc.h"
};
int keywordnr=sizeof(keywordname)/sizeof(char*);
#undef initwordname
#undef initword 
#define initword(name) realprocname(name),
#define initwordname(name,proc)  initword(proc)
keywordreturn  (*keywordproc[])(keywordargs) ={
#include "bedrcproc.h"
};
#undef initwordproc 
#undef initwordname
#undef initword 

#define initwordptr(name) realprocname(name)

	int outofcontext=0;
int readconfig(char *file) {
	char gegsbuf[MAXLINE],*ptr,*gegs;
	FILE *fp=fopen(file,"rb");
//#ifdef HAVE_SETENV
#if 1
#ifdef PDCURSES
	setenv("CURSES_VERSION","PDCURSES",1);
#else
#ifdef NCURSES_VERSION
	setenv("CURSES_VERSION","NCURSES",1);
#else
	setenv("CURSES_VERSION","UNKNOWN",1);
#endif
#endif
#ifdef USE_KEYPAD
	setenv("CURSES_KEYPAD","y",1);
#endif
#endif
	if(!fp) { return -1;
		}
//fprintf(stderr,"Using %s\n",file);
	for(int l=0;fgets(gegsbuf,MAXLINE,fp);l++) {
		for(gegs=gegsbuf;isspace(*gegs);gegs++)
			;
		if(!*gegs||*gegs=='#')
			continue;
		ptr=next(gegs);
		 int index;
		keywordreturn (*procedure)(keywordargs);
		if((index=getlabelcase(keywordname, keywordnr,gegs))<0)
			procedure=initwordptr(defaulthandler);
		else
			procedure=keywordproc[index];
		if(outofcontext&& procedure!=initwordptr(context)){
			continue;
			}
		procedure(gegs,ptr,l);
		}
	fclose(fp);
	return 0;
	}
	

#undef initwordname
#undef initwordproc
#undef initword
#define initword(name) keywordreturn name##_initprocedure(keywordargs)
#define initwordname(execute,call_on_open)
#define initwordproc(name) initword(name)
#ifdef _WIN32
#ifdef __CYGWIN__
#include <sys/cygwin.h>
#include "win.h"
#define		expandunixname(naar,van)  absunixpath(van ,naar)
#else
#include "system.h"
#define		expandunixname(outname,inname)  expandfilename(outname,inname) 
#endif
#else
#define		expandunixname(outname,inname)  expandfilename(outname,inname) 
#endif
#include "bedrcproc.cpp"
#undef initwordname
#undef initwordproc
#undef initword

#include "system.h"
char readingconfig=1;
extern char *getconfig(char *configname) ;
char *getconfig(char *configname) {
	char *conf=getenv(CONFFILE);
	if(!conf) 
		conf=getenv(CONFVAR);
	if(conf) {
		expandunixname(configname,conf);
		return configname;
		} 	
#ifdef _WIN32
//	conf=getenv("LOCALAPPDATA");
	conf=getenv("APPDATA");
	if(conf) {
		expandunixname(configname,conf);
		int len=strlen(configname);
		configname[len++]='/';
		strcpy(configname+len,CONFFILE);
		}
	else
#endif
		expandunixname(configname,PERSCONFFILE);
	if(access(configname,0)>=0) {
		return configname;
		}
//	expandunixname(configname,GLOBALCONFFILE);
	strcpy(configname,GLOBALCONFFILE);
	if(access(configname,0)>=0) {
		return configname;
		}
	return NULL;
	}
int readconfig(void) {
	readingconfig=1;
	char configname[BEDSMAXPATH];
	if(getconfig(configname)) {
		if(readconfig(configname)>=0) {
			readingconfig=0;
			return 0;
			}
		else {
			fprintf(stderr,"Can't open %s\015\012", configname);
			readingconfig=0;
			return 1;
			}
		}
#ifdef __CYGWIN__
#warning change this
//	char *conf=getenv("LOCALAPPDATA");
	char *conf=getenv("APPDATA");
if(conf)  {
//	fputs("Can't read \07%%LOCALAPPDATA%%\\" CONFFILE " or \07",stderr);
	fputs("Can't read \07%%APPDATA%%\\" CONFFILE " or \07",stderr);
	}
else {
	fputs("Can't read ",stderr);
	winpath( PERSCONFFILE,configname); 
	fputs(configname,stderr);
	fputs(" or ",stderr);
	}
	winpath( GLOBALCONFFILE,configname); 
	fputs(configname,stderr);
	fputc('\n',stderr);
#else
	fprintf(stderr,"Can't read %s or %s\07\015\012",PERSCONFFILE,GLOBALCONFFILE );
#endif
	readingconfig=0;
	return 1;
	}
struct gegs {
		const char *name;
		 int (Editor::*proc)(void);
		};
int comproc(const void *one,const void *two) {
	return strcmp(((const struct gegs *)one)->name,((const struct gegs *)two)->name);
	}

extern int adddynprocedures(const  char **newdynprocnames, int (Editor::**newdynproc)(void),int nr) ;
#ifdef NODL
int dynlink(const char *name) {
	configmessage("Can't use plugin: %s, bed is compiled with NODL defined\015\012",name);
	return -1;
	}
#else 
#include <dlfcn.h>

/*
don't add dlclose because of initialisation procedures like 
addprocedure(proc)
registerfilterclass(filtername) 
datatype(type)
*/
#ifdef PROCARRAY
#ifdef __linux__
#define	linkdynprocnr	  "_6Editor.dynprocnr"
#define linkdynproc	"_6Editor.dynproc"
#define linkdynprocnames "_6Editor.dynprocnames"
#else
#ifdef __FreeBSD__
#define	linkdynprocnr	  "_6Editor$dynprocnr"
#define linkdynproc	"_6Editor$dynproc"
#define linkdynprocnames "_6Editor$dynprocnames"
#endif
#endif
#endif

int dynlink(const char *name) {
	const char *error;
	const char *file=name;
#ifdef PROCARRAY
	int *nrp;
#endif
	if(name[0]!='/'&&name[0]!='~'&&name[0]!='.') {
			int len=strlen(name)+1;
			char *namebase=(char *)alloca(plugindirlen+len);
			memmove(namebase,plugindir,plugindirlen-1);
			namebase[plugindirlen-1]='/';
			memmove(namebase+plugindirlen,name,len);
			if(access(namebase,F_OK)) {
				char *namebase2=(char *)alloca(BEDSMAXPATH);
				if(getcwd(namebase2,BEDSMAXPATH)) {
					int end= strlen(namebase2);
					namebase2[end]='/';
					memmove(namebase2+end+1,name,len);
					if(access(namebase2,F_OK))
						name=namebase;
					else
						name=namebase2;
					}
				else
						name=namebase;
				}
			else
				name=namebase;
			}
	int oldinitprociter=execopen?execopen->initprociter:0;
	void *handle=dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
	if(!handle) {
		 if ((error = dlerror()) != NULL)  {
			int len=strlen(name);
			if(!strncmp(error,name,len))
				error+=len;
			configmessage("%s: %s",file,error);
			}
		else
			configmessage("Can't open %s",file);
		return -1;
		}

	char **bedplugin=(( char**) dlsym(handle,"bedplugin"));
	 if ((error = dlerror()) != NULL)  {
		configmessage("bedplugin: %s",error);
		if(execopen)
			execopen->initprociter=oldinitprociter;
		dlclose(handle);
		return -1;
		}
	if(!bedplugin) {
		configmessage("&bedplugin=NULL");
		if(execopen)
			execopen->initprociter=oldinitprociter;
		dlclose(handle);
		return -1;
		}

#ifdef PROCARRAY
	nrp=((int*) dlsym(handle,linkdynprocnr));
	 if ((error = dlerror()) != NULL||!nrp)  {
		return 0;
		   }
	else {
	int (Editor::**newdynproc)(void)= (int (Editor::**)(void)) dlsym(handle,linkdynproc);
	 if ((error = dlerror()) != NULL ||!newdynproc)  {
		configmessage("%s: Editor::dynproc: %s",file,error);
		return -1;
	   }
	const char **newdynprocnames=(const char **) dlsym(handle,linkdynprocnames);
	 if ((error = dlerror()) != NULL || !newdynprocnames)  {
		configmessage("%s: Editor::dynprocnames: %s",file,error);
		return -1;
		   }

		return adddynprocedures( newdynprocnames, newdynproc,*nrp) ;
		}
#endif
	return 0;
	}


#endif /*NODL */
extern int adddynprocedure(const char *name,int (Editor::*proc)(void)) ;
int adddynprocedure(const char *name,int (Editor::*proc)(void)) {
	return adddynprocedures(&name,&proc,1);
	}

int adddynprocedures( const char **newdynprocnames, int (Editor::**newdynproc)(void),int nr) {
	int newnr=Editor::dynprocnr+nr;
	int (Editor::**olddynproc)(void)=Editor::dynproc;
	const char **olddynprocnames=Editor::dynprocnames;
	Editor::dynprocnames=myallocar( const char *,newnr);
	Editor::dynproc=myallocarp( int (Editor::**)(void),int (Editor::*)(void) ,newnr);


	struct gegs *newdyn=myallocar( struct gegs,nr);
	for(int n=0;n<nr;n++) {
		newdyn[n].name=newdynprocnames[n];
		newdyn[n].proc=newdynproc[n];
		};
	qsort(newdyn,nr,sizeof(struct gegs),comproc);
	
	int o=0,n=0,naar=0;
	if(Editor::dynprocnr) {
			for(o=0,n=0;(o<Editor::dynprocnr)&&(n<nr);) {
				int ver=strcmp(newdyn[n].name,olddynprocnames[o]);
				if(ver) {
					if(ver>0)	{
						Editor::dynprocnames[naar]=olddynprocnames[o];
						Editor::dynproc[naar++]=olddynproc[o++];
						}
					else {
						Editor::dynprocnames[naar]=newdyn[n].name;
						Editor::dynproc[naar++]=newdyn[n++].proc;
						}
					}
				else {
					 Editor::dynprocnames[naar]=newdyn[n].name;
					 Editor::dynproc[naar++]=newdyn[n++].proc; 
					 o++;
					 }
				}
			}
	if(n>=nr) {
		if(o<Editor::dynprocnr) {
			memmove(&Editor::dynprocnames[naar],&olddynprocnames[o],(Editor::dynprocnr-o)*sizeof(char *));
			memmove(&Editor::dynproc[naar],&olddynproc[o],(Editor::dynprocnr-o)*sizeof(int (Editor::*)(void)));
			naar+=(Editor::dynprocnr-o);
			}
		}
	else
		for(;n<nr;n++) 	{
			   Editor::dynprocnames[naar]=newdyn[n].name;
			   Editor::dynproc[naar++]=newdyn[n].proc;
				}
	if(Editor::dynprocnr>0) {
		myfree(olddynprocnames);
		myfree(olddynproc);
		}
	myfree(newdyn);
	Editor::dynprocnr=naar;
	return 0;


	}

//#define TEST
#ifdef TEST
int main(int argc,char **argv) {
	initscr();
	           clearok(stdscr,FALSE);
	                           clearok(curscr,FALSE);
	                                    noecho();
	                                                    cbreak();
	                                                                    nonl();

	clear();
	bool	hasc=has_colors();
		if(hasc)
			start_color();
		readconfig(argv[1]);
	assume_default_colors(fieldcolor[0].forg,fieldcolor[0].back);
		refresh();
		for(int i=0;i<colortypes;i++) {
			attrset(fieldcolor[i].var);
			move(i+10,0);
			refresh();
			addstr(fieldcolor[i].text);
			refresh();
			}
		wgetch(stdscr);
	endwin();
	}
#endif
