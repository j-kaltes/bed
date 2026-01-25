#include "global.h"
BEDPLUGIN

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#define menunum 9
#include CURSES_HEADER
#define font(nr)  static char font##nr[]="\033]50;#" #nr "\7";
font(6)
font(5)
font(4)
font(3)
font(2)
font(1)
static char larger[]=	"\033]50;#+\07";
static char smaller[]=	"\033]50;#-\07";
static char reverse[]=	"\033[?5t";
static char normal[]= "\033]50;#\07";
static char scrollbar[]= "\033[?30t";

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
#define GeenEditor
class Editor {
static int  GeenEditor deletemenu(int menunr) ;
static int  GeenEditor deletemenuitem(int menunr,int pos) ;
static int  GeenEditor addmenuitem(int menunr,const char *name,int pos, int ( Editor::*proc)(void),int insert=0,const char *keystr="") ;
static int  GeenEditor addmenu(const char *name,int menunr,int insert=0) ;
static int menus(void) ;
int rewrite(void);
#define termproc(name) int to##name(void) { chterm(name);return 0; };
	public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
termprocs
#pragma GCC diagnostic pop
int xtermmenu(void) {
static int ismenu=0;
#define address(class,proc) &class::proc
if(!ismenu) {
	addmenu("~XTerm",menunum,1);
	addmenuitem(-1,"Font ~1",-1, address(Editor,tofont1));
	addmenuitem(-1,"Font ~2",-1, address(Editor,tofont2));
	addmenuitem(-1,"Font ~3",-1, address(Editor,tofont3));
	addmenuitem(-1,"Font ~4",-1, address(Editor,tofont4));
//	addmenuitem(-1,"Font ~5",-1, address(Editor,tofont5));
//	addmenuitem(-1,"Font ~6",-1, address(Editor,tofont6));
	addmenuitem(-1,(char *)NULL,-1, (int (Editor::*)()	)NULL);
	addmenuitem(-1,"~Normal",-1, address(Editor,tonormal));
	addmenuitem(-1,"~Larger",-1, address(Editor,tolarger),0,"S-KP+");
	addmenuitem(-1,"~Smaller",-1, address(Editor,tosmaller),0,"S-KP-");
	addmenuitem(-1,(char *)NULL,-1, (int (Editor::*)()	)NULL);
	addmenuitem(-1,"~Reverse",-1, address(Editor,toreverse));
	addmenuitem(-1,"Scroll~bar",-1, address(Editor,toscrollbar));
	ismenu=1;
	}
else {
	deletemenu(menunum);
	ismenu=0;
	}
menus();
return 0;
}
};
sethelp(xtermmenu,"Adds xterm menu.\nYou can remove the scrollbar, reverse video and if you don't have\na buggy xterm you can change fonts.");
#undef termproc

#define termproc(name) call(to##name)
termprocs
call(xtermmenu)
