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

#include "desturbe.h"
#include <features.h>
#include <ctype.h>
#include "screen.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "system.h"
#include "main.h"
#include "readconfig.h"
#ifdef USETHREADS
#define __GNU_VISIBLE 1
#include <pthread.h>
#endif
#include "dialog.h"


int waitinput=0;
int resizeflag=0;



class Editor;
#include "trace.h"
int maxedit=10,nredit=0, editfocus=-1,startnonactive=0;
int	startactive=0;
Editor **edits;


#include "editor.h"
#include "inout.h"

int Editor::nop(void) {
	return 0;
	};
int Editor::getmode(DataType type,int len,int order) {
	if(type&&(tomode(type,len,order)<0))
		return addbefore(type,mode+1,len,order);
	return 0;
	}
int Editor::discard(void) {
	INCDEST();
	mem.empty();
	modified=0;
	updated=1;
	DECDEST();
	return 0;
	}
int Editor::tocommand(void) {
        commandmode=1;
        return 0;
        }
int Editor::fromcommand(void) {
        commandmode=0;
        return 0;
        }
int Editor::switchcommand(void) {
        commandmode=!commandmode;
        return 0;
        }


// Split window operaties 
int Editor::switchmessage(void) {
	nomessage=!nomessage;
	shouldreinit=1;
	return 0;
	}

void Editor::windel(void) {
	if(editscreen)
		delwin(editscreen);
	editscreen=NULL;
	}







void Editor::editup(void) {
	startnonactive=minnum(startnonactive,nredit);
	startactive=maxnum(startactive,0);
	for(int i=startactive;i<startnonactive;i++) {
		touchwin(edits[i]->editscreen);
		wrefresh(edits[i]->editscreen);
		}
	}

	int Editor::emptyfile(void) {
		if(::emptyfile())
			return 0;
		return -1;
		}

 int Editor::getch(void) {
	int ch;
	waitinput=1;
	if((mysigsetjmp(resizejmp,1))) {

		waitinput=0;
		return RESIZE;
		};
	SHOWCURSOR;
	wrefresh(editscreen);
	ch=mywgetch(editscreen);
	HIDECURSOR;
	waitinput=0;
	return ch;
	};
int Editor::resetnumbase(void) {
				int oldnum=maxnumstr;
				putnumbase(numbase);
				if(oldnum!=maxnumstr)
					shouldreinit=1;
			return shouldreinit;
			}
#ifdef TRACEON
#include "trace.cpp"
#endif
void Editor::erefresh(void) {
	wrefresh(editscreen);
	}
	OFFTYPE Editor::filesize(void) {return mem.size();}






/*
long Editor::digfrom(char *ant) {
	long base,add;
	char *ptr;

	while(isspace(*ant))
		ant++;
	switch(ant[0]) {
		case '+': {
			add=filepos+editpos;
			ant++;
			break;
			}
		case '|': add=filepos;ant++;break;
		default: add=0;
		}
	if(ant[0]=='0') {
		if(ant[1]=='x') {
			ant+=2;
			base=16;
			}
		else {
			if(ant[1]=='\0')
				return 0;
			ant++;
			base=8;
			}
		}
	else
		base=10;
	return(strtol(ant,&ptr,base)+add);
	}
*/

//((x>47&&x<58)?x-'0':((x>0x40&&x<0x47)?(x-55):((x>0x60&&x<0x67)?(x-87):30)))

/* INFO */

/*name, size, editpos */
/* stat gegs */
/*  wat is veranderd, gegevens undo?
* Geselecteerd gedeelte */

#include <sys/time.h>
#define timerdiff(een, twee) (((een).tv_sec - (twee).tv_sec)*1000000+ (een).tv_usec - (twee).tv_usec)







inline int mknumstr(int tmode,OFFTYPE num,char *buf,int buflen=MAXANT) {
	switch(tmode) {
		case 10: snprintf(buf,buflen,"%" OFFPRINTONLY "u",num);break;
		case 16: snprintf(buf,buflen,"%" OFFPRINTONLY "x",num);break;
		case 8: snprintf(buf,buflen,"%" OFFPRINTONLY "o",num);break;
		default:  snprintf(buf,buflen,"%" OFFPRINTONLY "u",num);break;
		};
	return tmode;
	}
int Editor::gettopos(void) {
	static bool forward=1,edge=1,bound;
	int tmode=numbase;
	char ant[50],*ptr;
	int wlin=13,wcol=40;
	int res=0;
	OFFTYPE	 pos=lastgotopos;
again:
	mknumstr(tmode,pos,ant,50);	
/*
        Common combase(tmode),*com=&combase;
	palcsconfig(res,wlin,wcol,3,com->last=(orcheck*)showformsarray[typevolg(tmode)+5],
		checker( "For~ward",2,3,forward), 
		checker( "~Edge file",2,20,edge), 
		checker( "~Align",4,3,bound), 
		linernrmax("~Position: ",ant,6,3,20,50),
		oke(8,3),
		act("~Cancel", 10,3),
		ore("~Hex",9,15,16,com),
		ore("~Dec",10,15,10,com),
		ore("Oc~t",9,27,8,com),
		ore("~Spec",10,27,0,com)
		); 
*/
int optbase;
switch(numbase) {
	case 16: optbase=0;break;
	case 10: optbase=1;break;
	case 8: optbase=2;break;
	default: optbase=3;break;
	};
int startpos;
	initdialog(13); //10
		checker( "For~ward",2,3,forward); 
		checker( "~Edge file",2,20,edge); 
if(activemode()->bytes>1)
		checker( "~Align",4,3,bound) ;

startpos=showformsiter;
		linernrmax("~Position: ",ant,6,3,20,50);
		oke(8,3);
		act("~Cancel", 10,3);
	optselect(tmode,optbase, opt("~Hex",9,15,16), opt("~Dec",10,15,10), opt("Oc~t",9,27,8), opt("~Spec",10,27,0)); 
	dodialog(res,wlin,wcol,startpos);


	if(!res||res==6) { 
		editup(); 
		return -1; 
		}	
	else {
			if(res==INT_MAX)  {
				menuresized();
				wrefresh(editscreen);
				goto again;
				}
		if((res>6)&&(res<11)) {
			pos=STRTOO(ant,&ptr,tmode);
			 switch((res-7)) {
				case 0:tmode=16;break;
				case 1:tmode=10;break;
				case 2:tmode=8;break;
				case 3:tmode=0;break;
				};
			 goto again;
			 }
		}
	pos=STRTOO(ant,&ptr,tmode);
	editup();
	if(ptr==ant) {
		message("No digit");
		return -1;
		}
	lastgotopos=pos;
	OFFTYPE cur=filepos+editpos;
	pos=  (edge?((!forward)*filesize()):cur)  + (2*forward -1)*(pos);
	if(pos<0)
		pos=0;
	else {
		if(pos>=filesize()) {
			beep();
			pos=filesize()-1;
			}
		}

	if(bound) 
		return topos(pos);
	else {
		return toborderpos(pos) ;
		}
	}

















//int Editor::zoomascii(void) {



//struct Editor::tree *tree =(struct Editor::tree*) {1,27,{NULL}, NULL };




int Editor::repeat(void) {
	int ret=-1;
	char ans[MAXANT],*ptr;
	ans[0]='2';
	ans[1]='\0';
	if((getanswer("How often? ",ans,MAXANT) >=0)&&ans[0]) {
		int nr=strtol(ans,&ptr,10);
		if(ptr!=ans) {
			if(nr>0) {
				output("Type any number of keys (end with M-.) ");
				ret=therepeat(nr);
				}
			else output("Not a positive integer");
			}
		else
			output("No digit");
		}
	return ret;
	}
#define cat(x,y) x ## y


/*
	static  int (Editor::*proc[])(void);
	static  char *procnames[];
*/








int Editor::place(int from) {
	fromtop=from;
	int count=(startnonactive-startactive);
	int leftheight=MainScreenheight-menuon;
	allheight=edittop=leftheight/count;
	if(fromtop==(count-1))
		allheight+=(leftheight%count);
	Screenwidth=MainScreenwidth;
	edittop=(fromtop*edittop)+menuon;
	INCDEST();
	if(editscreen) {
		delwin(editscreen);
		}
	editscreen=newwin(allheight,Screenwidth,edittop,0); 
	DECDEST();
	if(!editscreen) {
		message("newwin(%d,%d,%d,0) failed",allheight,Screenwidth,edittop);
		return -1;
		}
	return reinit();
	}
int Editor::askbase(void) {
	int ret=0;
	updated=0;
	switch(getkeyinput("Display offset in ((d)ec/(h)ex/(o)ct/(n)one)? ")) {
		case 'd': ret = putnumbase(10);break;
		case 'h': ret = putnumbase(16);break;
		case 'o': ret = putnumbase(8);break;
		case 'n': ret = putnumbase(0);break;
		case 'D': ret = putnumbase(10);break;
		case 'H': ret = putnumbase(16);break;
		case 'O': ret = putnumbase(8);break;
		case 'N': ret = putnumbase(0);break;
		};
	shouldreinit=1;
	return ret;
	};
int Editor::putnumbase(int base) {
	char ch;
	numbase=base;
	switch(base) {
		case 16: ch='x';break;
		case 8: ch='o'; break;
		case 10: ch='u';break;
		case 0: numbase=10;ch='u'; break;
		default: beep();return -1;
		};
//	maxnumstr=(int)ceil(log(mem.size()+88888)/log(numbase));
//	maxnumstr=(int)ceil(log((long double)mem.size()+numbase+30000)/log(numbase));
		
	snprintf(numstr,MAXNUMSTR,"%s" OFFPRINTONLY "%c","%0",ch);
	char testbuf[NUMDIGETS_USED+1];
	maxnumstr=snprintf(testbuf,NUMDIGETS_USED,numstr,mem.size()+numbase+30000);

//	return (((int)ceil(log(pow((double)2.0,(double)8.0*len)-1)/log((int)b))));
	snprintf(numstr,MAXNUMSTR,"%s%d" OFFPRINTONLY "%c","%0",maxnumstr,ch);
//	snprintf(numstr,100,"%s%d" OFFPRINTONLY "%c","%0",maxnumstr,ch);
	if(!base)
		numbase=0;
	return numbase;
	}

#include <math.h>

//extern int visualoffset;

int Editor::screensize(void) { return screenmax();};
int Editor::fileok(void) {
/*
	STATTYPE st;
	memorize();
	if(tmpbuffer(mem.filename))
		return 0;
	if(STAT(mem.filename,&st)<0) {
		saveinode();	
		return -1;
		}
		
	if(st.st_ino!=mem.statdata.st_ino) {
		saveinode();	
		return 1;
		}
	if((st.st_mtime!=mem.statdata.st_mtime)||(st.st_size!=mem.statdata.st_size)) {
		if(!modified)
			return 20;
		int i;
		for(i=startactive;i<startnonactive;i++) 
			if(this==edits[i])
				break;
		if(i==startnonactive) {
			startactive=editfocus;
			startnonactive=startactive+1;
			resizeflag=0;
			redistribute=0;
			place(0);
			}
		wrefresh(editscreen);
		output("WARNING: file saved by other process. Saving changes will probably destroy data.");
		}
*/
	return 0;
	}
extern int (Editor::*nameproc(const char *str))(void) ;
int Editor::proconname(char *str) {
	int (Editor::*proc)(void);	
	proc=nameproc("example1");
	if(proc)
		return callproc(proc);
	return -77;
	}

int Editor::modechanged(void) {
	shouldreinit=1;
	return 0;
	}
BLOCKTYPE Editor::blocksize(void) { return mem.blocksize();};
