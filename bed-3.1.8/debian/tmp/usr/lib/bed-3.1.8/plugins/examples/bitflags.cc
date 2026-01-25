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
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <alloca.h>
#include "screenpart.h"
#include "typelist.h"
#include "../dialog.h"
#include "../screen.h"

#define DEBUGUIT(...)
//#define DEBUGUIT(...) fprintf(stderr,__VA_ARGS__)
#define minnum(x,y) (((x)<(y))?(x):(y))
class BitFlags; 
class Editor {
public:
#include "faked.h"
int bitflagsconfig(BitFlags *bitflags=NULL,int kind=1) ;
iscalled(bitflagconfig)
};
class Editor;
	class BitFlags: public ScreenPart {
		int insidetable,usednr,maxtable;
		char **table;
		char *lens;
		char noflag;
		char flagapart;
		char *betweenflags;
		unsigned long long bitmask;
		int ordered;
		public:
		declareparent 
		BitFlags(BitFlags const &fl);
BitFlags( Editor *ed,int x1=0, int y1=0) ; 
virtual int chbytes(int b) override ;
 void recalc(void)  ;
virtual ScreenPart *newone(void) const override ;
	~BitFlags() ;
virtual int formal2bin(const char *formal, unsigned char *bin, int len) const override;
virtual	int bin2formal(unsigned char *bin, char *formal, int len) override ;
virtual	int isel(const int ch) const override ;
#define convsep '`'
virtual int setconvstr(char *convstr) override ;
virtual char *getconvstr(void) override ;
virtual int userconfig(int kind)  override ;
friend int Editor::bitflagsconfig(BitFlags *bitflags,int kind) ;
		};




;
typedef char *charptr;
BitFlags::BitFlags(BitFlags const &fl):ScreenPart(fl), insidetable(fl.insidetable),maxtable(fl.maxtable),noflag(fl.noflag),flagapart(fl.flagapart),bitmask(fl.bitmask),ordered(fl.ordered)
			{
			if(flagapart) {
				betweenflags = new char [flagapart+1];
				memcpy(betweenflags,fl.betweenflags,flagapart+1);
				}
			else
				betweenflags=NULL;
			table=new charptr[maxtable];
			lens=new char[maxtable];
			
			memcpy(lens,fl.lens,insidetable*sizeof(char));
			for(int i=0;i<insidetable;i++) {
				if(fl.table[i]) {
					table[i]=new char[lens[i]+1];
					memmove(table[i],fl.table[i],lens[i]+1);
					}
				else
					table[i]=NULL;
				}
			}
BitFlags::BitFlags( Editor *ed,int x1, int y1) : 
	ScreenPart( ed,5,2,1,x1, y1), 
	maxtable(1),
	noflag('-') ,
	flagapart(0),
	betweenflags(NULL),
	ordered(1)

	 {
		table=new charptr[maxtable];
		lens=new char[maxtable];
		insidetable=0;
		half=maxres;
		recalc();
		};
int BitFlags::chbytes(int b) {
	if(b==-1)
		return 1;
	if(b>0&&b<9) {
		int mt=b*8;
		bytes=b;
		if(mt<insidetable) {
			for(int i=mt;i<insidetable;i++){
				if(table[i])
					delete table[i];
				}
			insidetable=mt;
			recalc();
			}
		return bytes;
		}
	return -1;
	}

void BitFlags::recalc(void) {
	maxres=0;
	bitmask=0LL;
	usednr=0;
	for(int i=0;i<insidetable;i++) {
		if(table[i]) {
			usednr++;
			lens[i]=strlen(table[i]);
			maxres+=lens[i];
			bitmask|=(unsigned long long)1<<i;
			}
		else
			lens[i]=0;
		}
	maxres+=(usednr-1)*flagapart;
	if(half<0||half>maxres)
		half=maxres;
	}
		ScreenPart * BitFlags::newone(void) const {
			return new BitFlags(*this);
			}

	BitFlags::~BitFlags() {
		for(int i=0;i<insidetable;i++) {
			if(table[i])
				delete table[i];
			}
		if(table)
			delete[] table;
		if(lens)
			delete lens;
		if(flagapart>0)
			delete betweenflags;
		}
	int  BitFlags::formal2bin(const char *formal, unsigned char *bin, int len) const {
		unsigned long long was=0LL;
		memcpy((char *)&was,bin,datbytes());
		unsigned long long field=0LL;
		const char *form=formal;
		int thelen=minnum(len,(int)strlen(formal));	
		const char *end=formal+thelen;	
		if(ordered) {
			for(int i=(insidetable-1);i>=0;i--) {
				if(lens[i]) {
					if(!memcmp(form,table[i],lens[i]))
						field|=((unsigned long long)1<<i);
					form+=(lens[i]+flagapart);
					}
				}
			}
		else {
#define skipbetween while(form<end&&(((*form==noflag)&& (form++,1))||(flagapart>0&&form<(end+flagapart)&&!memcmp(form,betweenflags,flagapart)&& (form+=flagapart,1)))) ;
		skipbetween;
				
			while(form<end) {
				for(int i=(insidetable-1);i>=0;i--) 
					if(lens[i])
						if(!memcmp(form,table[i],lens[i])) {
							field|=((unsigned long long)1<<i);
							form+=lens[i];
							goto again;
							}
				break;
				again:
				skipbetween;
				}
			}
		if(form>=end) {
			field|=	(~bitmask)&was;
			memcpy(bin,(char *)&field,datbytes());
			return 0;
			}
		else
			return -1;

		}
		int  BitFlags::bin2formal(unsigned char *bin, char *formal, int len) {
			unsigned long long field=0LL;
			memcpy((char *)&field,bin,datbytes());
			char *form=formal;
			for(int i=(insidetable-1);i>=0;i--) {
				if(lens[i]) {
					if(field&((unsigned long long)1<<i)) {
						memcpy(form,table[i],lens[i]);
						}
					else
						memset(form,noflag,lens[i]);
					form+=lens[i];
					if(flagapart>0) {
						memcpy(form,betweenflags,flagapart);
						form+=flagapart;
						}
					}
				}
			return 0;
			}
		int  BitFlags::isel(const int ch) const {return !iscntrl(ch);}
#define convsep '`'
int  BitFlags::setconvstr(char *convstr) {
	int nrtable;
	char *afterflags=strchr(convstr,convsep);
	if(!afterflags)
		return -1;
	char *convptr=convstr,*end;
	int bitten=datbytes()*8;
//s/(char[ 	]*\*)/charptr /g
	char **newtable=new charptr [bitten];
	char *newlens=new char [bitten];
	for(nrtable=0;nrtable<bitten&&convptr<afterflags;nrtable++) {	
		end=(char *)memchr(convptr,',',afterflags-convptr);
		if(!end||end>afterflags)
			end=afterflags;
		int len=end-convptr;
		if(len) {
			newlens[nrtable]=len;
			newtable[nrtable]=new char[len+1];
			memcpy(newtable[nrtable],convptr,len);
			newtable[nrtable][len]='\0';
			}
		else {
			newtable[nrtable]=NULL;
			newlens[nrtable]=0;
			}
		convptr=end+1;
		}
	if(convptr<afterflags) {
		for(int i=0;i<nrtable;i++)
			if(newlens[i])
				delete newtable[i];
		delete[] newtable;
		delete[] newlens;
		return -1;
		}
	if(nrtable<1) {
		delete[] newtable;
		delete[] newlens;
		return -1;
		}
	convptr=afterflags+1;
	noflag=*convptr++;
	char *en;	
	if((*convptr!=convsep|| (convptr++,!(en=strchr(convptr,convsep))))) {
		for(int i=0;i<nrtable;i++)
			if(newlens[i])
				delete newtable[i];
		delete[] newtable;
		delete[] newlens;
		return -1;
		}
	int len= en-convptr;
	char *newbetween;
	if(len>0) {
		newbetween= new char[len+1];
		memcpy(newbetween,convptr,len);
		newbetween[len]='\0';
		}
	else
		newbetween=NULL;
	convptr+=(len+1);
	ordered=atoi(convptr);
	for(int i=0;i<insidetable;i++)
		if(lens[i])
			delete table[i];
	if(flagapart)
		delete betweenflags;
	if(table) 
		delete[] table;
	if(lens)
		delete lens;
	betweenflags=newbetween;
	flagapart=len;
	insidetable=nrtable;
	table=newtable;
	lens=newlens;
	maxtable=bitten;
	recalc();
	return nrtable;
	}
char * BitFlags::getconvstr(void) {
	static char *convstr=NULL;
	int convlen=maxres+insidetable+100;
	if(convstr)
		delete[] convstr;
	convstr=new char[convlen];
	char *convptr=convstr;
	for(int i=0;i<insidetable;i++) {
		if(lens[i]) {
			strcpy(convptr,table[i]);
			convptr+=lens[i];
			}
		*convptr++=',';
		}
	snprintf(convptr-1,convlen-(convptr-convstr)+1,"%c%c%c%s%c%d",convsep,noflag,convsep,flagapart?betweenflags:"",convsep,ordered);
	return convstr;
	}
int  BitFlags::userconfig(int kind) {
	return editor->bitflagsconfig(this,kind);
	}

int Editor::bitflagsconfig(BitFlags *bitflags,int kind) {
	int res=0;
	int listlen=14;
	int wlin=13;
	int wcol=40;
	int used;
	int ant=0;
	int created;
	int maxtable;
	int bytes;
	char **newtable;
	char *lens;
	int insidetable;
	char **flags;
	int flagslen;
	char notflags[5];
	bool order=true;
	int maxbetween=40;
	char between[maxbetween];
	int flagapart;
	flagapart=bitflags->flagapart;
	if(flagapart>0) {
		memcpy(between,bitflags->betweenflags,flagapart);
		between[flagapart]='\0';
		}
	else
		strcpy(between,"");
	bytes=bitflags->datbytes();
	created=0;
	 maxtable=bitflags->maxtable;
	insidetable=bitflags->insidetable;
	DEBUGUIT("maxtable=%d insidetable=%d\n",	maxtable,insidetable);
	newtable= new charptr [maxtable];
	lens = new char[maxtable];
	memmove(lens,bitflags->lens,sizeof(char)*insidetable);
	memmove(newtable,bitflags->table,sizeof(char*)*insidetable);
	flagslen=maxtable;
	flags=(char **)malloc(sizeof(char *)*flagslen);
	used=0;
	for(int i=0;i<insidetable;i++) {
		if(newtable[i]) {
			flags[used]=(char *)alloca(sizeof(char)*(listlen+1));
			snprintf(flags[used],listlen,"%d: %s",i,newtable[i]);
			used++;
			}
		}
	notflags[0]=bitflags->noflag;
	order=bitflags->ordered;
	notflags[1]='\0';
	char bytestr[40];
	snprintf(bytestr,40,"%d",bytes);
	int separ=1;
	char bitstr[10]="";
	char labelstr[40]="";
	int fromok;
	res=-1;
//#define opt(name,y,x,val) ore(name,y,x,val,comptr) 
//#define optselect(name,sel,items...)  {int startformsiter=showformsiter; Common *comptr=new Common(name,0);##items;comptr->last=(orcheck*)showformsarray[startformsiter+sel];};
do {
	res--;
	initdialog(20)
		linernrmax("~Not char",notflags,1,2,2,5);
		linernrmax("Bet~ween",between,2,2,5,maxbetween);
		checker("O~rder",4,2,order);
		linernrmax("B~ytes",bytestr,6,2,2,5);
		if(created||!kind) {
			optselect(separ,separ, 
				opt("~Separate",8,2,0),
				opt("F~ield",9,2,1));
			fromok=7;
			}
		else
			fromok=5;
		oke(11,2);
		act("~Cancel",11,10);
		listall("~Flags",1,20,flags,used,ant, ant,5,listlen);
		saveact("~Del", 1,33);
		linernrmax("~Bit",bitstr,8,20,5,5);
		linernrmax("~Label",labelstr,9,20,5,39);
		saveact("~Add", 11,29);
#define delres (3+fromok)
#define addres (6+fromok)
#define escres (1+fromok)
#define flagsres (2+fromok)
	if(res<0||res>=addres)
		res=addres-3;
	dodialog(res,wlin,wcol,res);
	erefresh();
	if(res==(INT_MAX-1)) {
		res=0;
		resized();rewriteall();
		continue;
		}
	if(!res||res==escres) {
		if(created) {
			for(int i=0;i<insidetable;i++) 
				if(newtable[i])
					delete newtable[i];
			}
		else {
			for(int i=0;i<insidetable;i++) 
				if(newtable[i]&&(i>=bitflags->insidetable||newtable[i]!=bitflags->table[i]))
					delete newtable[i];
			}
		delete[] newtable;
		delete lens;
		break;
		}
	bytes=atoi(bytestr);
	int bitten=bytes*8;
	if(insidetable>bitten) {
		int newused=0;
		for(int i=0;i<used;i++) {
			if(atoi(flags[i])<bitten) {
				
				}
			else {
				if(newused!=i)
					flags[newused]=flags[i];
				newused++;
				}

			}
		used=newused;
		}
	while(insidetable>bitten) {
		insidetable--;
		if(newtable[insidetable]&&(created||(insidetable>=bitflags->insidetable||bitflags->table[insidetable]!=newtable[insidetable]))) {
			delete newtable[insidetable];
			}
		}
	if(res==fromok) {
		if(used<=0) {
				beep();message("No bit flags specified");erefresh();continue;
				};
		if(created) {
				BitFlags *newbitflags=new BitFlags(this);
				newbitflags->bytes=bytes;
				newbitflags->noflag=notflags[0];
				newbitflags->ordered=order;
				newbitflags->table=newtable;
				newbitflags->maxtable=maxtable;
				newbitflags->insidetable=insidetable;
				newbitflags->lens=lens;
				flagapart=strlen(between);
				newbitflags->flagapart=flagapart;
				if(flagapart) {
					newbitflags->betweenflags=new char[flagapart+1];
					memcpy(newbitflags->betweenflags,between,flagapart+1);
					}
				newbitflags->recalc();
				installpart(newbitflags,separ);
				modechanged();
			break;
			}
		else {
			for(int i=0;i<bitflags->insidetable;i++) {
				if(bitflags->table[i]&&(i>=insidetable||bitflags->table[i]!=newtable[i]))
					delete bitflags->table[i];
				}
			delete[] bitflags->table;
			delete bitflags->lens;
			if(bitflags->flagapart) 
				delete bitflags->betweenflags;
			flagapart=strlen(between);
			bitflags->flagapart=flagapart;
			if(flagapart) {
				bitflags->betweenflags=new char[flagapart+1];
				memcpy(bitflags->betweenflags,between,flagapart+1);
				}
			bitflags->maxtable=maxtable;
			bitflags->insidetable=insidetable;
			bitflags->table=newtable;
			bitflags->lens=lens;
			bitflags->noflag=notflags[0];
			bitflags->ordered=order;
			bitflags->bytes=bytes;
			bitflags->recalc();
			if(kind==0)
				installpart(bitflags,separ);
			modechanged();
			break;
			}

		break;
		}
	if(res==delres) {
			if(ant<0||ant>=used) {
				message("Can't del position %d",ant);erefresh();continue;
				}
			int bit;
			sscanf(flags[ant],"%d",&bit);
			if(bit<0||bit>=insidetable) {
				message("%d outside range\n",bit);erefresh();continue;
				}
			if(newtable[bit]) {
				if(created||(newtable[bit]!=bitflags->table[bit]))
					delete newtable[bit];
				newtable[bit]=NULL;
				}
			used--;
			if(ant<used)
				memmove(flags+ant,flags+ant+1,(used-ant)*sizeof(char *));
			}
	else if(res==addres) {
			int bit=atoi(bitstr);
			int bitten=bytes*8;
			if(bit<0||bit>=maxtable) {
				if(bit<bitten) {
					char **oldtable=newtable;
					char *oldlens=lens;
					newtable=new charptr[bitten];
					lens=new char[bitten];
					if(insidetable>0) {
						memcpy(newtable,oldtable,insidetable*sizeof(char *));
						memcpy(lens,oldlens,insidetable*sizeof(char ));
						delete oldtable;
						delete oldlens;
						}
					maxtable=bitten;
					}
				else {
					beep();
					message("%d outside range",bit);erefresh();
					continue;
					}
				}
			{

				char *oldvalue;
				if(bit<insidetable)
					oldvalue=newtable[bit]; 
				else 
					oldvalue=NULL;
				int ll=lens[bit]=strlen(labelstr);
				newtable[bit]=new char[ll+1];
				memcpy(newtable[bit],labelstr,ll+1);
				if((bit<insidetable)&&oldvalue) {
					if(created||oldvalue!=bitflags->table[bit])
						delete oldvalue;
					char zoek[10];	
					int len=snprintf(zoek,10,"%d: ",bit),u;
					for(u=0;u<used;u++)
						if(!strncmp(zoek,flags[u],len)) 
							break;
					if(u>=used) {
						message("System Error"); 
						if(flagslen>0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
							free(flags);
#pragma GCC diagnostic pop


						return  -1;
						}
					snprintf(flags[u],listlen,"%d: %s",bit,labelstr);
					ant=u;
					}
				else {
				if(used>=flagslen) {
					char **tmp=flags;	
						flags=(char **)malloc(sizeof(char *)*maxtable);
					if(flagslen>0) 
						memmove(flags,tmp,sizeof(char *)*flagslen);
					free(tmp);
					flagslen=maxtable;
					}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
				flags[used]=(char *)alloca(sizeof(char)*(listlen+1));
#pragma GCC diagnostic pop
				ant=used;
					snprintf(flags[used++],listlen,"%d: %s",bit,labelstr);
					if(bit>=insidetable) {
						while(insidetable<bit)
							newtable[insidetable++]=NULL;
						insidetable++;
						}
					}


				}	
			}
		else {
			if(flagsres==res) {
				if(ant>=0&&ant<used) {
					char *ptr=flags[ant];
					char *sep=strchr(ptr,':');
					if(sep) {
						memcpy(bitstr,ptr,sep-ptr);
						strcpy(labelstr,sep+2);
						}
					else {
						message("System Error can't find : in %s",ptr);erefresh();beep();continue;
						}
					}
				}
			}
	} while(1);
	filesup();
	
if(flagslen>0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	free(flags);
#pragma GCC diagnostic pop
return 0;
	};

int Editor::bitflagconfig(void) {
	int fors,bins;
	ScreenPart *super=activemode();
	ScreenPart *part=super->getsubpart(fors,bins,super->half);
	if(part->getname()==BitFlags::desc.getname()) {
		bitflagsconfig((BitFlags *)part);
		super->calcsizes();
		return 0;
		}
	else {
		message("Only applicable to BitFlags");
		beep();
		return -1;
		}
	}
parentd(ScreenPart,BitFlags) 
datatype(BitFlags)
call(bitflagconfig)




