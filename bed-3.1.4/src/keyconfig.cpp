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

#include <signal.h>
#undef ERR
#include "config.h"
#include CURSES_HEADER
#include "config.h"
#ifndef PD24
#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <signal.h>
#include <ctype.h>
#undef ERR
#include "screen.h"
#include "editor.h"
#include "myalloc.h"
#include "onlykey.h"
#include "system.h"
#include "keyproc.h"


extern int getkeys(unsigned char *keys,int maxkeys) ;
/*
struct keydeffer {
	char *name;
	char *proc;
	};
#define KeydefType struct keydeffer 
#define elk(x,y,z) ((struct keydeffer) {y,#z})
#define sym(x) (struct keydeffer) {x,x}
#define toproc(i) keydefto[i].proc
#define toname(i) keydefto[i].name
#define two(name,proc)  {name,#proc}
*/

struct keydeffer {
	const char   *name;
	const char  *proc;
	};
#define KeydefType struct keydeffer 
#define elk(x,y,z)  {y,#z}
#define sym(x)  {x,x}
#define toproc(i) keydefto[i].proc
#define toname(i) keydefto[i].name
#define two(name,proc)  {name,#proc}

#define tonameel(i,x) toname(i)[x]
#define	LINE {NULL,NULL}
#define elm(x,z) LINE
//#define elm(x,z)
//#define	LINE 
#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define menumake(file,name,els...)		,##els VARMACRO
#else
#define menumake(file,name,...)		,__VA_ARGS__
#endif
KeydefType keydefto[]={
	sym("home"), sym("end"), sym("up"), sym("down"), sym("left"), sym("right"), sym("pgup"), sym("pgdn"), sym("stab"), sym("delete"), sym("backspace"), sym("esc") ,sym("f1")
/*	
	two("F1",moveshow)
	elk("~Save","F2",dosave),
	elk( "~Nextsearch","F3",nextsearch), 
	elk("~Prev","F5",prevfile),  
	elk("~Next","F6",nextfile), 
	elk("A~dd Prev","S-F5",addprev), 
	elk("~Add Next","S-F6",addnext),
	elk("~Prev","F7",prevmode), 
	elk("~Next","F8",nextmode), 
	elk("Beg~in Pg","F9",beginpage),
	elk("~Begin file", "S-F9",beginfile),
	elk("En~d Pg","F10",endpage),
	elk("~End file", "S-F10",endfile),
	elk("~Select","F12",setselect), 
//	elk( "Ne~xt Mark","Tab",nextmark), 
	elk("P~revious Mark","M-Tab",prevmark)
*/
//#include "../src/menu.h"
#include "menu.h"
	};
int total=sizeof(keydefto)/sizeof(KeydefType);
//C-x
//M-x
#define nam(x) tonameel(i,x)
#define cond (toname(i)&&!(nam(1)=='-'&&(nam(0)=='C'||nam(0)=='M')&&isalpha(nam(2))&&nam(3)=='\0'))

#define printkeys { if(fprintf(fp,"keys ")<1) { fprintf(stderr,"Can't write to %s",filename);return -1; } ; for(j=0;j<nr;j++) if(fprintf(fp," %d",keys[j])<1) { fprintf(stderr,"Can't write to %s",filename);return -1; } ; if(fprintf(fp," = %s #%s\n",toproc(i),toname(i))<1) { fprintf(stderr,"Can't write to %s",filename);return -1; } ; }











void endscreen(void) {
	if(!isendwin()) {
		endwin();
		putchar('\n');
		}
	}


#ifdef _WIN32
#ifdef CYGWIN
#include <sys/cygwin.h>
#endif
#include "win.h"
#endif
int keyconfig(char *filename=NULL) {
fprintf(stderr,"start keyconfig\n");
#ifdef STATICKEYS
extern keyinput inputkeys;
#else

extern void putkeys(class keyinput *keyinp) ;
keyinput inputkeys(putkeys);
#endif
	atexit(endscreen);
	int wrotekeys=0;
	const int maxkeys=40;
	unsigned char keys[maxkeys];
	int nr,j,i;
	FILE *fp;
	void  *procedure;
	scrollok(stdscr,1);
	nl();
	SHOWCURSOR;
	clear();
	if(!filename) {
//#ifdef Windows
#ifdef  __CYGWIN__
		filename =(char * )alloca(BEDSMAXPATH) ;
#endif
		char *conf=getenv(CONFFILE);
		if(!conf) 
			conf=getenv(CONFVAR);
		if(conf) {
#ifdef  __CYGWIN__
			absunixpath(conf,filename);
#else
			filename=conf;
#endif
			}
		else {
//#ifndef _WIN32
#ifndef  __CYGWIN__
		filename =(char * const)alloca(BEDSMAXPATH) ;
#else

//		conf=getenv("LOCALAPPDATA");
		conf=getenv("APPDATA");
		if(conf) {
			absunixpath(conf,filename);
			strcat(filename,"/" CONFFILE);
			}
		else
#define         expandfilename(naar,van)  absunixpath(van ,naar)
#endif
			expandfilename(filename,PERSCONFFILE);
			if(!(fp=fopen(filename,"rb"))) { /*IF PERSCONFFILE doesn't exist, create and copy GLOBALCONFFILE */
				fp	= fopen(filename,"wb");
				if(!fp) {
					printw("Can't open %s\n",filename);
					endscreen();
					fprintf(stderr,"Can't open %s\n",filename);return -1;
					}
const char * const general= GLOBALCONFFILE;
				FILE *genfp	= fopen(general,"rb");
				if(genfp) {
					int end=-ftell(genfp);
					fseek(genfp,0L,SEEK_END);
					end+=ftell(genfp);
					fseek(genfp,0L,SEEK_SET);
					char gegs[end];
					if(fread(gegs,end,1,genfp)!=1) {
						printw("reading error while reading from %s\n",general);return -1;  }
					if(fwrite(gegs,end,1,fp)!=1) {
						printw("writing error while writing to %s\n",filename);return -1;  }
					fclose(genfp);fclose(fp);
					}
				else
					fclose(fp);
				}
			else
				fclose(fp);
			}
		}
	if(filename&& (fp=fopen(filename,"ab"))) {
	int nrf;
	printw("Testing if Alt works:\nPress f\n");
	nrf=getkeys(keys,maxkeys);
	printw("Press Alt-f\n");
	nr=getkeys(keys+nrf,maxkeys-nrf);
	if(nrf==nr&&!memcmp(keys,keys+nrf,nr)) {
		printw("Alt-f and f give the same keycodes\nEnable Alt keys before you continue\r\n");
		}

	printw("Key definition will be appended to %s\n",filename);
		for(i=0;i<total;i++)	{
			if(cond) {
				printw("Press %s: ",toname(i));
				nr=getkeys(keys,maxkeys);
			for(int k=0;(k<nr)||(addstr(" type some more keys: "),nr+=getkeys(keys+nr,maxkeys-nr));k++) {
				if((procedure=lookkey(keyedit,keys[k])))
					goto next;
				if(endkey(keyedit) ) {
					break;
					}
				}
		for(int k=0;k<nr;k++) {
			intptr_t keyp;
			if((keyp=inputkeys.keyseq(keys[k]))!=-1&&keyp!=-2)
				goto next;
			if(endkey(inputkeys) ) {
				break;
				}
			}
	if(!wrotekeys) {
		char *term;
		if((term=getenv("TERM")))
			if(fprintf(fp,"\nCONTEXT TERM %s\n# apply if shell variable TERM=%s\n",term,term)<1)
			{

		endscreen();
			fprintf(stderr,"Can't write to %s\n",filename);return -1;
			}
		wrotekeys++;
		}
				printkeys;

				addstr(" Added\n");
				continue;
				next:
				addstr(" Already bounded\n");
				}
			}
		if(wrotekeys) {
			if(fprintf(fp,"CONTEXT TERM\n#general context\n")<1)
{
		endscreen();
			fprintf(stderr,"Can't open %s\n",filename);return -1;
		}
			move(LINES-3,0);
			printw("Keys appended to %s",filename);
			}
		fclose(fp);
		}
	else {
		endscreen();
			fprintf(stderr,"Can't open %s\n",filename);return -1;
		}
	return 0;
	}

#endif
