#include "editor.h"
#ifdef HAVE_TCSETATTR
#include <termios.h>
#endif
#include <signal.h>
#include "system.h"
#include "main.h"
#include "cursesinit.h"
#include "shell.h"

struct help {
	int (Editor::*proc)(void) ;
//	char *name;
	char *text;
	short *ind;
	short nr;
	};


//#define helpname(proc) &Editor::proc,#proc,proc##name,proc##ind
#define helpname(proc) &Editor::proc,proc##name,proc##ind
#define helpprocname(procname,proc) &Editor::proc,procname##name,procname##ind
#include "helptext.h"
#include "helpindex.h"

struct help helplist[] = {
#include "helpdata.h"
};
#include "crc.c"
#define castcast (int (Editor::**)(void))
//#define compfunctie(x,y)  (*(castcast x) == *(castcast y))
extern int startterminal;
extern struct termios  starttermios; 
extern void otherscreen(void);
extern void bedscreen(void) ;
#define shellstop
typedef void (*sighan_t) (int);

#ifndef NOSIGNALS
#ifdef shellstop
sighan_t oldstop= SIG_IGN;
#endif
sighan_t oldcont= SIG_DFL;
#endif
void otherscreen(void)
{
//reset_shell_mode();
#ifndef NOSIGNALS
oldcont=signal(SIGCONT,SIG_IGN);
#ifdef shellstop
oldstop=signal(SIGTSTP,SIG_DFL);
#endif
#endif
#ifndef NOENDWIN
if(!isendwin())
  endwin();
#ifdef HAVE_TCSETATTR
if(!startterminal) 
  tcsetattr(STDIN_FILENO, TCSANOW ,&starttermios );
#endif
#endif
  SHOWCURSOR;
}
void bedscreen(void) {
if(isendwin())
	wrefresh(stdscr);
if(curses_init_curses.isresized()) {
	resized();
	rewriteall();
	}
#ifndef PDCURSES
#endif
	HIDECURSOR;
#ifndef NOSIGNALS
#ifdef shellstop
signal(SIGTSTP, oldstop);
#endif

signal(SIGCONT,oldcont);
#endif
}
int compfunctie(const char * const x,const char *const y)  {
 	int (Editor::**px)(void)=castcast x;
 	int (Editor::**py)(void)=castcast y;
	return memcmp(px, py, sizeof(int (Editor::*)(void)));
	}
#define hashfunctie(str) block_crc((unsigned char*)str, sizeof(int (Editor::*)(void)))
#define Hash HashHelp
#include "hash.h"
static int helpnr=20+sizeof(helplist)/sizeof(helplist[0]);
static Hash *helphash;
void addhelp(struct help *item) {
	POINTER *ptr;
	if(!helphash)
		return;
	ptr=helphash->itemptr((char *)&(item->proc));
	*ptr=(POINTER*)item;
	}
void inithelphash(void) {
	helphash=new Hash(helpnr);
	}
extern int fillhelp(void);
int fillhelp(void) {
	if(!helphash)
		inithelphash();	
	for(int i=0;i<helpnr;i++) {
		addhelp(&helplist[i]);
		}
	return helpnr;
	}
//static int nothing=fillhelp();
#undef Hash
#include "main.h"
#include "dialog.h"

extern int mytmpname(char *buf,int len,const char *name) ;
int (Editor::*helpproc)(void)=NULL;
int showhelp( int (Editor::*proc)(void)) {
	struct help *helpitem;
	int startlin=0;
	if(helphash&&(helpitem=(struct help*)helphash->getitem((char *)&proc))) {
		int maxlin=thelines-6;
		int maxscreen= minnum(helpitem->nr,maxlin);
		short *ind=helpitem->ind;
#define pagelines "LINES_USE_PAGER"
		char *tmpname;
		static int lines_use_pager=((tmpname=getenv(pagelines))?strtol(tmpname,(char **)NULL,0):50);
		static char *pager=getenv("PAGER");
		
	if(pager&&helpitem->nr>lines_use_pager) {
			char tmpitemfile[256];
		if(mytmpname(tmpitemfile,256,"bedhelp-XXXXXX")<0) {

		edits[editfocus]->message("Failed to create to create temporary file");
			return -1;
			}
			FILE *fp=fopen(tmpitemfile,"w");
			for(int it=0;it<helpitem->nr;it++) {	

					fprintf(fp,
#ifdef _WIN32
"%s\015\012"
#else
"%s\n"
#endif
,helpitem->text+(it?ind[it-1]:0));
				}
			fclose(fp);
			otherscreen();
int ret=process(pager,tmpitemfile);
/*
#ifdef _WIN32
int ret=process(pager,tmpitemfile);
#else

			char command[256];
			snprintf(command,256,"%s %s",pager,tmpitemfile);
			int ret= system(command);
#endif
*/
			remove(tmpitemfile);
			bedscreen();


//			if(ret==-1||WEXITSTATUS(ret)) {
			if(ret) {

				edits[editfocus]->message("%s %s failed, switched to internal pager",pager,tmpitemfile);
					}
			else 
					return 0;
			}
		for(;;) {
		int res=0,i;
		begindialog((maxscreen+4),thecols,4);
			int starty=1,startx=2;
			for(i=startlin;i<(minnum((maxscreen+startlin),helpitem->nr));i++) {
				mvwaddstr(takescr,starty+i-startlin,startx,helpitem->text+(i?ind[i-1]:0));
				}
			  oke((maxscreen+2),2);
			  if(i<helpitem->nr)
				saveact("~Next", maxscreen+2,10);
			  if(startlin)
				saveact("~Prev", maxscreen+2,16);
		  enddialog(res,res);
		  	
			if(res==INT_MAX)  {
				edits[editfocus]->menuresized();
				wrefresh(edits[editfocus]->editscreen);
				continue;
				}
			if(res==2&&i<helpitem->nr) {
				startlin=i;
				continue;
				}
			if(res>=2&&startlin>0) {
				startlin=maxnum(startlin-maxscreen,0);
				continue;
				}
			break;
			};
			return 0;
		}
	else {
		edits[editfocus]->message("No help");
		}
	return -4;
	}
extern int lasthelp(void) ;
int lasthelp(void) {
	 showhelp(helpproc);
	 edits[editfocus]->editup();
	 return 0;
	}

int makehelp(char *text,int len,int  (Editor::*proc)(void)) {
	if(!helphash)
		inithelphash();	
	int innr=0;	
	short inds[len];
	for(int i=0;i<len;i++) {	
		if(text[i]=='\n') {
			inds[innr++]=i+1;
			text[i]='\0';
			}
		}
	struct 	help *item=myalloc( struct help);
	item->nr=innr+1;
	item->ind=myallocar(short,innr);
	memcpy(item->ind,inds,innr*sizeof(short));
	item->text=text;
	item->proc=proc;
	addhelp(item);
	return 0;
	}



