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

#include <ctype.h>
#include <string.h>
#include <math.h>
#include "system.h"
#include "main.h"
#include "readconfig.h"
#include "dialog.h"

#include "editor.h"
#include "screenpart.h"
#include "keyproc.h"
#include "screen.h"

#define cat(x,y) x ## y



#undef prodef
#define prodef(pr) funcptr(Editor,pr),
#undef prodefname
#define prodefname(name,pr) funcptr(Editor,pr),


int (Editor::*Editor::proc[])(void)={
	#include "procedures.h"
	};

#undef prodefname
#define prodefname(name,pr) #name,
#undef prodef
#define prodef(pr) #pr,
const char *Editor::procnames[] = {
	#include "procedures.h"
	};
int Editor::procnr=sizeof(Editor::procnames)/sizeof(char *);



#define keylabel(label,edit,menu,open) menu,
int (Editor::*Editor::inmenuproc[])(void)={
#include "keylist.h"
};
#undef keylabel
#define keylabel(label,edit,menu,open) edit,
int (Editor::*Editor::ineditproc[])(void)={
#include "keylist.h"
} ;

int (Editor::**Editor::dynproc)(void);
const char **Editor::dynprocnames;
int Editor::dynprocnr=0;

#include "getlabel.h"

int (Editor::*nameproc(const char *str))(void) {
	int i;
	i=getlabel(Editor::procnames,Editor::procnr,str);
	if(i>=0)
		return Editor::proc[i];
	i=getlabel(Editor::dynprocnames,Editor::dynprocnr,str);
	if(i>=0)
		return Editor::dynproc[i];
	return (int (Editor::*)(void)) NULL;
	}


int putproc(const char *str,int nr,unsigned char keys[]) {
	int (Editor::*func)(void)=nameproc(str);
	if(func)
		return setkeymap(keyedit,keys,nr,func);
	else
		return -1;
	}


extern int showhelp( int (Editor::*proc)(void)) ;

int selectprocedure(Editor *ed,const char **procnames,int procnr,int (Editor::**proc)(void),const char *label,int &take) {
	int ant,help=0;
	while((ant=selectitemindexhelp(label,(char **)procnames,procnr,take,help,43))!=INT_MAX) {
			if(ant==(INT_MAX-1)) {
				ed->menuresized();
				wrefresh(ed->editscreen);
				}
			else {
				take=ant;
				if(help) {
					::showhelp(proc[ant]);
					ed->editup();
					}
				else {
					ed->editup();
					clcallproc(ed,proc[ant]);
					return take;
					}

				 }
		}
	ed->editup();
	return -1;
	}
inline int selectprocedure(Editor *ed,char **procnames,int procnr,int (Editor::**proc)(void),const char *label,int &take) {
return  selectprocedure(ed,(const char **)procnames, procnr, proc,label,take) ;
}
#define selectbody(proc,label)\
	static int take=0;\
	return ::selectprocedure(this,proc##names,proc##nr,proc,label,take);

int Editor::selectprocedure(void) {
	selectbody(proc,"Procedures F1=help Enter=execute");
	}
int Editor::selectdynprocedure(void) {
	selectbody(dynproc,"Plugin Procedures F1=help Enter=execute");
	}


char *keystring(unsigned char *in,char *uit,int len) {
	char *ptr=uit;
	for(int i=0;i<len;i++)
		ptr+=sprintf(ptr,"%u ",in[i]);
	return ptr;
	}

int getkeys(unsigned char *keys,int maxkeys) ;
int mykbhit(void) ; 
const char *procedurename(int (Editor::*proc)(void)) {
	int i;
	for(i=0;i<Editor::procnr;i++) {
		if(proc==Editor::proc[i]) {
			return Editor::procnames[i];
			}
		}
	for(i=0;i<Editor::dynprocnr;i++) {
		if(proc==Editor::dynproc[i]) {
			return Editor::dynprocnames[i];
			}
		}
	return NULL;
		}
int Editor::showprocedure(void) {
	const char *name;
	int ret=0;
	const int MAXOUTBUF=256;
	char outbuf[MAXOUTBUF]="Nothing";
	int outbufiter=0;
	const int maxkeys=40;
	unsigned char inputkeys[maxkeys];
	char keys[140];
	output("Type keys (no movement keys within menu)");erefresh();
	do {
	int ch,nr=0;
	void  *procedure=NULL;
	int k;
	while((nr=getkeys(inputkeys,maxkeys))<1) 
			;

	refresh();
	if(!commandmode) {
		if(((ch=parts[mode]->newchar(inputkeys[0]))>=0))  {
			outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"Builtin: %c (%u)",inputkeys[0],inputkeys[0]);
			goto leaveshow;
			}
		}
	for(k=0;(k<nr)||(nr+=getkeys(inputkeys+nr,maxkeys-nr));k++) {
		if(inputkeys[0]==27&&k==1) {
			for(int i=0;i<menuiter;i++) {
				if(menulist[i]&&menulist[i]->key==inputkeys[1]) {
					int mkey;
					resetkey(keyedit);
					if(nr<3)	{
						touchwin(menulist[i]->win);
						wrefresh(menulist[i]->win);
						mkey=ggetch(stdscr);	
						filesup();
						}
					else
						mkey=inputkeys[2];
					struct menu *m=menulist[i];
					if(isprint(mkey)) {
					for(int j=0;j<m->len;j++)
						if(m->el[j].key==mkey) {
							char label[80];
							int keypos=m->el[j].keypos;
							memcpy(label,m->el[j].name,keypos);
							label[keypos]='~';
							strcpy(label+keypos+1,m->el[j].name+keypos);
							if((name=procedurename(m->el[j].proc))) {
			
							char *alt= m->el[j].alt;
							const char *sep= alt[0]?"\"":"";
							outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"menuitem %d \"%s\" %s%s%s %d  = %s",i,label,sep,alt,sep,j,name);
							}
else {
	outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"System Error: can't find the name for this procedure");
	}
							goto leaveshow;
							}
						}

					message("%c not in menu Alt-%c",mkey,inputkeys[1]);
					goto leaveshow;
					}
				}
			}
		if((procedure=lookkey(keyedit,inputkeys[k])))
			break;
		if(endkey(keyedit) ) {
			beep();
			flushinp();
			keystring(inputkeys,keys,nr);
			outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"No keybinding for %s",keys);
			goto leaveshow;
			}
		}
	char *ptr;
	k++;	
	ptr=keystring(inputkeys,keys,k);
	if(k<nr) {
		*ptr++='(';
		ptr=keystring(inputkeys+k,ptr,nr-k);
		*(ptr-1)=')';
		}
	name=procedurename(procproc(keyedit,procedure));
	if(name) {
		outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"keys %s= %s",keys,name);
		 }
	else
		outbufiter+=snprintf(outbuf+outbufiter,MAXOUTBUF-outbufiter,"System error: I forgot the name (%s)",keys);
	leaveshow:
	output(outbuf);
	erefresh();
	usleep(500000);
	outbuf[outbufiter++]=',';
	outbuf[outbufiter++]=' ';
	} while(mykbhit());
return ret;
}


	#define minnum( x,y) ((x)>(y)?(y):(x))
#include "execproc.h"
static int proclistcon=123;
	Proclist::Proclist(void) {
		initproc=NULL;
		initprocnr=0;
		 initprociter=0;
		proclistcon=4;
		}
	Proclist::~Proclist() {
		if(initprocnr>0) {
			 initprociter=initprocnr=0;
			myfree( initproc	);
			}
		}
	int Proclist::addprocedure(int (Editor::*proc)(void)) {
		if(initprociter<=initprocnr) {
			int oldinitprocnr __attribute__ ((unused)) =	initprocnr;
			initprocnr=2*initprocnr+10;
			initproc= myrealloc( int (Editor::**)(void) ,initproc,int (Editor::*)(void),initprocnr,oldinitprocnr);
			}
		initproc[initprociter++]=proc;
		return 0;
		}
	int			Proclist::straddprocedure(char *name) {
		int i;
		i=getlabel(Editor::procnames,Editor::procnr,name);
		if(i>=0) {
			return addprocedure(Editor::proc[i]);
			}
		i=getlabel((const char **)Editor::dynprocnames,Editor::dynprocnr,name);
		if(i>=0) {
			return addprocedure(Editor::dynproc[i]);
			}
		return -1;
		}
	int Proclist::executeinitproc(Editor *ed) {
		if(initprociter>0)
			ed->toscreen();
		for(int i=0;i<initprociter;i++) {

			clcallproc(ed,initproc[i]);
			}
		return 0;
		};
Proclist *execswitch=NULL,*execopen=NULL,*execones=NULL;

int Editor::executeinitproc(void) {
	initproclist(execopen);
	return execopen->executeinitproc(this);
	}


 int addprocedure(int (Editor::*proc)(void)) {
	return call_open(proc) ;
	}
 int			straddprocedure(char *name) {
			return str_call_open(name) ;
	}
int call_open(int (Editor::*proc)(void)) {
	initproclist(execopen);
	return execopen->addprocedure(proc) ;
	}
int str_call_open(char *name) {
	initproclist(execopen);
	return execopen->straddprocedure(name) ;
	}
int call_switch(int (Editor::*proc)(void)) {
	initproclist(execswitch);
	int ret= execswitch->addprocedure(proc) ;
	return ret;
	}

int str_call_switch(char *name) {
	initproclist(execswitch);
	return execswitch->straddprocedure(name) ;
	}
int call_once(int (Editor::*proc)(void)) {
	if(!execones){
		execones=new Proclist;
		}
	return (execones->addprocedure(proc));
	}
int str_call_once(char *name) {
	if(!execones){
		execones=new Proclist;
		}
	return (execones->straddprocedure(name));
	}

#ifdef NODL
int Editor::opendynlib(void) {
	beep();	
	message(PROGRAM "should be compiled without NODL defined");
	return -1;
	}
#else

extern char  confdir[];
extern char mess[];
extern int plugindirlen;
extern char *plugindir;
extern int dynlink(const char *name) ;

int Editor::opendynlib(void) {
		const char *file;
		char cwdold[BEDSMAXPATH];
		char *got=getcwd(cwdold,BEDSMAXPATH);
			while(1) {
				if((file=getfile("Load Plugin",plugindir))) {
					char newcwd[BEDSMAXPATH];
					if(getcwd(newcwd,BEDSMAXPATH) ) {
						plugindirlen=strlen(newcwd)+1;
						if(plugindir!=confdir)
							myfree( plugindir);
						plugindir=myallocar( char,plugindirlen);
						memcpy(plugindir,newcwd,plugindirlen);
						}
					mess[0]='\0';
					if(dynlink(file)<0) {
						beep();
						message(mess);
						wrefresh(editscreen);
						continue;
						}
					else {
						editup();
					message("Plugin %s loaded: %s",file,mess);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
if(got)
						chdir(cwdold);
#pragma GCC diagnostic pop
						return 0;
						}
					}
				else {
					editup();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
if(got)
						chdir(cwdold);
#pragma GCC diagnostic pop
					return -1;
					}
				}
			}
#endif
