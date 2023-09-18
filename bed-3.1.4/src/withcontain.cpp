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
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_SYS_MMAN_H 
#include <sys/mman.h>
#endif
#include "editor.h"
#include "contain.h"
#include "datatypes.h"
#include "dialog.h"
#include "screen.h"
#include "system.h"
extern char showfileinfo;
char showfileinfo=1;
char emptyshowdefault[2]="#";
extern void                     mychdir(const char *path);
extern char	*mygetcwd(char *name,int len) ;
extern char datatypedirectory[]	;

#include "repeat.h"


class  Empty : public ScreenPart {	
	char *show;
	public:	 Empty ( Editor *ed,int b=4,int x1=0, int y1=0): ScreenPart( ed, 1, 10 ,b,x1, y1,0)	{
	spaceafter=1;
//	name=	"Empty";
	show=new char[2];
	memcpy(show,emptyshowdefault,2);
	};	

	declareparent 
	 Empty ( Empty const &rep):ScreenPart(rep) {
		int len=strlen(rep.show);
		show=new char[len+1];
		memcpy(show,rep.show,len+1);
	 	}
	int formal2bin(const char *formal, unsigned char *bin, int len) const override{	
		return 0;	
		}
	ScreenPart *newone(void) const override{return new Empty(*this);}
	int bin2formal(unsigned char *bin, char *formal, int len) override{	
		memmove(formal,show,maxres);
		return 0;	
		}	
	int isel(const int ch) const override{return 0;}
	int chbytes(int b) override{
		if(b==-1)
			return datbytes();
		if(b>0) {
			datbytes(b);
			return b;	
			}
		else
			return -1;
		}

	void mklabel(void) override{
		if(!userlabel)
			snprintf(label,maxlabel,"skip %d bytes",bytes);
		}
	char *getconvstr(void) override{
		return show;
		}
	int setconvstr(char *str) override{
		maxres=strlen(str);
		delete[] show;
		show=new char[maxres+1];
		memcpy(show,str,maxres+1);
		return 0;
		}
	virtual int userconfig(int kind) override{
		return editor->emptyconfig(this,kind);
		}
	};

parentd(ScreenPart,Empty) 

#include "typelist.h"
parentdef(HasSub,Contains,"Composed")  /*Gives Contains a name */

datatype(Ascii)
datatypename(Contains,Contains::thename)
datatype(Digit)
datatype(Empty)

/*
Contain
	Repeat
		basic
	basic

*/
int Editor::downlevel(void) {
	if(isleaf(parts[mode]))  {
		message("No composed datatype");
		return -1;
		}
	ScreenPart *sub=parts[mode]->getsubpart();
	if(isleaf(sub))  {
		beep();
		parts[mode]->mklabel();
		showfileinfo=1;
		return -1;
		}
	HasSub *has=(HasSub *)sub;
	has->poschanged(has->half);
	if(!(sub=sub->getchild())) {
		message("System Error: no child");erefresh();beep();return -1;
		}
	if(!isleaf(sub)) {
		has=(HasSub *)sub;
		has->active=-1;
		}
	parts[mode]->mklabel();
	showfileinfo=1;
	return 0;
	}
int Editor::uplevel(void) {
	if(isleaf(parts[mode]))  {
		message("No composed datatype");
		return -1;
		}
	ScreenPart *par=parts[mode],*sub,*grand=NULL;
	int forstart=0;
	int binstart=0;
	int f,b;
	int pos=par->half;
	while(( sub=par->getchild(f,b,pos-forstart))) {
		grand=par;
		par=sub;
		forstart+=f;
		binstart+=b;
		}
	if(par==parts[mode]) {
		beep();
		parts[mode]->mklabel();
		showfileinfo=1;
		return 0;
		}
	else {
		if(!grand) {
			beep();message("System error: no grandparent");return -1;
			}
		if(isleaf(grand)) {
			beep();message("System error: leaf grandparent??");return -1;
			}
		HasSub *has=(HasSub *)grand;
		has->active=-1;
		has->mklabel();
		}
	parts[mode]->mklabel();
	showfileinfo=1;
	return 0;
	}
	
std::vector<ScreenPart *> Editor::progenitors(void) {
	std::vector<ScreenPart *> pro;
	if(isleaf(parts[mode]))  {
		pro.push_back(parts[mode]);
		return pro;
		}
	int forstart=0;
	int binstart=0;
	int f,b;
	ScreenPart *par=parts[mode];
	int pos=par->half;
	for(ScreenPart *sub; (sub=par->getchild(f,b,pos-forstart));par=sub) {
		pro.push_back(par);
		forstart+=f;
		binstart+=b;
		}
	pro.push_back(par);
	return pro;
	}
int Editor::stretchcontain(void) {
	ScreenPart *sub=parts[mode]->getsubpart();
	HasSub *par=(HasSub *)sub->getparent();
	if(!par) {
		message("No parent found");beep();return -1;
		}
	if(!has_super(par,Contains)) {
		message("Parent not composed");beep();return -1;
		}
	int acti=par->active;
	if(acti<1) {
		message("No left sibling");beep();return -1;
		}
	ScreenPart *sib=par->subpart(acti-1);
	if(!has_super(sib,Contains)) {
		message("Left sibling isn't a composed");beep();return -1;
		}
	Contains *con=(Contains *)sib;	
	par->freesubpart(acti);	
	con->addsubpart(sub);
	if(parts[mode]->calcsizes()<=0) {
		con->freesubpart();
		((Contains *)par)->addsubpart(sub,acti);
		return parts[mode]->calcsizes();
		}
	modechanged();
	message("added to left composed");
	return 0;
	}
	
int Editor::levelout(void) {
	ScreenPart *sub=parts[mode]->getsubpart();
	if(!has_super(sub,HasSub)) {
		beep();
		message("Can only levelout modes with children");return -1;
		}
	HasSub *has=(HasSub *)sub,*par;
	par=(HasSub *)sub->contain;
	if(!par) {
		if(has->nrsubparts>1) {
			message("Too many members" );beep();return -1;
			}
		ScreenPart *part=has->subpart(0);
		if(has_super(part,Repeat)) {
			message("Repeat child should be leveled out first");beep();return -1;
			}
		message("Leveled out %s",has->getname());
		parts[mode]=part;
		has->freesubpart();
		parts[mode]->contain=NULL;
		delete has;
		parts[mode]->calcsizes();
		modechanged();
		return 0;
		}
	if(has_super(has,Repeat)) {
		if(has_super(par,Repeat)) {
			par->setsubpart(has->subpart(0));
			par->nrsubparts*=has->nrsubparts;
			message("Leveled out %s",has->getname());
			if(parts[mode]->calcsizes()<=0) {
				par->nrsubparts/=has->nrsubparts;
				has->setsubpart(par->subpart(0));
				par->setsubpart(has);
				message("Leaving out failed");
				return parts[mode]->calcsizes();
				}
			has->freesubpart();
			delete has;
			modechanged();
			return 0;
			}
		par->setsubpart(has->subpart(0));
		if(parts[mode]->calcsizes()<=0) {
			has->setsubpart(par->subpart(0));
			par->setsubpart(has);
			message("Leaving out failed");
			return parts[mode]->calcsizes();
			}
		message("Leveled out %s",has->getname());
		has->freesubpart();
		delete has;
		modechanged();
		return 0;
		}
	if(has_super(par,Repeat)) {
		if(has->nrsubparts==1) {
			par->setsubpart(has->subpart(0));
			if(parts[mode]->calcsizes()<=0) {
				par->setsubpart(has);
				message("Leaving out failed");beep();
				return parts[mode]->calcsizes();
				}
			message("Leveled out %s",has->getname());
			has->freesubpart();
			modechanged();
			delete has;
			return 0;
			}
		message("Can't put more then one not without composed in repeat");
		beep();return -1;
		}
	if(has_super(par,Contains)) {
		Contains *con=(Contains *)par;
		con->setsubpart(has->subpart(0));
		int act=con->active;
		int pos=act+1;
		for(int i=1;i<has->nrsubparts;i++) 
			con->addsubpart(has->subpart(i),pos++); 
		if(parts[mode]->calcsizes()<=0) {
			for(int i=1;i<has->nrsubparts;i++) {
				con->rmsubpart(act+1);
				}
			con->setsubpart(has);
			message("Failt");
			return parts[mode]->calcsizes();
			}
		message("Leveled out %s",has->getname());
		has->nrsubparts=0;
		delete has;
		modechanged();
		return 0;
		}
	message("Don't know what to do with %s",par->getname());beep();return -1;
	}
/*
Changes screenbytes to b;

*/
ScreenPart *rmmode(ScreenPart *part) {
	ScreenPart *sub,*back;
	if(!isleaf(part)&&((HasSub *)part)->issubindex()) {
		HasSub *has=(HasSub *)part;
		int act=has->activesubpart();
		sub=has->subpart(act);
		if(!(back=rmmode(sub))	) {
			if(iscontain(has)) {
				Contains *con=(Contains *)has;
				if(!(back=con->rmsubpart(act))) {
					return NULL; /*remove part */
					}
				if(back!=con)
					return back; /*remove part and put back */
				con->calcsizes();
				return con; /*finished */
				}
			else {
				return NULL;
				}
			}
		if(back!=sub) {
			has->setsubpart(back,act);
			delete sub;
			}
		has->calcsizes();
		return has;
		}
	else {
		return NULL;
		}
	}
int canresize(ScreenPart *sub,int negbyt) {
	int byts=-negbyt;
	while((sub=sub->getparent())) {
		int dby=sub->datbytes();
		int fby=sub->getbytes();
		int by;
		if((by=sub->screenupdatefilters(dby-byts))<1) {
			sub->calcsizes();
			return -1;
			}
		byts=fby-by;
		}
	return 1;
	}
int Editor::removemode(void) {

	OFFTYPE unitpos=(editpos/parts[mode]->bytes)*parts[mode]->bytes+filepos;
	ScreenPart *sub=parts[mode]->getsubpart();
	int byts=sub->getbytes();
	if(canresize(sub,-byts)<0) {
		message("Filter is in the way");
		return -1;
		}
	ScreenPart *back=rmmode(parts[mode]);
	shouldreinit=1;
	if(back==parts[mode]) {
		toborderpos(unitpos) ;
		return 0;
		}
	if(!back) {
		delete parts[mode];
		nrparts--;
		if(nrparts<=0) {
			mode=0;nrparts=1;
			parts[mode]=new Ascii(this);
			message("Removed last datatype, so to ascii");
			beep();
			return -1;
			}
		for(int i=mode;i<(nrparts);i++)
			parts[i]=parts[i+1];
		if(mode>=nrparts)
			mode--;
		toborderpos(unitpos) ;
		return 0;
		}
	if(back!=parts[mode]) {
		parts[mode]=back;
		toborderpos(unitpos) ;
		return 0;
		}
	else {
		message("System Error: removemode");
		beep();
		return -1;
		}
	return 0;
	}
/* Toevoegen aan contain:                                                    */
/* contains                                                                  */
/* 	contains                                                                 */
/* 		el                                                                      */
/* 		el                                                                      */
/* 		contains                                                                */
/* 	el                                                                       */
/* Neem altijd de meest specifieke contain als er een is                     */
/* Tue Oct 31 20:01:54 2000                                                  */

int Editor::newcontain(void) {
	ScreenPart *current=parts[mode]->getsubpart();
	ScreenPart *par=current->getparent(parts[mode]);
	Contains *con= new Contains(this);
	if(con->addsubpart(current)<0) {
		message("Creating composed around %s failed",current->getname());
		delete con;
		return -1;
		}
	if(par) {
		((HasSub *)par)->setsubpart(con);
		}
	else {
		parts[mode]=con;
		}
	shouldreinit=1;
	message("Made composed around %s",current->getname());
	return parts[mode]->calcsizes();
	}
int Editor::addtocontain(ScreenPart *part) {
	Contains *con;
	HasSub *par=NULL;
	int take;
	ScreenPart *current=parts[mode];
/*
 *tmp;
	int fors,bins;
	int pos=current->half;
	while((tmp=current->getchild(fors,bins,pos))) {
		par=(HasSub *)current;	
		current=tmp;
		pos-=fors;
		}
*/
	current=current->getsubpart(); 
	par=(HasSub *)current->getparent();
	while(par&&has_super(par,Repeat)) {
		current=par;
		par=(HasSub *)par->getparent();
		}

	if(!par&&!has_super(current,Contains)) {
		con= new Contains(this);
		if(current->maxres==1&&!(editpos%nrx))
			take=0;
		else {
			if(current->half>=((current->maxres)/2)) 
				take=1;
			else
				take=0;
			}
		if(con->addsubpart((ScreenPart *) current)<0) {
			while(con->freesubpart())
				;
			delete con;
			return -1;
			}
		if(con->addsubpart((ScreenPart *) part,take)>=0) {
			if(current==parts[mode]) {
				parts[mode]=con;
				}
			else {
				HasSub *par=(HasSub *)current->getparent();
				if(!par) {
					}
				int activ=par->active;
				par->setsubpart(con,activ);
				if(((HasSub *)parts[mode])->calcsizes()<0) {
					par->setsubpart(current,activ);
					while(con->freesubpart())
						;
					delete con;
					return -1;
					}
				}
			shouldreinit=1;
			editup();
			return 0;
			}
		while(con->freesubpart())
			;
		delete con;
		return -1;
		}
	else { 
		if(par&&has_super(par,Contains)) {
			con=(Contains *)par;
//			if((con->half-fors)>=((current->maxres)/2)) 
			if((con->half-current->half)>=((current->maxres)/2)) 
					take=1;
			else
				take=0;
			if(con->active>=0)
				take+=con->active;
			else
				take+=con->nrsubparts-1;
			if(par&&current->maxres==1&&!con->active&&!(editpos%nrx))
				take=0;
			}
		else {
			if(has_super(current,Contains)) {
				con=(Contains *)current;

				int tot=0,end=con->half;
				for(take=0;take<con->nrsubparts&&tot<=end;take++) {
					ScreenPart *sub=con->subpart(take);
					tot+=con->spacebetween(sub)+sub->maxres;
					}
				}
			else {
				message("System error: inconsistent addtocontain");return -1;
				}
			}
		if(con->addsubpart((ScreenPart *) part,take)<0) {
			return -1;
			}
		if(parts[mode]->calcsizes()<=0) {
			con->freesubpart(take);
			parts[mode]->calcsizes();
			return -1;
			}
		if(getcols()) {
			shouldreinit=1;
			editup();
			return 0;
			}
		con->freesubpart(take);
		return  -1;
		}
	}

int Editor::emptycontain(void) {
	Empty *emp= new Empty(this,1);
	if(emptyconfig(emp,0)<0) {
		delete emp;
		return -1;
		}
	return 0;
	}
int Editor::emptyconfig(Empty *emp,int kind) {
	int maxshowas=80;
	#define maxbytes 20
	int separ=0,last;
	char bytesstr[maxbytes];
	int wlin=10, wcol=25;
	int byt=emp->datbytes();
	sprintf(bytesstr,"%d",byt);	
	char showas[maxshowas];
	strcpy(showas,emp->getconvstr());
	while(1) {
		int res=0;
		last=4;
  begindialog(wlin,wcol,10);
    if(!kind) {
      		optselect(separ,separ, 
			opt("~Add field",1,3,1),
			opt("~Separate",2,3,0));
		last+=2;
      		}
			linernrmax("S~how as: ",showas,4,3,3,maxshowas);
			linernrmax("~Nr bytes: ",bytesstr,6,3,3,maxbytes);
			oke(8,3);
			act("~Cancel", 8,15);
  enddialog(res,last-3);



		if(!res||res==last)	{
			editup();
			return 0;
			};
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		
		char *endpo;
		int len=strtol(bytesstr,&endpo,10);
		if((endpo==bytesstr)||len<=0) {
			message("No lenght given");
			continue;
			}
		emp->chbytes(len);
		if(emp->setconvstr(showas)<0) {
			message("Cant set %s",showas);
			delete emp;
			continue;
			}
		if(!kind) {
			if(installpart(emp,separ)<0) {
				message("Failed");
				delete emp;
				return -1;
				}
			}
		return len;
		}
	}

int Editor::repeatcontain(void) {
	const int anslen=20;
	int byt,ret;
	char ans[anslen];
	ScreenPart *sub=parts[mode],*par=NULL,*tmppart;
	while((tmppart=sub->getchild())) {
		par=sub;
		sub=tmppart;
		}

	OFFTYPE unitpos=(editpos/sub->bytes)*sub->bytes+filepos;
	if(isrepeat(sub))
		par=sub;
	if(par&&isrepeat(par)) {
		HasSub *con=(HasSub *) par;
		byt=con->chrepeat(-1);
		if(byt<1)
			byt=1;
		}
	else
		byt=1;
	snprintf(ans,anslen,"%d",byt);
	do {
		if(getanswer("Nr ~Repeat",ans,anslen)<0) {
			beep();
			return -1;
			}
		byt=atoi(ans);
		} while(byt<=0);

	if(par&&isrepeat(par)) {
		HasSub *con=(HasSub *) par;
		shouldreinit=1;
		int old=con->nrsubparts;
		ret= con->chrepeat(byt) ;
		if(parts[mode]->calcsizes()<=0) {
			con->nrsubparts=old;
			parts[mode]->calcsizes();
			beep();
			toscreen();
			message("Filter or so in the way");
			return -1;
			}
		toscreen();
		toborderpos(unitpos);
		return ret;
		}
	Contains *con;
	if(par&&has_super(par,Contains)) {
		con=(Contains *)par;
		
		ret= con->chrepeat(byt);
		if(parts[mode]->calcsizes()<=0) {
			ScreenPart *part=con->setsubpart(sub);
			if(isrepeat(part)) {
				((Repeat *)part)->freesubpart();
				delete part;
				}
			parts[mode]->calcsizes();
			beep();
			toscreen();
			message("Filter or so in the way");
			return -1;
			}
		}
	else {
		con= new Contains(this);
		if(con->addsubpart((ScreenPart *) sub)<0) {
			delete con;
			shouldreinit=1;
			parts[mode]->calcsizes();
			beep();
			toscreen();
			message("Filter or so in the way");
			return -1;
			}
		parts[mode]=con;	
		ret= con->chrepeat(byt);
		parts[mode]->calcsizes();
		}
	shouldreinit=1;
	toscreen();
	toborderpos(unitpos);
	return ret;
	}
#include "main.h"



#ifdef _WIN32
#include "winpad.h"
char datatypedirectory[BEDSMAXPATH]=CONFDIRWIN FILENAMESEP "misc";	
#else
char datatypedirectory[BEDSMAXPATH]=CONFDIR "/misc";	
#endif

extern const char datatypefile[];
const char datatypefile[]="bed-datatype:";
/*
#ifdef _WIN32
const char datatypefile[]="bed-datatype:\015\012";
#else
const char datatypefile[]="bed-datatype:\n";
#endif
*/
const int datatypefilelen=sizeof(datatypefile)-1;

	const char SAVEDTYPE[]="*.bedda";
FILEHANDLE Editor::getsavedatahandler(const char *question) {
	char cwdold[BEDSMAXPATH];
	char buf[BEDSMAXPATH];
	FILEHANDLE handle;
	memcpy(buf,SAVEDTYPE,sizeof(SAVEDTYPE));
	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(datatypedirectory);
	const char *ant;
	do {
		ant=getfile(question,buf) ;
		editup();
		if(!ant) {
			mygetcwd(datatypedirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return MY_INVALID_HANDLE_VALUE;
			}
		} while((handle=openfile(ant))==MY_INVALID_HANDLE_VALUE);
	mygetcwd(datatypedirectory,BEDSMAXPATH);
	mychdir(cwdold);
	return handle;
	}
void beddatatypestart(FILEHANDLE handle) {
#ifdef _WIN32
	char line[2]={0xd,0xa};
#else
	char line[1]={0xa};
#endif

	writetofile(handle,datatypefile,datatypefilelen);
	writetofile(handle,line,sizeof(line));
}

int Editor::savecontain(void) {
	FILEHANDLE handle;
	extern int savedatatype(ScreenPart *part,FILEHANDLE handle) ;
	if((handle=getsavedatahandler("Save datatype"))==MY_INVALID_HANDLE_VALUE) {
		message("Cancelled");
		return -1;
		}
	beddatatypestart( handle) ;
	savedatatype(parts[mode],handle); 
	closehandle(handle);
	message("Saved datatype info");
	return 0;
	}

int Editor::savealldatatypes(void) {
	FILEHANDLE handle;
	extern int savedatatype(ScreenPart *part,FILEHANDLE handle) ;
	if((handle=getsavedatahandler("Save all datatypes"))==MY_INVALID_HANDLE_VALUE){
		message("Cancelled");
		return -1;
		}
	beddatatypestart( handle) ;
	for(int i=0;i<nrparts;i++)
		savedatatype(parts[i],handle); 
	closehandle(handle);
	message("Saved all datatypes");
	return 0;
	}

int Editor::savedatatype(void) {
	if(nrparts>1) {
		switch(getkeyinput("Save (o)ne datatype or (a)ll datatypes? ")) {
			case 27:;
			case 'c':;
			case 'C':message("Cancelled");return -1;

			case 'o':;
			case 'O':return savecontain();
			}
		}
	return savealldatatypes() ;
	}



#ifdef HAVE_SYS_MMAN_H 
#define endmmap {if(nommap) myfree(gegs);else munmap(gegs, endf+1); close(handle);}
#else
#define endmmap {myfree(gegs); close(handle);}
#endif
			//	message("error reading filters %d",ret);
/*
Float 10 4
*/
#define errormessage message
#define filtersfrom(x,y) if((ret=::readfiltersfrom(x,y,end,mes))<0) {\
			return -1;\
			}
extern void skipsep(char *&ptr,size_t  &len) ;
extern int getuserlabel(ScreenPart *part, char *&ptr) ;
extern int readfiltersfrom(ScreenPart *part,char *&ptr,char *end,char *mes) ;
extern ScreenPart *str2datatype(Editor *edit,char *&ptr, char *end,char *mes) ;







int Editor::loaddatatype(char *gegs,int endf,int addfield) {
	char mes[256]="Error";
	char * end= gegs+endf;
	char *ptr=gegs;
	size_t len=0;
#ifdef READ0x2x4DATATYPE
	int oldbedda=0,ret;
	Contains *con= new Contains(this);
#else
	int maxnewparts=0,nrnewparts=0;
	ScreenPart **newparts=NULL;
#endif
	while(ptr<end) {	
#ifdef READ0x2x4DATATYPE
		if(ptr[0]=='\"') {
			oldbedda=1;
			if(getuserlabel(con,ptr)<0) {
				errormessage("Wrong userlabel for contain");
				delete con;
				return  -1;
				}
			skipsep(ptr,len);
			if(*ptr!=';') {
				errormessage("Error after userlabel");
				delete con;
				return  -1;
				}
			ptr++;
			skipsep(ptr,len);
			}	
		else {
		if(ptr[0]=='[') {
			oldbedda=1;
			filtersfrom(con,ptr);
			skipsep(ptr,len);
			if(*ptr!=';') {
				errormessage("Error after contain filters");
				delete con;
				return  -1;
				}
			ptr++;
			skipsep(ptr,len);
			}
			else	 {
#endif
				ScreenPart *part=str2datatype(this,ptr,end,mes);
				if(part)  {
#ifdef READ0x2x4DATATYPE
			if(*ptr=='*') {
				ptr++;
				skipsep(ptr,len);
				len=strcspn(ptr, "	 \012\015,"); 
				char *endptr;
				int count=strtol(ptr, &endptr, 10); 
				ptr+=len;		
				skipsep(ptr,len);
				if(*ptr!=',') {
					delete part;
					delete con;
					errormessage("missing ,");
					return -1;
					}
				ptr++;
				Repeat *rep= new Repeat(this,part,count);
				if(::readfiltersfrom(rep,ptr,end,mes)<0) {
					delete rep;
					delete con;
					return -1;
					}
				skipsep(ptr,len);
				if(*ptr==',') {
					ptr++;
					skipsep(ptr,len);
					if(getuserlabel(rep,ptr)<0) {
						delete rep;
						delete con;
						errormessage("Wrong label format");
						return -1;
						}
					skipsep(ptr,len);
					}
				part=rep;
				}
					con->addsubpart((ScreenPart *) part);
#else
				if(nrnewparts==maxnewparts) {
					maxnewparts=maxnewparts*2+5;
					newparts=myrealloc(ScreenPart **,newparts,ScreenPart *, maxnewparts*2,nrnewparts) ;

					}
				newparts[nrnewparts++]=part;

//					installpart(part,addfield);	
#endif
					}
				else {
					message(mes);
					return -1;
					}
				if(ptr[0]!=';') 
					message("Missing ;");
				else
					ptr++;
#ifdef READ0x2x4DATATYPE
				}
			}
#endif
		skipsep(ptr,len);
		};
#ifdef READ0x2x4DATATYPE
	if(oldbedda&&(con->nrsubparts>1||isrepeat(con->subparts[0])||con->getlastfilter())) {
		installpart(con,addfield);
		}
	else {
//		for(int i=(con->nrsubparts-1);i>=0;i--) {
		const int nrsubs=con->nrsubparts-1;
		for(int i=0;i<con->nrsubparts;i++) {
			ScreenPart *sub;
			if(addfield)
				sub=con->subpart(nrsubs-i);
			else
				sub=con->subpart(i);
			sub->contain=NULL;
			installpart(sub,addfield);	
			}
		con->nrsubparts=0;
		delete con;
		}
#else
	if(addfield) {
		for(int i=(nrnewparts-1);i>=0;i--)
				installpart(newparts[i],addfield);	
		}
	else {
		for(int i=0;i<nrnewparts;i++)
				installpart(newparts[i],addfield);	
		}
	myfree(newparts);	
#endif
	return 0;
	}













int Editor::readcontain(void) {
	char buf[BEDSMAXPATH];
	char cwdold[BEDSMAXPATH];
	const char *ant;
	int handle;
	memcpy(buf,SAVEDTYPE,sizeof(SAVEDTYPE));
	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(datatypedirectory);
	do {
		ant=getfile("Read Datatype",buf) ;
		editup();
		if(!ant) {
			mygetcwd(datatypedirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return -1;
			}
		char name[BEDSMAXPATH];
		if(!arealpath(ant,name)) {
			message("realpath %s failed",ant);
			wrefresh(editscreen);
			}
		else {
			if((handle=open(name,O_RDONLY BINARY_FLAG))<0) {
				message("Can't open %s: %s",name,strerror(errno));
				wrefresh(editscreen);
				}
			}
		} while(handle<0);
	mygetcwd(datatypedirectory,BEDSMAXPATH);
	mychdir(cwdold);
       off_t endf;
	endf=lseek(handle, 0L, SEEK_END);
	if(endf<0) {
		message("lseek failed");
		return -1;
		}
	if(endf==0) {
		message("%s is zero",ant);
		return -1;
		}
char *gegs 
#ifdef HAVE_SYS_MMAN_H 
       =(char *)mmap(NULL, endf+1,   PROT_WRITE|  PROT_READ ,  MAP_PRIVATE, handle,0);
	int	nommap=0;
 	if(gegs==MAP_FAILED) {
		nommap=1;
#else
;{
#endif
		gegs=myallocar(char,endf+1);
		lseek(handle, 0L, SEEK_SET);
		ssize_t res;
		if((res=read(handle,gegs,endf))!=endf) {
			message("read(%s..) gives %d instead of %d",ant,res,endf);
			endmmap;
			
			return -1;
			}
		}
	gegs[endf]='\0';
	int addfield=0;
	switch(getkeyinput("(a)ad field/(s)eparate?")) {
		case 'a': addfield=1;break;
		case 's':
		default: addfield=0;break;
		};
	int ret=loaddatatype(gegs,endf,addfield);
	endmmap
	return ret;
	}


int getindatatype(const ScreenPart * const thesub) {
int indatatype=0;
for(const ScreenPart *par,*sub=thesub;(par=sub->contain);sub=par) {
	if(has_super(par,Contains)) {
		indatatype+=sub->oldhalf;
		}
	else {
		if(has_super(par,Repeat))
			indatatype+=(sub->getbytes())*((Repeat *)par)->active;
		else {
			if(has_super(par,HasSub)) {
				HasSub *has=(HasSub *)par;
				for(int i=0;i<has->nrsubparts;i++)
					indatatype+=has->subpart(i)->getbytes();
				}
			}
		}
	indatatype=par->tofilefilterspos(indatatype);
	}
return indatatype;
}
