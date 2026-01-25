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
/* Makes use of index2table.plug and is used by ../../misc/dynsym.bedda         */
/* ../../misc/sectionheader.bedda ../../misc/symtab.bedda                    */
/* Find symbolstring offsets for sectionheader, dynsym and symtab            */
/* symboltables                                                              */
/*                                                                           */
/* Sat Nov 11 19:26:01 2000                                                  */
#include "global.h"
BEDPLUGIN
#include <string.h>
#include "index2table.h"
#define waslong int
#define STARTSECHEADPOS 32 // #4
#define SECHEADSIZEPOS 46 // #2
#define SECNRPOS 48 // #2
#define STRINDEXPOS 50 //	#2

#define TYPEOFF 4
#define OFFADDR 12
#define OFFOFF 16
#define OFFSIZE 20
#define OFFES 36

#define DYNSYM 11
#define PROGBITS 1
#define SYMTAB 2
#define STRTAB 3
class Editor {
public:
#include "faked.h"
int  GeenEditor startindex2table(Filterprocptr) ;
int  GeenEditor getanswer(const char *question,char *answ,int anslen,int ansroom=5);

int  GeenEditor getsymposition(int &length, int &startpos) ;

int  GeenEditor getdynposition(int &length, int &startpos) ;

int  GeenEditor getsymtabposition(int &length, int &startpos) ;
int symtabposfromsection(int &length, int &startpos,int symtype,const char *symtabstr,int strtype,const char *strtabstr) ;
unsigned waslong  GeenEditor symboloffset(char *name,waslong &datasize);
int  GeenEditor getsymtabentry(char *name,char *entry,unsigned waslong startsymtab,int off,int  entlen,int entnr,unsigned waslong strtab ) ;
iscalled(section)
iscalled(dynsym)
iscalled(symtab)
iscalled(gotosymbol)
};
class Elfdata { 
	char elfheader[52];
//	char *sectionheader;
	int err;
	public:
	Editor *ed;
	Elfdata(Editor *edit): ed(edit) {
		unsigned char elfreg[]={0x7F,0x45,0x4C,0x46};
		err=0;
		ed->getmem(0,elfheader,52);
		if(memcmp(elfheader,elfreg,sizeof(elfreg))) {
			err=1;
			return;
			}
		if(secheadsize()<OFFES)
			err=1;
		if(secnr()<0)
			err=1;
	 	if(startsecheads()< 0)
			err=1;
		}
	int error(void) {
		return err;
		}
	 waslong startsecheads(void) {
		return *((waslong *)(elfheader+STARTSECHEADPOS));
		}
	short secheadsize(void) {
		return *((short *)(elfheader+SECHEADSIZEPOS));
		}
	short secnr(void) {
		return *((short *)(elfheader+SECNRPOS));
		}

	waslong sectionnames(int &len) {
		short strindex=*((short *)(elfheader+STRINDEXPOS));
		waslong thissec=startsecheads()+strindex*secheadsize();	
		int starttable;
		if(ed->getmem(thissec+OFFOFF,(char *)&starttable,4)!=4|| ed->getmem(thissec+OFFSIZE,(char *)&len,4)!=4) {
		err=1;
		return -1;
		}
		return starttable;
		}
	waslong startsectionsnames(void) {
		int len;
		return sectionnames(len);
		}

	};

#define fromstart(iter,startsec,left)  (((iter)>=(left))?((iter)-(left)):((iter)+(startsec)))
int getsection(Elfdata *elf, int sectype,const char *secname, int len, char *sectio,int startsec) {
	int lenz=len+1;
	char label[lenz];
	int secnr=elf->secnr();
	int left=secnr-startsec;
	waslong starttable=elf->startsectionsnames();
	if(starttable<0)
		return -1;
	waslong	startsecheads=	elf->startsecheads();
	waslong secheadsize=elf->secheadsize();
	for(int iter=0;iter<secnr;iter++) {
		int i=fromstart(iter,startsec,left);
		elf->ed->getmem(startsecheads+i*secheadsize,sectio,secheadsize);
		if(sectio[TYPEOFF]==sectype) {
				elf->ed->getmem(starttable+*((int*)sectio),label,lenz);
				   if(!strncmp(label,secname,lenz)) {
					return i;
					}
			};
		};
	return -1;	
	}
index2tablefindfunction(Section,getsymposition)
int Editor::getsymposition(int &length, int &startpos) {
	Elfdata elf(this);
	if(elf.error())
		return -1;
	if((startpos=elf.startsecheads())<0)
		return -1;
	return elf.sectionnames(length);
	}

int Editor::section(void) {
	return startindex2table(&getSection);
	}
index2tablefindfunction(Dynsym,getdynposition)

int Editor::dynsym(void) {
	return startindex2table(&getDynsym);
	}

index2tablefindfunction(Symtab,getsymtabposition)

inline int Editor::symtabposfromsection(int &length, int &startpos,int symtype,const char *symtabstr,int strtype,const char *strtabstr) {
Elfdata elf(this);
if(elf.error())
	return -1;
short secsize=elf.secheadsize();
char sectio[secsize];
int starts=0;
#define getsec(type,str)  if((starts=getsection(&elf, type,str,strlen(str),sectio,starts)<0)) return -1;
getsec(symtype,symtabstr);
startpos=*((int *)(sectio+OFFOFF));
getsec(strtype,strtabstr);
length=*((int *)(sectio+OFFSIZE));
int symtabstrstart=*((int *)(sectio+OFFOFF));
#undef getsec
return symtabstrstart;	
}


int Editor::getsymtabposition(int &length, int &startpos) {
	return symtabposfromsection(length, startpos, SYMTAB,".symtab",STRTAB,".strtab");
	}
int Editor::getdynposition(int &length, int &startpos) {
	return symtabposfromsection(length, startpos, DYNSYM,".dynsym",STRTAB,".dynstr");
	}

int Editor::symtab(void) {
	return startindex2table(&getSymtab);
	}
sethelp(section,"Used by composed datatype file sectionheader.bedda to show\nlabels of names in section header");
sethelp(dynsym,"Used by composed datatype file dynsym.bedda to show the procedure\nnames in dynsym (dynamische) symbol table.");
sethelp(symtab,"Used by composed datatype file symtab.bedda to show the procedure\nnames in symtab (statische) symbol table.");
call(section)
call(dynsym)
call(symtab)

const int ENTRYSIZE=16;

#define getsectionaddress(sectio) *((int *)(sectio+OFFADDR))
#define getsectiontype(sectio) *((int *)(sectio+TYPEOFF))
#define getsectionoffset(sectio) *((int *)(sectio+OFFOFF))
#define getsectionsize(sectio) *((int *)(sectio+OFFSIZE))
#define getsectionelsize(sectio) *((int *)(sectio+OFFES))
#define getsectionelnr(sectio) (getsectionsize(sectio)/getsectionelsize(sectio))
#define DATAADDRESS 4
#define DATASIZE 8
#define DATAASTRADDR 0
inline unsigned waslong getdatasize(char *entry) {
		return *((unsigned waslong *)(entry+DATASIZE));
		}
inline unsigned waslong getdataaddress(char *entry) {
		return *((unsigned waslong *)(entry+DATAADDRESS));
		}
inline unsigned waslong getdatastraddr(char *entry) {
		return *((unsigned waslong *)(entry+DATAASTRADDR));
		}
int Editor::getsymtabentry(char *name,char *entry,unsigned waslong startsymtab,int off,int  entlen,int entnr,unsigned waslong strtab ) {
	int len=strlen(name)+1;
	char sym[len];
	for(int i=0;i<entnr;i++) {
		getmem(startsymtab+i*entlen,entry,entlen);
		unsigned waslong addr=getdatastraddr(entry);
		getmem(strtab+addr,sym,len);
		if(!memcmp(name,sym,len))
			return i;
		}
	return -1;
	}
int symbolentry(Elfdata *elf,char *name,char *entry,int symtype,const char *symtab, int strtype,const char *strtab) {
	int start=0;
	short secsize=elf->secheadsize();
	if(secsize<OFFES)
		return -1;
	char sectio[secsize];
	if((start=getsection(elf,symtype,symtab,strlen(symtab),sectio,start)>=0)) {
		int startsymtab=getsectionoffset(sectio);
		int entnr=getsectionelnr(sectio);
		int entlen=getsectionelsize(sectio);
		if((start=getsection(elf,strtype,strtab,strlen(strtab),sectio,start)>=0))  {
			int strtab=getsectionoffset(sectio);
return elf->ed->getsymtabentry(name,entry,startsymtab,0,
entlen,entnr, strtab );
			}
		}
	return -2;
	}
/*
walks throught all sections and see if add is between section addres and
section addres+size
*/

unsigned waslong getsectionentry(Elfdata *elf,unsigned waslong add,char *sectio) {
	 waslong start= elf->startsecheads();
	short nr=elf->secnr();
	short seclen=elf->secheadsize();
	for(short i=0;i<nr;i++) {
		elf->ed->getmem(start+i*seclen,sectio,seclen);
		unsigned waslong secad=getsectionaddress(sectio);
		unsigned waslong secend=	secad+getsectionsize(sectio);
		if(add>=secad&&add<secend)
			return getsectionoffset(sectio);
		}
	return 0;
	}


	#define erbij 0
unsigned waslong Editor::symboloffset(char *name,waslong &datasize){ 
	Elfdata elf(this);
	if(elf.error())
		return 0;
	char sectio[elf.secheadsize()];
	char entry[ENTRYSIZE+erbij];
	if(symbolentry(&elf,name, entry, SYMTAB,".symtab",STRTAB,".strtab")<0&&
		symbolentry(&elf,name,entry, DYNSYM,".dynsym",STRTAB,".dynstr")<0) {
			return 0;
			}
	datasize =getdatasize(entry) ;
	unsigned waslong dataaddress= getdataaddress(entry); 
	unsigned waslong offset=getsectionentry(&elf,dataaddress,sectio);
	if(offset) {
		if(getsectiontype(sectio)==PROGBITS) {
			return offset+dataaddress-getsectionaddress(sectio);
			}
		else
			return 0;
		}
	return offset;
	}

#include "multidigit.h"
int Editor::gotosymbol(void){ 
	char symbol[256]="";
	if(getanswer("Symbol: ",symbol,256,20) <0)  {
		message("Wrong answer");return -1;
		}
	waslong symsize;
	unsigned waslong off=symboloffset(symbol,symsize) ;
	if(off==0) {
		message("Can't find %s",symbol);	 return -1;
		}
	int takesize;
	if(symsize>0&&symsize<5) {
		takesize=symsize;
		}
	else
		takesize=1;
	ScreenPart *act=activemode();
	if(act->getbytes()!=takesize) {
		ScreenPart *mul=new Digit(this,16,takesize);
		if(installpart(mul,0)<0) {
			delete mul;
			}
		}
	message("%s at %d (#%d)",symbol,off,symsize);
	return toborderpos(off);
	}
sethelp(gotosymbol, "Procedure searches in symbol tables symtab and dynsym for a given\nsymbol. It sets the cursor to its fileoffset and starts a better fitting\ndatatype. It shows the offset and its size in the message line.");

call(gotosymbol)
