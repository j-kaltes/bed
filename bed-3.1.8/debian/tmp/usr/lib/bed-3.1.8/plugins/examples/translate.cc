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
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "../filter.h"
#include "../screenpart.h"
#include "../ascii.h"
#include "../multidigit.h"
#include "../dialog.h"
#include "../screen.h"
#include "../hassub.h"

//#define  debug 1

#ifdef DEBUGFLAG
#warning "Debug informatie"
#endif
#ifdef OLDGCCVARMACRO
	#ifdef DEBUGFLAG
	#define NDEBUG(VARPAR...)  {fprintf(stderr, VARPAR);fflush(stderr);}
	#else
	#define NDEBUG(VARPAR...)
	#endif
#else
	#ifdef DEBUGFLAG
	#define NDEBUG(...) { fprintf(stderr, __VA_ARGS__);fflush(stderr);}
	#else
	#define NDEBUG(...)
	#endif
#endif
	#ifdef debug
	#define DEBUG2(cond,...) { if(cond){fprintf(stderr, __VA_ARGS__);fflush(stderr);}}
	#else
	#define DEBUG2(...)
	#endif
extern unsigned short block_crc(const unsigned char *d, int len);
//#include "crc.c"
#define maxnum(x,y) (((x)>(y))?(x):(y))
#define minnum(x,y) (((x)<(y))?(x):(y))
struct databuf {
  int len;
   unsigned char *buf;
  };
int compvalue(const struct databuf *const one,const struct databuf * const two) {
  if(one->len!=two->len)
    return 1;
  return memcmp(one->buf,two->buf,one->len);
  }
ScreenPart *asktype(Editor *ed);
extern unsigned short block_crc(const unsigned char *d, int len);
unsigned short hashvalue(const struct databuf *const dat) {
  return block_crc(dat->buf, dat->len) ;
  }
#define hashfunctie(X) hashvalue((struct databuf *)X) 
#define compfunctie(X,Y) compvalue((struct databuf *)X,(struct databuf *)Y)

#include "hash.h"
extern char *filtername(Translate) ;

static inline int transhexnum(int x)  {
  int res= x+((x<10)?'0':('A'-10));
  return res;
  }
static char *tohexstring(char *conv,unsigned char *buf,int len) {
  char *ptr=conv;
  for(int i=0;i<len;i++) {
    unsigned char ch=buf[i];
    *ptr++=transhexnum(ch>>4);
    *ptr++=transhexnum(ch&0xf);
    }
  return ptr;
  }
static inline char    *addtostr(char *conv,struct databuf *name,struct databuf *meaning) {
    char *ptr=conv;
    ptr=tohexstring(ptr,name->buf,name->len);
    *ptr++=':';
    ptr=tohexstring(ptr,meaning->buf,meaning->len);
	return ptr;
  }
static void dooraddtostr(void *gegs,void *meaning,void *name) {
	char **convptr=(char **)gegs;
	*convptr=addtostr(*convptr,(struct databuf *)name,(struct databuf *)meaning);
        *(*convptr)++=',';
	}
	
class Translate: public Filter {
  public:

  Hash toscreen;
  Hash fromscreen;
  int minscreen,minfile;
	int32_t labelnr;
  struct databuf unknown;
mutable  char *conf;
mutable  int maxlen;
int addunknown(unsigned char *unk,int len) {
	if(unknown.len<0)
		free(unknown.buf);
	unknown.buf=(unsigned char *)malloc(len);
	memcpy(unknown.buf,unk,len);
	unknown.len=len;
	if(len<minscreen)
		minscreen=len;
	if(len>bytesscreen)
		bytesscreen=len;
	return len;
	}
inline  struct databuf *getname(const struct databuf *const in ) const {
    return (struct databuf *)toscreen.getitem((const char *)in);
    }
inline  struct databuf *getmeaning(const struct databuf *const in) {
    return (struct databuf *)fromscreen.getitem((char *)in);
    }

//#define newbuf(name,namelen) struct databuf *name=(struct databuf *)malloc(sizeof(struct databuf)+namelen); name->buf=(unsigned char *)(name)+sizeof(struct databuf);name->len=namelen;

#define newbuf(name,namelen) struct databuf * tobuf(name,namelen)
#define tobuf(name,namelen) name=(struct databuf *)malloc(sizeof(struct databuf)+namelen); name->buf=(unsigned char *)(name)+sizeof(struct databuf);name->len=namelen;
#define adatabuf(name) newbuf(name,name##len);memcpy(name->buf,name##str,name##len);

#define putintable(table,from,to); tmp=(struct databuf **)table.itemptr((char *)from); *tmp=to;
int putconnection(struct databuf *name,struct databuf *meaning) {
  struct databuf *one=getmeaning(name);
 if(one) {
	if(meaning->len==one->len&&!memcmp(meaning->buf,one->buf,meaning->len))
		return -1;
 	one=getname(meaning);
	if(one&&name->len==one->len&&!memcmp(name->buf,one->buf,name->len))
		return -1;
	}
  struct databuf **tmp;
  putintable(toscreen,meaning,name);
  putintable(fromscreen,name,meaning);
  minscreen=(name->len<minscreen)?name->len:minscreen;
  minfile=(meaning->len<minfile)?meaning->len:minfile;

  bytesscreen=(name->len>bytesscreen)?name->len:bytesscreen;
  bytesfile=(meaning->len>bytesfile)?meaning->len:bytesfile;
    labelnr++;
 return labelnr;
  }
int putconnection(unsigned char *namestr,int namelen,unsigned char *meaningstr,int meaninglen) {
    adatabuf(name);
    adatabuf(meaning);
    if(putconnection(name,meaning)<0) {
		free(name);
		free(meaning);
		return -1;
		}
	return 0;
    };

  Translate(void):  Filter(1,1,filtername(Translate),filtername(Translate)),minscreen(0xffff), minfile(minscreen), labelnr(0),  conf(NULL), maxlen(0)
 {
	unknown.len=1;
	unknown.buf=new unsigned char[2] ;
    strcpy((char *)unknown.buf,"?");
    }
  Translate(int fb,int sb,const char *c) : Filter(fb,sb,filtername(Translate),filtername(Translate)),minscreen(0xffff), minfile(minscreen), labelnr(0),  conf(NULL), maxlen(0) {
	unknown.len=0;
    int errs=0;
    if((errs=setconvstr(c))<0)
      err=errs;
    }
void leeg(void) {
    minscreen=minfile=0xffff;
   bytesscreen=bytesfile=1;
	    fromscreen.leeg();
	    toscreen.leeg();
	    labelnr=0;
	}
void empty(Hash hash) {
  Hash::Deel *item,*oud;
  for(int i=0;i<hash.TABLESIZE;i++) {
    for(item=hash.tab[i];item;) {
        oud=item;
        item=item->next;
        free(oud->label);
        free(oud->plaats);
      free(oud);
      }
	hash.tab[i]=NULL;
   }
  }
void empty(void) {
    empty(toscreen);
    fromscreen.leeg();
   }

  ~Translate() {
    empty();
    free( toscreen.tab);
    free( fromscreen.tab);
    if(unknown.len)
      delete[] unknown.buf;
    if(maxlen>0)
      delete[] conf;
    }

/*
  Translate(Translate &rev): Filter(rev) {
    labelnr=0;
    minscreen=minfile=0xffff;
    label=name=filtername(Translate);
    unknown.len=0;
    char *conv=rev.convstr();  
    int errs=0;
    if((errs=setconvstr(conv))<0)
      err=errs;
    }
*/
//   int quot;                   /* Quotient.  */
 //   int rem;  
inline int transnumhex(int x) { 
  int res= x-((x<='9')?'0':('A'-10));
  return res;
  }
int fromhexstring(const char *conv,int len,unsigned char *buf) {
  const char *ptr=conv;
  int buflen=len/2;  
  for(int i=0;i<buflen;i++) {
    buf[i]=(transnumhex(*ptr)<<4)|transnumhex(ptr[1]);
    ptr+=2;
    }
  return buflen;
  }

//unknown:labelnr:
  int setconvstr(const char *conv) {
    const char *ptr=conv;  
    const char *end=strchr(ptr,':');
    if(!end)
      return -1;
    if(unknown.len)
      delete[] unknown.buf;
    int len=end-ptr;
    unknown.buf=new unsigned char[len/2+1];
    unknown.len= fromhexstring(ptr,len,unknown.buf);
    ptr=end+1;
    end=strchr(ptr,':');
    if(!end)
      return -1;
int32_t last;
    fromhexstring(ptr,end-ptr,(unsigned char *)&last);
    ptr=end+1;

    minscreen=minfile=0xFFFF;
	labelnr=0;
	int i;
    for(i=0;i<last;i++) {
      end=strchr(ptr,':');
      if(!end) {
        break;
	}
      int len=end-ptr;
      newbuf(name,len/2+1);
      name->len=fromhexstring(ptr,len,name->buf);
      ptr=end+1;
      end=strchr(ptr,',');
      if(!end) {
        if(i==(last-1))
          len=strlen(ptr);
        else 
          	break;
        }
      else
        len=end-ptr;
      newbuf(meaning,len/2+1);
      meaning->len=fromhexstring(ptr,len,meaning->buf);
      ptr=end+1;
      putconnection(name,meaning);
      }
labelnr=i;
if(labelnr!=last)
	fprintf(stderr,"Translate::setconvstr found only %d labels, not %d\n",labelnr,last);
 /*otherwise a copy seems not identical */
    return 0;
    }
inline  char *convstr(void) const  override{
		return constconvstr();
	}
  char *constconvstr(void) const  {
    int newlen=labelnr*((bytesscreen+bytesfile)*2+2)+sizeof(labelnr)*2+unknown.len*2+3;
    if(newlen>maxlen) {
      if(maxlen>0)
        delete[] conf;
      maxlen=newlen;
      conf=new char[maxlen];
      }
    char *ptr=conf;
    ptr=tohexstring(ptr,unknown.buf,unknown.len);
    *ptr++=':';
    ptr=tohexstring(ptr,(unsigned char *)&labelnr,sizeof(labelnr));
    *ptr++=':';
     toscreen.doortable((void *)&ptr,dooraddtostr);
/*
    Hash::Deel *item;
    for(int i=0;i<toscreen.TABLESIZE;i++)
      for(item=toscreen.tab[i];item;item= item->next) {
        ptr=addtostr(ptr,(struct databuf*)item->plaats,(struct databuf *)item->label);
        *ptr++=',';
        }
*/
    *ptr='\0';
    return conf;
    }
  virtual Filter *newone(void) const  override {
    char *conv=constconvstr();  
    return new Translate(bytesfile,bytesscreen,conv);
    }
  virtual int chbytes(int b)  override {
    return bytesscreen;
    }
  virtual int chbytesback(int b)  override {
    return bytesfile;
    }
  virtual int fromfile(const unsigned char * const input,int &len,unsigned char * const output) const  override {
    struct databuf *uit=NULL;
    memset(output,' ',bytesscreen);
    for(;len>=minfile;len--) {
	const	struct databuf in={len,const_cast<unsigned char *>(input)};
      uit =getname(&in);
      if(uit)  {
        memcpy(output,uit->buf,uit->len);
        break;
        }
      }
    if(!uit)  
      memcpy(output,unknown.buf,unknown.len);
    return bytesscreen;
    }
  virtual int tofile(const unsigned char * const input,int &inplen,unsigned char * const output)  override {
    struct databuf *uit=NULL;
    int len;  
    for(len=inplen;len>=minscreen;len--) {
const	struct databuf in={len,const_cast<unsigned char *>(input)};
      uit =getmeaning(&in);
      if(uit)  {
        memcpy(output,uit->buf,uit->len);
        break;
        }
      }
    for(;len<inplen&&isspace(input[len]);len++)
      ;
    if(len<inplen) 
      inplen=len-part->apart;
    return bytesfile;
    }
  int  equal(const Filter *fil) const override{
    if(!Filter::equal(fil))
      return 0;
    char *convthis=convstr(); /*Caution overwriten by new call*/
    char *convthat=fil->convstr();
    if(strcmp(convthis,convthat))
      return 0;
    return 1;
    }
  };
registerfilterclass(Translate) 




class Editor {
public:
#include "faked.h"
iscalled(translate)
iscalled(selectlabels)
/*
int translate(void) {
  ScreenPart *super=activemode(),*act;
  act=super->getsubpart();
  Translate *ad=new Translate();
  if(ad->error()) {
    delete ad;
    message("Error adding add");
    return -1;
    }
  addfilter(ad);
  toscreen();  
  }
*/

int starttranslate(void) {
  ScreenPart *sub=activemode()->getsubpart();
  return assignlabels(sub,0) ;
  }
int changetranslate(void) {
  ScreenPart *sub=activemode()->getsubpart();
  return assignlabels(sub,1) ;
  }
/*
struct databuf {
  int len;
  unsigned char *buf;
  };*/
int assignlabels(ScreenPart *part=NULL,int kind=0) ;


  };

  #define listlen 34
  struct transgegs {
    struct databuf *from,*to;
    char showstr[listlen+1];
    };

	struct addel{
		char **list;
		int nr;
		} ;

#define mkdatabuf(buf,len)
//struct addel *tra,struct databuf *label,struct databuf *plaats) {




#define tobufcp(name,namelen,stri) tobuf(name,namelen) ;memcpy(name->buf,stri,namelen);
void addtotrans(void *tra,void *label,void *plaats) {
	struct  transgegs *gegs=(struct transgegs *)malloc(sizeof(struct transgegs));
	struct databuf *to= (struct databuf *)label;
	struct databuf *from= (struct databuf *)plaats;

	tobufcp(gegs->to,to->len,to->buf);
	tobufcp(gegs->from,from->len,from->buf);


	struct addel *trans=(struct addel *)tra;
	trans->list[trans->nr++]=gegs->showstr;
	};
int strcompfunc(const void *one,const void *two) {	
	return strcmp(*((char **)one),*((char **)two));
	}
void gettransgegs(void *tra,void *label,void *plaats) {
	struct  transgegs *gegs=(struct transgegs *)malloc(sizeof(struct transgegs));
	struct databuf *to= (struct databuf *)label;
	struct databuf *from= (struct databuf *)plaats;

	gegs->to=to;
	gegs->from=from;

	struct addel *trans=(struct addel *)tra;
	trans->list[trans->nr++]=gegs->showstr;
	};
  #define strtostart(str) (struct transgegs *) ((char *)(str)-stroff)
int Editor::selectlabels(void) {
	ScreenPart *super= activemode();
  ScreenPart *part=super->getsubpart();
Translate *trans=NULL;
 for(int i=part->firstfilter;i<part->lastfilter;i++) {
      if(part->filters[i]->name==filtername(Translate)) {
	trans=(Translate*)part->filters[i];
	break;
	}
   }
  if(!trans) {
	message("Can't find translate filter");
	beep();return -1;
	}
DEBUG2(debug==1,"labelnr=%d\n",trans->labelnr)
char **translist=(char **)malloc(sizeof(char *)*trans->labelnr);
const int stroff=offsetof(struct transgegs, showstr);

	struct addel tran={translist,0};
	trans->fromscreen.doortable((void *)&tran,gettransgegs);
int	used=tran.nr;
	for(int i=0;i<used;i++) {
		struct transgegs *gegs=strtostart(translist[i]);
		int formtolen=((gegs->from->len+part->bytes)/part->bytes)*(part->maxres+part->apart)+1;
		char formto[formtolen+10];
		int takelen=maxnum(part->bytes,gegs->from->len);
		unsigned char bins[takelen+10];
		memset(bins,'\0',takelen);
		memcpy(bins,gegs->from->buf,gegs->from->len);
		int istolen=part->bin2search(bins,formto,takelen);
		formto[istolen]='\0';
		int len=minnum(istolen,listlen);
		memmove(gegs->showstr,formto,len);
		gegs->showstr[len]='\0';
		}
	qsort(translist,used,sizeof(char *),strcompfunc);
int ant;
 while((ant=selectitemindex("Select Label",translist,used,0))!=INT_MAX) {
     if(ant==(INT_MAX-1)) {
		resized();rewriteall();
		}
     else {
		if(ant>=0&&ant<used) {
			filesup();
			struct transgegs *gegs=strtostart(translist[ant]);
				int sbyt=super->getbytes();
			
				char  bin[sbyt+10];
				int edlen=(geteditpos()/sbyt)*sbyt;
				getmem(edlen+getfilepos(),bin,sbyt);
				char subbin[part->getbytes()];
				int inplen=gegs->to->len;
				int sublen=part->tofilefilters((char *)gegs->to->buf,inplen,subbin);
				addsubbin(super,part,(unsigned char *)bin,(unsigned char *)subbin,sublen) ;

				putmem(edlen+getfilepos(),bin,sbyt);
	free(translist);
			return 0;
			}
		else {
			filesup();
			beep();
			message("selectlabel: System Error %d",ant);
	free(translist);
			return -1;
			}
		}
	}
free(translist);
filesup();
return 0;

}
int Editor::assignlabels(ScreenPart *part,int kind) {

  ScreenPart *totype=NULL,*fromtype=NULL;
  Translate *trans=NULL;
const int maxunknown=512 ;
//#define maxunknown  512 
//#define maxunknown  4096 
  char unknown[maxunknown]="";
  unsigned char unknownbin[maxunknown];
int unknownbinlen=0,unknownlen=0;
const int stroff=offsetof(struct transgegs, showstr);
const int fromstrlen=256;
const int tostrlen=256;

  char fromstr[fromstrlen+1]="";
  char tostr[tostrlen+1]="";
	
  int wlin=16;
  int wcol=40;
  int separ=1,res=0;
  int ant=0,used=0,els;
  char **translist=NULL;
  if(part) {
	int byt=part->datbytes();
	 totype=part->newone();
	while(totype->rmlastfilter()>=0)
		;
	totype->screenbytes(byt);
	if(kind) {
	    for(int i=part->firstfilter;i<part->lastfilter;i++)
	      if(part->filters[i]->name==filtername(Translate)) {
		trans=(Translate*)part->filters[i];
		break;
		}
	if(!trans) {
		delete totype;
		message("Create new translate with starttranslate");beep();return -1;
		}
	   }
    }
else
	 totype=new Ascii(this);

fromtype=new Digit(this,16,1);
  unknownbinlen=0;
  memset(unknownbin,'\0',maxunknown);
//unknownbin[maxunknown-1]='\0';
  if(trans) {
	translist=(char **)malloc(sizeof(char *)*trans->labelnr);
	struct addel tran={translist,0};
	trans->fromscreen.doortable((void *)&tran,addtotrans);
    if(trans->unknown.len) {
      memcpy(unknownbin, trans->unknown.buf,trans->unknown.len);
	unknownbinlen=trans->unknown.len;
	}
	

	used=tran.nr;
	for(int i=0;i<used;i++) {
		struct transgegs *gegs=strtostart(translist[i]);
		int formtolen=((gegs->to->len+totype->bytes)/totype->bytes)*(totype->maxres+totype->apart)+1;
		char formto[formtolen];
		int takelen=maxnum(totype->bytes,gegs->to->len);
		int takefromlen=maxnum(fromtype->bytes,gegs->from->len);

		unsigned char bins[maxnum(takelen,takefromlen)];
		memset(bins,'\0',takelen);
		memcpy(bins,gegs->to->buf,gegs->to->len);
		int istolen=totype->bin2search(bins,formto,takelen);
		formto[istolen]='\0';

		int formfromlen=((gegs->from->len+fromtype->bytes)/fromtype->bytes)*(fromtype->maxres+fromtype->apart)+1;
		char formfrom[formfromlen];
		memset(bins,'\0',takelen);
		memcpy(bins,gegs->from->buf,gegs->from->len);
		int isfromlen=fromtype->bin2search(bins,formfrom,takefromlen);	
		formfrom[isfromlen]='\0';
		snprintf(gegs->showstr,listlen,"%s->%s",formfrom,formto);
		}
	

    }
else {
	if(kind)
		return -1;
	}
res=4;
NDEBUG("before while ")
while(1 ) {
	res--;
NDEBUG("before begindialog: unknownbinlen=%d, unknownlen=%d\n", unknownbinlen, unknownlen);
  begindialog(wlin,wcol,20);
    listall("Assi~gnments",1,2,translist,used,ant, ant,5,listlen);
    saveact("~Del", 1,33);
    saveact("Ty~pe", 8,2); 
mvwprintw(takescr,8,7,"%s-%d",fromtype->getname(),fromtype->getbytes());
linernrmax("~From",fromstr,8,18,13,fromstrlen);
    saveact("T~ype", 9,2);
mvwprintw(takescr,9,7,"%s-%d",totype->getname(),totype->getbytes());
    linernrmax("~To",tostr,9,20,13,tostrlen);
    saveact("~Add", 11,29);
  memset(unknown,'\0',totype->maxres+totype->apart);
int inbinlen=(unknownbinlen>0)?minnum(maxnum(unknownbinlen,totype->getbytes()),maxunknown):0;
NDEBUG("before bin2search: unknownbinlen=%d, inbinlen=%d,unknownlen=%d\n", unknownbinlen, inbinlen,unknownlen);
  unknownlen=totype->bin2search(unknownbin,unknown,inbinlen);
NDEBUG(" unknownbinlen=%d, inbinlen=%d,unknownlen=%d\n", unknownbinlen, inbinlen,unknownlen);
unknown[unknownlen]='\0';
    linernrmax("D~efault",unknown,11,2,8,maxunknown);
    oke(14,2);
    act("~Cancel",14,10);
    if(!kind) {;
      optselect(separ,separ, 
        opt("~Separate",13,24,0),
        opt("F~ield",14,24,1));
      els=2;
      }
    else
      els=0;
    if(res<0||res>(9+els))
		res=4;
  enddialog(res,res);
  unknownlen=strlen(unknown);
  unknownbinlen=totype->search2bin(unknown,unknownbin,unknownlen);
NDEBUG("after enddialog: unknownbinlen=%d, unknownlen=%d\n", unknownbinlen, unknownlen);

  switch(res) {
    case 1:
		if(ant>=0&&ant<used) {
			struct transgegs *gegs=strtostart(translist[ant]);
			int formtolen=((gegs->to->len+totype->bytes)/totype->bytes)*(totype->maxres+totype->apart)+1;
			char formto[formtolen];
			int takelen=maxnum(totype->bytes,gegs->to->len);
			int takefromlen=maxnum(fromtype->bytes,gegs->from->len);

			unsigned char bins[maxnum(takelen,takefromlen)];
			memset(bins,'\0',takelen);
			memcpy(bins,gegs->to->buf,gegs->to->len);
			int istolen=totype->bin2search(bins,formto,takelen);

			int formfromlen=((gegs->from->len+fromtype->bytes)/fromtype->bytes)*(fromtype->maxres+fromtype->apart)+1;
			char formfrom[formfromlen];
			memset(bins,'\0',takelen);
			memcpy(bins,gegs->from->buf,gegs->from->len);
			int isfromlen=fromtype->bin2search(bins,formfrom,takefromlen);	

			int tlen=minnum(isfromlen,fromstrlen-1);
			memcpy(fromstr, formfrom,tlen);
			fromstr[tlen]='\0';
			tlen =minnum(istolen,tostrlen-1);
			memcpy(tostr, formto,tlen);
			tostr[tlen]='\0';
			}
		else {
			beep();
			message("System error: %d not >=0 and <%d",ant,used);erefresh();break;
	
			}
		break;
    case 2: {
		if(ant>=0&&ant<used) {
			struct transgegs *gegs=strtostart(translist[ant]);
			free(gegs->to);
			free(gegs->from);
			free(gegs);
			used--;
			if(ant<used)
				memmove(translist+ant,translist+ant+1,(used-ant)*sizeof(char *));
			else
				ant--;
			}
		else {
			beep();
			message("System error: %d not >=0 and <%d",ant,used);erefresh();break;
	
			}
		break;
		}
    case 3: {
	ScreenPart *tmp=asktype(this);
	filesup();
	if(!tmp) {
		message("Cancelled");break;
		}
	tmp->userconfig(1);
	tmp->calcsizes();
	if(fromtype)
		delete fromtype;
	fromtype=tmp;
	break;
	}
    case 5: {
	ScreenPart *tmp=asktype(this);
	filesup();
	if(!tmp) {
		message("Cancelled");break;
		}
	tmp->userconfig(1);
	tmp->calcsizes();
	if(totype)
		delete totype;
	totype=tmp;
	break;
	}
    case 9: {
{
		filesup();
		  Translate *ad=new Translate();
		
		  if(!ad||ad->error()) {
		  	message("Error creating Translate");break;
		  	}

		for(int i=0;i<used;i++) {
			struct transgegs *gegs=strtostart(translist[i]);
			ad->putconnection(gegs->to,gegs->from);
			if(ad->error()) {
				message("Error adding %s",translist[i]);
				delete ad;
				return -1;;
				}
			}
		for(int i=0;i<used;i++) {
			struct transgegs *gegs=strtostart(translist[i]);
			free(gegs);
			}
		if(translist)
			free(translist);


NDEBUG("before addunknown: unknownbinlen=%d, unknownlen=%d ", unknownbinlen, unknownlen);
		ad->addunknown(unknownbin,unknownbinlen);
NDEBUG("after addunknown: unknownbinlen=%d, unknownlen=%d\n", unknownbinlen, unknownlen);
		if(!trans) {	
			totype->addfilter(ad);
			totype->chbytes(ad->bytesscreen);
			if(installpart(totype,separ)==-1) {	
				delete totype;	
				message("Adding  failt");	
				return -1;	
				}	
			}
		else {
			totype->copyfiltersafter(part);
			trans=NULL;
			for(int i=totype->firstfilter;i<totype->lastfilter;i++) {
			      if(totype->filters[i]->name==filtername(Translate)) {
				delete totype->filters[i];
				totype->filters[i]=ad;
				ad->part=totype;
				break;
				}
				}
			ScreenPart *par;
			if((par=part->getparent())) {
				HasSub *has=(HasSub *)par;
				if((has->setsubpart(totype,has->activesubpart()))!=part) {
					message("System Error: replaces wrong datatype");
					}
				}
			else {
				if(replacepart(totype)!=part) {
					message("System Error: replaces wrong datatype");
					}

				}
			totype->calcsizes();
			activemode()->calcsizes();
			delete part;
		

			}
		return 0;
		}
		};
	case 0:;
    case 10: {
		filesup();
		for(int i=0;i<used;i++) {
			struct transgegs *gegs=strtostart(translist[i]);
			free(gegs->from);
			free(gegs->to);
			free(gegs);
			}
		if(translist)
			free(translist);
		};return -1;
    case 4:;
    case 6:;
    case 7: {
	      if(fromstr[0]!='\0'&&tostr[0]!='\0')  {
		int fromunit=fromtype->maxres+fromtype->apart;
		int fromlen=strlen(fromstr);
		int frombinlen=((fromlen+fromunit-1)/fromunit)*fromtype->bytes;
		unsigned char frombin[frombinlen*2+40];
		int isfrombin=fromtype->search2bin(fromstr,frombin,fromlen);


		int tounit=totype->maxres+totype->apart;
		int tolen=strlen(tostr);
		int tobinlen=((tolen+tounit-1)/tounit)*totype->bytes;
		unsigned char tobin[tobinlen*2+40];
		int istobin=totype->search2bin(tostr,tobin,tolen);

		struct transgegs *gegs;
		gegs=(struct transgegs *)malloc(sizeof(struct transgegs));
		translist=(char **)realloc(translist,(used+1)*sizeof(char *));
		translist[used++]=gegs->showstr;

		tobufcp(gegs->from,isfrombin,frombin) ;
		tobufcp(gegs->to,istobin,tobin) ;
		snprintf(translist[used-1],listlen,"%s->%s",fromstr,tostr);
		ant=used-1;
		  }
          };break;
	default:;
	case (INT_MAX-1):
		res=4;
		resized();
		rewriteall();
		break;
         }
	}
return 0;
  }
int Editor::translate(void) {
while(1) {
int res=2;
  begindialog(12,12,6);
	mvwaddstr(takescr,1,2,"Translate");
    saveact("~New", 3,2);
    saveact("~Change", 5,2); 
    saveact("~Use", 7,2); 
    saveact("C~ancel", 9,2); 
  enddialog(res,res);
   filesup();
	switch(res) {
	case (INT_MAX-1):resized();rewriteall();break;
	  case 1:  return starttranslate();break;
	  case 2: return changetranslate();break;
	case 3:  return selectlabels();break;
	case 0:;
	case 4:return -1;
	};
  };
return -1;
 }

call(translate)
call(selectlabels)
call(starttranslate)
call(changetranslate)
