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
#include <alloca.h>
#include <features.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "screen.h"
#include "dialog.h"
#include "datatypes.h"
#include "editor.h"
#include "repeat.h"
#include "ascii.h"
/*
#undef DEBUG
//#define DEBUG(form,...) fprintf(stderr,"line %d",__LINE__);fprintf(stderr,form,__VA_ARGS__)
#define DEBUG(form,x...)
*/
#define TOOBIGMESSAGE message("Too big for screen")
/*
#define naardata(type) int Editor::naar##type(void) { return getmode(type,1,0); }
naardata(ASCII)
naardata(HEX)
*/
#define maxbytes 20
#define maxbase 20
extern char showfileinfo;
int Ascii::userconfig(int kind) {
	return editor->configASCII(this, kind) ;
	}
int Editor::datatypeconfig(void) {
	int fors,bins;
	ScreenPart *sub=parts[mode]->getsubpart(fors,bins, parts[mode]->half) ;
	int ret=sub->userconfig(1);
	parts[mode]->calcsizes();
	shouldreinit=1;
	editup(); 
	return ret;
	}
int Editor::naarASCII(void) {
	Ascii *sub=new Ascii(this);
	return configASCII(sub,0);
	}
/*
int Editor::sizelabels(ScreenPart *sub,int kind,int nr,...) {
	va_list ap;
	va_start(ap,nr);
int ret= sizelabels(sub,kind,nr, ap) ;
	va_end(ap);
return ret;
	}
*/
int Editor::sizelabels(ScreenPart *sub,int kind,int nr,va_list ap,bool setlabel) {
int wcol=33;
int wlin=2*!kind+7+nr;

static int separ=1;
int byt=sub->datbytes();
int nrbyte=byt,res=0;
int last=nr+2,sel=0,issel=0;
char *labels[nr];
int  bytes[nr];
int labelid;
while(1) {
  begindialog(wlin,wcol,nr+2+(!kind)*2+1)
//  begindialog(wlin,wcol,20)
	mvwaddstr(takescr,1,2,sub->getname());
//	mvwaddstr(takescr,1,2,"Install datatype");
int line=2; 
beginoptions(labelid)
	for(int i=0;i<nr;i++) {
		labels[i]=va_arg(ap,char *);
		bytes[i]=va_arg(ap,int);
			opt(labels[i],++line,3,i);
		if(byt==bytes[i]) {
			issel=i;
			if(i==sel) {
				++sel;
				}
			}
		}
endoptions(issel)
	if(!kind) {
		last+=2;
		++line;
		optselect(separ,!separ, 
			opt("~Add field",++line,3,1),
			opt("~Separate",++line,3,0));
		}
	else {
		}
	
	oke(++++line,3);
	act("~Cancel",line,15);
  enddialog(res,sel);
		if(!res||res==last)	{
			editup();
			return 0;
			};
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}

nrbyte=bytes[labelid];
	if(sub->chbytes(nrbyte)<=0||(kind&&parts[mode]->calcsizes()<=0)) {
			sub->chbytes(byt);	
			beep();
			message("Can't change to %d bytes",nrbyte);
			continue;
			}
		if(!kind) {
			if( installpart(sub,separ)<0) {
				beep();
				message("Adding datatype failed");
				delete sub;
				return -1;
				}
			}
		if(setlabel) {
			const char *label=labels[labelid];
			if(sub->userlabel)
				 myfree(sub->userlabel);
			int len=strlen(label)+1;
			sub->userlabel=myallocar(char,len);	
//			if(char *ptr=reinterpret_cast<char *>(memccpy(sub->userlabel,label,'~',len))) {
			if(char *ptr=(char *)memccpy(sub->userlabel,label,'~',len)) {
				int ind=ptr-sub->userlabel;
				memcpy(--ptr,label+ind,len-ind);
				}
			strcpyn(sub->label,sub->userlabel,maxlabel);
			parts[mode]->mklabel();
			showfileinfo=1;
			}
		break;
		}
	return nrbyte;
	}
				

int Editor::configASCII(ScreenPart *screenpart1,int kind) {
	if(!asciisuper(screenpart1))
		return -1;
	Ascii *sub=(Ascii *)screenpart1;
	static int separ=1;
	int last;
	char bytesstr[maxbytes];
	int res;
	int wlin=14, wcol=25;
//	int byt=sub->datbytes();
	int byt=sub->maxres;
	sprintf(bytesstr,"%d",byt);	
	char nonprint[maxbytes];
	sprintf(nonprint,"%c",sub->nonprint);
	int problem=sub->problemchars;
	while(1) {
		long len;
		char *endpo;
		res=0;



	initdialog(11)
		linernrmax("No ~print: ",nonprint,1,3,3,maxbytes);
	int propos;
	switch(problem) {
		case 0: propos=0;break;
		case 'a':propos=2;break;
		default:propos=1;break;
		};
	optselect(problem,propos,
			opt("S~trict",3,3,0),
			opt("+1~61-255",4,3,'u'),
			opt("+1~28-255",5,3,'a'));
	linernrmax("~Nr bytes: ",bytesstr,7,3,3,maxbytes);
	if(!kind) {
		optselect(separ,!separ, 
			opt("~Add field",9,3,1),
			opt("~Separate",10,3,0));
		wlin=12;
		last=9;
		}
	else {
		wlin=9;
		last=7;
		}
	oke(wlin,3);
	act("~Cancel", wlin,15);
	wlin+=2;
//	startwith=last-4;
	dodialog(res,wlin,wcol,last-3-(kind*2));


		if(!res||res==last)	{
			editup();
			return 0;
			};
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		sub->nonprint=nonprint[0];
		sub->problemchars=problem;
		len=strtol(bytesstr,&endpo,10);
		if((endpo!=bytesstr)) {
			if(sub->chbytes(len)<=0||(kind&&parts[mode]->calcsizes()<=0)) {
				sub->chbytes(byt);	
				beep();
				message("Can't change to %d bytes",len);
				continue;
				}
				

		}
		if(!kind)
			if( installpart(sub,separ)<0) {
				beep();
				message("Adding subpart failed");delete sub;
				return -1;
				}
		break;
		}
	return 0;
}








#include "contain.h"
ScreenPart *Editor::activemode(void) {
	return parts[mode];
	}
//int Editor::makemode(void) {
int Editor::addunsigned(void) {
	int tmode=10;
	char ant[maxbytes];
	char base[maxbase]="";
	int res;
	static int separ=0;
	int wlin=8, wcol=42;
	int fors,bins;
	ScreenPart *sub=parts[mode]->getsubpart(fors,bins, parts[mode]->half) ;
	if(!sub) {
		message("No mode selected");
		beep();
		return -1;
		}
	int byt=sub->datbytes();
	sprintf(ant,"%d",byt);	
#define predef(x) ((!((x)%2))&&strchr("1458",((x)/2)+'0'))
	tmode=predef((int)sub->type)?(int)sub->type:0xff;
	
	if(!predef(tmode)) {
		if(digitsuper(sub)) {
			sprintf(base,"%d",(int)sub->type);
			}
		else
			tmode=10;
		}
	while(1) {
		int startwith=predef(tmode)?(typevolg(tmode)-1):4;
		long len;
		char *endpo;
		res=0;
		Common combase(tmode),*com=&combase;
		Common fieldbase(separ),*field=&fieldbase;
		palcsconfig(res,wlin,wcol,6,(com->last=(orcheck*)showformsarray[startwith],field->last=(orcheck *)showformsarray[7]),
			ore("~Hex",1,3,16,com),
			ore("~Dec",1,13,10,com),
			ore("Oc~t",2,3,8,com),
			ore("B~in",2,13,2,com),
			ore("Custo~m: ",1,23,0xff,com),
			linernrmax("~Base nr: ",base,2,23,3,maxbase),
			linernrmax("~Nr bytes: ",ant,4,3,3,maxbytes),
			ore("~Add field",5,26,1,field),
			ore("~Separate",6,26,0,field),
			oke(6,3),
			act("~Cancel", 6,15)
			); 
		if(!res||res==11)	{
			editup();
			return 0;
			};
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		len=strtol(ant,&endpo,10);
		if((endpo!=ant)) {
			if(tmode==0xff) {
				int b=strtol(base,&endpo,10);
				if((endpo!=ant)&&b>1&&b<37) 
					tmode=b;
				else
					continue;
				}
			editup();

			int oldmode=mode;
			MultiDigit *part=new  MultiDigit(this,tmode,len);
			if(installpart(part,separ)<0) {
				message("Adding %s failed",part->getname());erefresh();
				beep();
				delete part;
				mode=oldmode;
				continue;

				}
			}
		else {
			beep();
			continue;
			}
		break;
		}
	return 0;
}
int Editor::tomodenr(int newmode) {
		if(newmode<0||newmode>=nrparts) 
			return -1;
		parts[mode]->leave();
		mode=newmode;
		editpos=lowborder();
		parts[mode]->select();
		return 0;
		}
int Editor::tomode(DataType type,int len,int order) {
				for(int i=0;i<nrparts;i++) 
//					if(parts[i]->type==type&&(asciisuper(parts[i]))||((parts[i]->bytes==len)&&(digitsuper(parts[i])))) {
					if(parts[i]->type==type&&(asciisuper(parts[i])||((parts[i]->bytes==len)&&digitsuper(parts[i])))) {
						tomodenr(i);
						 if(!nomessage) 
							fileinfo();
						return 0;
						}
				return -1;
				};
int Editor::tosubscreen(void) {
		int ch=getch()-'0';
		if(ch>=0&&ch<nrparts) {
			return tomodenr(ch);
			}
		beep();
		return -1;
		};
/*
int Editor::asktomodeget(void) {
		 output("What mode do you want ((h)ex/(o)ct/(d)ecimal/(a)scii/(b)it/(c)ancel) ?");
		 return tomodeget();
		 }
int Editor::tomodeget(void) {
		DataType type;
		char ch;
		switch((ch=getch())) {
			case 'h': type=HEX;break;
			case 'd': type=DEC;break;
			case 'o': type=OCT;break;
			case 'b': type=BIN;break;
			case 'a': type=ASCII;break;
			case 27:;
			case 'c': clearmessage();beep();return -1;
			default: beep();return asktomodeget();
			};
		clearmessage();
		ScreenPart *part;
		if(type==ASCII) 
			 part=new Ascii(this);
		else	
			  part=new  MultiDigit(this,(int)type,1,0);
		return installpart(part);
		}

*/




/* Specifiek voor een file */

/* ADD: posibility to expand file */


#include <limits.h>
long maxvirtualwidth= INT_MAX-1;
int Editor::getcols(void) {
	int i,j,chsize,chars;
	int *maxes=(int *)alloca(sizeof(int)*nrparts);
	int spaces=0;
	spaces=-parts[nrparts-1]->apart;
	for(i=0;i<(nrparts-1);i++) {
		spaces+=parts[i]->spaceafter-parts[i]->apart;
		maxes[i]=parts[i]->bytes;
		}
	maxes[i]=parts[i]->bytes;
	for(i=0;i<nrparts;i++) {
		for(j=i+1;j<nrparts;j++) {
			div_t res;
			res=div(maxes[i],maxes[j]);
			if(res.rem==0) 
				maxes[j]=1;	
			else {
				res=div(maxes[j],maxes[i]);
				if(res.rem==0) {
					maxes[i]=1;
					break;
					}
				}
			}
		
		}
	for(i=0,chars=1;i<nrparts;i++)
		chars*=maxes[i];
	
	for(i=0,chsize=0;i<nrparts;i++)
		chsize+=((parts[i]->maxres+parts[i]->apart)*(chars/parts[i]->bytes));
	int totalleft=Screenwidth-nrnumber()-spaces;
	if(chsize<totalleft) {
		virtualwidth=Screenwidth;
		return (totalleft/chsize)*chars;
		}
	if(chsize<(maxvirtualwidth-spaces-nrnumber())) {
		virtualwidth=chsize+spaces+nrnumber();
		return  chars;
		}
	return 0;
	}

void Editor::enlargeparts(void) {
	if(nrparts>=maxparts) { 
		parts=myrealloc(ScreenPart **,parts,ScreenPart *,maxparts*2,maxparts); 
		maxparts*=2; 
		}
	}

int Editor::copymode(void) {
	enlargeparts();
	int forstart,binstart;
	ScreenPart *sub=parts[mode]->getsubpart(forstart,binstart,parts[mode]->half),*two;
	two=sub->newone();
	two->mklabel();
	if(isrepeat(two)) {
		Contains *con= new Contains(this);
		if(con->addsubpart( two)<0) {
			delete con;
			delete two;
			message("System Error: 	addsupart failed");
			return -1;
			}
	con->mklabel();
		parts[nrparts++]=con;
		}
	else {
		parts[nrparts++]=two;
		}
	parts[mode]->leave();
	if(getcols()<1) {
		delete parts[--nrparts];
		beep();
		TOOBIGMESSAGE;
		return -1;
		}
	mode=nrparts-1;
	shouldreinit=1;
	if(!nomessage)
		fileinfo();

	return 0;
	}
/*
	parts[mode]->leave();
	for(int i=nrparts;i>to;i--)
		parts[i]=parts[i-1];
			for(int i=to;i<nrparts;i++)
				parts[i]=parts[i+1];
			return -1;
	nrparts++;
	if(getcols()<1) {
		delete parts[to];
		nrparts--;
		for(int i=to;i<nrparts;i++)
			parts[i]=parts[i+1];
		beep();
		TOOBIGMESSAGE;
		return -1;
		}
	mode=to;
*/
int Editor::addbefore(DataType type,int to,int len,int order) {
	enlargeparts();
	if((to<0||to>nrparts)||!(type==ASCII||(type>1&&type<37)))
		return -1;
	ScreenPart *part;
	if(type==ASCII) 
		 part=new Ascii(this);
	else {
		if(type>1&&type<37)
			  part=new  MultiDigit(this,(int)type,len,order);
		else return -1;
		}

	return installpart(part);
/*	if(!nomessage)
		fileinfo();
*/
	return 0;
	}

int Editor::prevmode(void) {
	if(mode>0) {

		tomodenr(mode-1);
		if(!nomessage)
			fileinfo();
		return 0;
		}
/*
	if(visualoffset!=0) {
		int x,y;
		parts[mode]->postoxy(editpos,x,y);
		if(x<visualwidth) {
			visualoffset=0;
			updated=0;
			}
		}
		*/
	beep();
	return -1;
	}
int Editor::nextmode(void) {
	if(mode<(nrparts-1)) {
		tomodenr(mode+1);
		if(!nomessage)
			fileinfo();
		return 0;
		}
	beep();
	return -1;
	}
int Editor::zoommode(void) {
	ScreenPart *old=parts[0];
	parts[0]=parts[mode];
	parts[mode]=old;
	mode=0;
	for(int i=1;i<nrparts;i++)
		delete parts[i];
	nrparts=1;
	shouldreinit=1;
	return 0;
	}
	/*
int Editor::removemode(void) {
	if(nrparts>1) {
		delete parts[mode];
		nrparts--;
		for(int i=mode;i<(nrparts);i++)
			parts[i]=parts[i+1];
		if(mode>=nrparts)
			mode--;
		shouldreinit=1;
		return 0;
		}
	else {
		beep();
		return -1;
		}
	return 0;
	}
	*/
#define maxorder 20
int Editor::reorderunit(void) {
	int fors,bins;
	ScreenPart *sub=parts[mode]->getsubpart(fors,bins,parts[mode]->half);
	if(!sub) {
		message("Strange subpart");
		beep();
		return -1;
		}
	int wlin=9,wcol=29,res=0;
	char order[maxorder];
	strcpy(order,"1");
	while(1) {
		again:
		alcsconfig(res,wlin,wcol,1,
			linernrmax("Order ~unit:",order,2,3,5,maxorder),
			oke(4,3),
			act("~Cancel", 6,3)
			); 
		if(!res||res==4) { 
			editup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			goto again;
			}
		int i=atoi(order);	
		short bytes=sub->datbytes();
		if(i<1||i>bytes) {
			beep();
			message("The unit should be between 1 and %d",bytes);
			erefresh();
			continue;
			}
		if(bytes%i) {
			beep();
			message("%d (number of bytes per item) should be devisiable by it",bytes);
			erefresh();
			continue;
			}
		if(sub->addreverse(sub->datbytes(),i)<0) {
			beep();
			message("adding reverse failed");
			erefresh();
			beep();
			continue;
			}
		parts[mode]->mklabel();
		updated=0;
		break;
		}
	editup();
	return 0;
	}
/*
int Editor::reorderunit(void) {
	char ans[50];
	short bytes=parts[mode]->datbytes();
	getinput("Order unit: ",ans) ;
	if(!ans[0])
		return -1;
	int i=atoi(ans);	

	if(i<1||i>=bytes) {
		beep();
		message("The unit should be between 1 and %d",bytes);
		return -1;
		}
	if(bytes%i) {
		beep();
		message("%d (number of bytes per item) should be devisiable by it",bytes);
		return -1;
		}
	parts[mode]->reorder=i;
	updated=0;
	return 0;
	}
*/
#define myatoi atoi
#define maxliner 20


int Editor::changebytes(void) {
	int fors,bins;
	ScreenPart *sub=parts[mode]->getsubpart(fors,bins, parts[mode]->half) ;
	if(!sub) {
		message("Some error selecting field");
		beep();
		return -1;
		}
	int by=sub->datbytes();
	if(sub->screenbytes(-1)<1) {
		message("Number of bytes of this datatype can not be changed");
		beep();
		return -1;
		}
	OFFTYPE unitpos=(editpos/parts[mode]->bytes)*parts[mode]->bytes+filepos;
	int wlin=10,wcol=29,res=0;
	char bytes[50];
	sprintf(bytes,"%d",by);
	again:
	alcsconfig(res,wlin,wcol,0,
		linernrmax("~Nr bytes:",bytes,2,3,5,50),
		oke(5,3),
		act("~Cancel", 7,3)
		); 
	if(!res||res==3) { 
		editup(); 
		return -1; 
		}	
	if(res==INT_MAX)  {
		menuresized();
		wrefresh(editscreen);
		goto again;
		}
	memorize();
	if(parts[mode]->screenbytes(atoi(bytes))<1) {
		beep();
		message("Can not change to %s",bytes);
		wrefresh(editscreen);
		parts[mode]->screenbytes(by);
		goto again;
		}
	if(getcols()<1) {
		parts[mode]->screenbytes(by);
		beep();
		TOOBIGMESSAGE;
		wrefresh(editscreen);
		goto again;
		}
	shouldreinit=1;
	editup(); 

	toborderpos(unitpos) ;
	return 0;
	}
int Editor::changebase(void) {
	short ba=parts[mode]->chbase(0);
	if(ba==-1) {
		message("Base of this datatype can not be changed");
		beep();
		return -1;
		}
	ba=-ba;
	int wlin=10,wcol=29,res=0;
	char base[maxbase];
	sprintf(base,"%d",ba);
	again:
	alcsconfig(res,wlin,wcol,0,
		linernrmax("~Base nr: ",base,2,3,5,maxbase),
		oke(5,3),
		act("~Cancel", 7,3)
		); 
	if(!res||res==3) { 
		editup(); 
		return -1; 
		}	
	if(res==INT_MAX)  {
		menuresized();
		wrefresh(editscreen);
		goto again;
		}
	memorize();
	if(parts[mode]->chbase(atoi(base))<1) {
		beep();
		message("Can not change base to %s",base);
		wrefresh(editscreen);
		goto again;
		}
	if(getcols()<1) {
		parts[mode]->chbase(ba);
		beep();
		TOOBIGMESSAGE;
		wrefresh(editscreen);
		goto again;
		}
	editup(); 
	shouldreinit=1;
	return 0;
	}
int Editor::getanswer(const char *question,char *answ,int anslen,int ansroom) {
	int wlin=10,wcol,res=0;
	int qlen=strlen(question);
	wcol=qlen+10+ansroom;
	while(1) {
		alcsconfig(res,wlin,wcol,0,
			linernrmax(question,answ,2,3,ansroom,anslen),
			oke(5,3),
			act("~Cancel", 7,3)
			); 
		if(!res||res==3) { 
			editup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		editup(); 
		return res;
		}
	}
int Editor::installpart(ScreenPart *sub) {
	return partconfig(sub,0);
	}
int Editor::partconfig(ScreenPart *sub,int kind) {
	int varbyt,varbase;
	static int separ=1;
	int last;
	char bytesstr[maxbytes];
	char basestr[maxbase];
	int res;
	int byt=sub->datbytes();
	if(sub->chbytes(-1)>0) {
		varbyt=1;
		sprintf(bytesstr,"%d",byt);	
		}
	else
		varbyt=0;
	if(sub->chbase(-1)>0) {
		sprintf(basestr,"%d",sub->type);	
		varbase=1;
		}
	else
		varbase=0;
	if(kind&&!varbase&&!varbyt) {
		beep();
		message("Nothing to configure for %d",sub->getname());return -1;
		}
	while(1) {
		long len;
		char *endpo;
		res=0;


const int wlinset=5+(!kind)*3+(varbase?2:0)+(varbyt?2:0);

	int wlin=3;
const int wcol=25;
begindialog(wlinset,wcol,8)
		last=2;
	mvwaddstr(takescr,1,2,sub->getname());
		if(!kind) {
			optselect(separ,!separ, 
					opt("~Add field",wlin++,3,1),
					opt("~Separate",wlin,3,0));
			last+=2;
			wlin+=2;
			}
		if(varbase) {
			linernrmax("~Base nr: ",basestr,wlin,3,3,maxbase);
			wlin+=2;	
			last++;
			}
		if(varbyt) {
			linernrmax("~Nr bytes: ",bytesstr,wlin,3,3,maxbytes);
			last++;
			wlin+=2;
			}
		oke(wlin,3);
		act("~Cancel",wlin ,15);
	wlin+=2;
enddialog(res,last-2-kind)
		if(!res||res==last)	{
			editup();
			return 0;
			};
		if(res==INT_MAX)  {
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		if(varbyt) {
			len=strtol(bytesstr,&endpo,10);
			if((endpo!=bytesstr)) {
				ScreenPart *super=sub->getsuperpart();
				if(super->screenbytes(len)<1) {
					super->screenbytes(byt);
					beep();
					message("Can't change to %d bytes",len);erefresh();
					continue;
					}

				}
			else {
				beep();
				continue;
				}
			}
		if(varbase) {
			len=strtol(basestr,&endpo,10);
			if((endpo!=basestr)) {
				if(sub->chbase(len)<=0) {
					beep();
					message("Can't change to %d base",len);
					continue;
					}

				}
			else {
				beep();
				continue;
				}
			}
		if(!kind)
			if(installpart(sub,separ)<0) {
				message("Adding %s failed",sub->getname());beep();return -1;
				}
		break;
		}
	return 0;
}
int Editor::installpart(ScreenPart *sub,int separ) {
	if(sub->bytes<=0)
		return -1;
	int ret;
	if(separ) {
		OFFTYPE unitpos=(editpos/parts[mode]->bytes);
		unitpos=unitpos*parts[mode]->bytes+filepos;
		ret=addtocontain(sub);
		toborderpos(unitpos);
		}
	else {
		int chars;
		enlargeparts() ;
		parts[nrparts++]=sub;
		if((chars=getcols())) {
			parts[mode]->leave();
			mode=nrparts-1;
			shouldreinit=1;
			editup();
			ret=0;
			}
		else {
			nrparts--;
			return -1;
			}
		}
	return ret;
	}


ScreenPart *Editor::replacepart(ScreenPart *part,int pos) {
	if(!part)
		return parts[mode];
	if(pos<0)
		pos=mode;
	if(pos>=0&&pos<nrparts) {
		ScreenPart *old=parts[pos];
		parts[pos]=part;
		return old;
		}
	return NULL;
	}
/*
convstr:
apart
spaceafter
userlabel
*/
#define MAXAPART 20
#define MAXSPACEAFTER 20
#define MAXCONFSTRING 256
#define MAXUSERLABEL 80
void ScreenPart::setuserlabel(const char *label) {
	ScreenPart *sub=this;
			if(sub->userlabel)
				 myfree(sub->userlabel);
			int len=strlen(label)+1;
			sub->userlabel=myallocar(char,len);	
			memcpy(sub->userlabel,label,len);
			strcpyn(sub->label,label,maxlabel);
			sub->editor->parts[editor->mode]->mklabel();
			showfileinfo=1;
	}
int Editor::dataconfig(void) {
	int res=0;
	char label[MAXUSERLABEL],conf[MAXCONFSTRING],apart[MAXAPART],spaceafter[MAXSPACEAFTER];
	ScreenPart *sub=parts[mode]->getsubpart();
	strcpyn(conf,sub->getconvstr(),MAXCONFSTRING);
sub->mklabel();
	strcpyn(label,sub->label,MAXUSERLABEL);
	snprintf(apart,MAXAPART,"%d",sub->apart);
	snprintf(spaceafter,MAXSPACEAFTER,"%d",sub->spaceafter);
int aget,sget;
do {
	initdialog(8)
		linernrmax("~Label: ",label,2,3,23,MAXUSERLABEL);
		linernrmax("~Apart",apart,4,3,3,MAXAPART);
		linernrmax("~Spaceafter",spaceafter,4,19,3,MAXSPACEAFTER);
		linernrmax("Con~f: ",conf,6,3,24,MAXCONFSTRING);
		oke(8,3);
		act("~Cancel", 8,15);
	dodialog(res,11,40,4);
	if(!res||res==6)	{
		editup();
		return 0;
		};
	if(res==INT_MAX)  {
		sget=aget=-1;
		menuresized();
		wrefresh(editscreen);
		continue;
		}
	aget=atoi(apart);
	sget=atoi(spaceafter);
	} while(aget<0||sget<0);
	if(label[0]) {
		if(strcmp(sub->label,label)) {
			sub->setuserlabel(label) ;
/*
			if(sub->userlabel)
				 myfree(sub->userlabel);
			int len=strlen(label)+1;
			sub->userlabel=myallocar(char,len);	
			memcpy(sub->userlabel,label,len);
			strcpyn(sub->label,label,maxlabel);
			parts[mode]->mklabel();
			showfileinfo=1;
*/
			}
		}
	else {
		if(sub->userlabel)
			myfree(sub->userlabel);
		sub->userlabel=NULL;
		parts[mode]->mklabel();
		showfileinfo=1;
		}
	sub->setconvstr(conf); 

sub->apart=aget;
sub->spaceafter=sget;
editup();
parts[mode]->calcsizes();
shouldreinit=1;
return 0;
}
/*
ScreenPart **hiddenparts=NULL;
int firsthidden,lasthidden;
int allochidden(int nr) {
	hiddenparts= myrealloc(ScreenPart **,hiddenparts,ScreenPart *,lasthidden+nr,lasthidden) ;
	}
int Editor::hidepart(void) {
	if(nrparts<1)
		return -1;	
	allochidden(1);
	hiddenparts[lasthidden++]=parts[mode];		
	nrparts--;
	if(nrparts>mode)
		memmove(parts+mode,parts+mode+1,(nrparts-mode)*sizeof(ScreenPart *));
	if(mode)
		mode--;
	return nrparts;
	}
*/
