#include "global.h"
BEDPLUGIN
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "config.h"
#include CURSES_HEADER
#define font(nr)  static char font##nr[]="\E]50;#" #nr;
font(6)
static char font1[]=	"\E]50;#1";
static char font2[]=	"\E]50;#2";
static char font3[]=	"\E]50;#3";
static char font4[]=	"\E]50;#4";
static char font5[]=	"\E]50;#5";
static char larger[]=	"\E]50;#+";
static char smaller[]=	"\E]50;#-";
static char reverse[]=	"\E[?5t";
static char normal[]= "\E]50;#";
static char scrollbar[]= "\E[?30t";

#define chterm(type) write(STDOUT_FILENO,type,sizeof(type))
#define termprocs\
	termproc(font1)\
	termproc(font2)\
	termproc(font3)\
	termproc(font4)\
	termproc(font5)\
	termproc(font6)\
	termproc(normal)\
	termproc(smaller)\
	termproc(larger)\
	termproc(reverse)\
	termproc(scrollbar)
class Editor {
static int Editor::deletemenu(int menunr) ;
static int Editor::deletemenuitem(int menunr,int pos) ;
static int Editor::addmenuitem(int menunr,char *name,int pos, int ( Editor::*proc)(void),int insert=0,char *keystr="") ;
static int Editor::addmenu(char *name,int menunr,int insert=0) ;
static int menus(void) ;
int rewrite(void);
#define termproc(name) int to##name(void) { chterm(name);return 0; };
	public:
termprocs
int trymenu(void) {
static int ismenu=0;
//#define address(class,proc) &class::proc
if(!ismenu) {
	int menupos;
for(int i=0;i<10;i+=2) {
	menupos=2;
	if(addmenu("~Rxvt",menupos,1)>=0) {
		addmenuitem(menupos,"Font ~1",-1, address(Editor,tofont1));
		addmenuitem(menupos,"Font ~2",-1, address(Editor,tofont2));
		addmenuitem(menupos,"Font ~3",-1, address(Editor,tofont3));
		addmenuitem(menupos,"Font ~4",-1, address(Editor,tofont4));
		addmenuitem(menupos,"Font ~5",-1, address(Editor,tofont5));
		addmenuitem(menupos,"Font ~6",-1, address(Editor,tofont6));
		addmenuitem(menupos,NULL,-1,editcast NULL);
		addmenuitem(menupos,"~Normal",-1, address(Editor,tonormal));
		addmenuitem(menupos,"~Larger",-1, address(Editor,tolarger),0,"S-KP+");
		addmenuitem(menupos,"~Smaller",-1, address(Editor,tosmaller),0,"S-KP-");
		addmenuitem(menupos, NULL,-1,editcast NULL);
		addmenuitem(menupos,"~Reverse",-1, address(Editor,toreverse));
		addmenuitem(menupos,"Scroll~bar",-1, address(Editor,toscrollbar));
		}
	}
	ismenu=1;
	}
else {
	for(int i=0;i<10;i+=2) 
		deletemenu(2);
	ismenu=0;
	}
menus();
return 0;
}
int delmenu(void) {
	deletemenu(1);
	deletemenu(2);
	deletemenu(3);
	}
};

#undef termproc

#define termproc(name) call(to##name)
termprocs
call(trymenu)
call(delmenu)
