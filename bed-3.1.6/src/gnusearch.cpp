#include "desturbe.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "editor.h"
#include "main.h"
#include "system.h"
#include "screenpart.h"
#include "config.h"
extern const char *regexerrorstring;
/*
 * Uses re_search_2 contained in regex-0.12 originated by Richard Stallman.
 * Don't use re_search_2 contained in glibc-2.26.  It tries to make old sources compile by copying both arrays to force it in some other function.
 */
//OFFTYPE Editor::GNUformsearch(ScreenPart *datashown,OFFTYPE from,char *ant,short len,int *matchlen) {
OFFTYPE Editor::GNUformsearch(SearchAim &aim) {
	const int len=aim.len;
	if(len<=0)
		return OFFTYPEINVALID;
INCDEST();
	if(setsearchgegs(aim )<0) {
		message("failure");
		DECDEST();
		return OFFTYPEINVALID;
		}

	DECDEST();

	searchbytes=parts[mode]->bytes;
	searchgrens=(filepos+indatatype)%searchbytes;

	return nextGNUregexshown(aim);
	}



//#define onboundary ((mem.lastforward&8)?1:0)
#define onboundary Stype.Align

#define block(bl) (binbuf+(((bl)&1)*blocksize))
#define form(n) (formbuf+n*formsize)

#include "misc.h"

//#define onunit ((mem.lastforward&32)?1:0)
#define onunit Stype.Unit
#undef MEMORY
#define MEMORY 
#include "searchinterrupt.h"

//#define islist (mem.lastforward&0x10)
#define islist Stype.List

//inline int re2match(const char *data,const int len,int *offset) {
/*

template <typename T> class savedyn {
	T **addr;
	T *ptr=NULL;
	int *len;
	public:
	savedyn(T **a,int *l): addr(a),len(l) {}
	savedyn() = delete;
	savedyn(const savedyn &x) = delete; 
void	save(void) {
		del();
		ptr=*addr;
		*addr=(T *)malloc(sizeof(T)*(*len));
		};
void putback(void) {
	::free(*addr);
	*addr=ptr;
	ptr=NULL;
	}

void del(void) {
	if(ptr)
		::free(ptr);
	}
	~savedyn() {
		del();
	}
}
*/
class saveregs {
	private:
	Editor *edp;
	regoff_t	*start=NULL;
	regoff_t	*end=NULL;


	public:
	saveregs() = delete;
	saveregs(const saveregs &x) = delete; 
	saveregs(Editor *e): edp(e) {};
void	save(void) {
	del();
	start=edp->searchregs->start;
	end=edp->searchregs->end;
	edp->searchregs->start=(regoff_t *)malloc(sizeof(regoff_t)*edp->searchregs->num_regs);
	edp->searchregs->end=(regoff_t *)malloc(sizeof(regoff_t)*edp->searchregs->num_regs);
	}
void	putback(void) {
	::free(edp->searchregs->start);
	::free(edp->searchregs->end);
	edp->searchregs->start= start;start=NULL;
	edp->searchregs->end= end;end=NULL;
	}
void del(void) {
	if(start)
		::free(start);
	if(end)
		::free(end);
	}
	~saveregs() {
		del();
	}
};
//OFFTYPE Editor::nextGNUregexshown(ScreenPart *datashown,int *matchlen) {
OFFTYPE Editor::nextGNUregexshown(SearchAim &aim) {
	ScreenPart *datashown=aim.datashown;
	if(!searchexpr) {
			return OFFTYPEINVALID;
		}
	if(onunit) {
		OFFTYPE pos=nextsingleregex(datashown,&Editor::GNUhit) ;
		aim.matchlen=searchbytes;
		return pos;
		}
	int bytes=datashown->bytes;
	int bs= mem.BLOCKSIZE;
	long items=(bs/bytes);
	int ellen=datashown->maxres+datashown->apart;
	const int blnr=2,blnr1=blnr-1;
	int formsize=((items+1)*ellen);
	short boundaries;
	OFFTYPE bytestart;
	bool list=islist;
	if(onboundary) {
		bytestart=(searchpos/searchbytes)*searchbytes+searchgrens;
		boundaries=1;
		}
	else {
		bytestart= searchpos;
		boundaries=bytes;
		}

	if(bytestart>=filesize())
			return OFFTYPEINVALID;
	BLOCKTYPE bl=bytestart/bs;
	int inblock=bytestart%bs;
         char formbuf[(boundaries+1)*formsize];
	 char *prev[boundaries];
	 int prevlen[boundaries];
	 int binpos[boundaries];
	 OFFTYPE binstart[boundaries];
	 char *formover=formbuf+boundaries*formsize;


extern unsigned long long startuptime;
SEARCHINTDEC(6000000000/startuptime,blnr1,bl+blnr)

         char *binbuf=alignedalloc(bs,blnr*bs);

	int took=mem.getblocks(bl,binbuf,blnr);
char *binrest=binbuf+bs;
	const int firstb=minnum(bs,took);
	for(int shift=0;shift<boundaries;shift++) {
		const int takelen=((firstb-shift-inblock)/bytes)*bytes;

		char *const formhier= formbuf+shift*formsize;
		const int  inbin= shift+inblock;
		prevlen[shift]=datashown->bin2search((unsigned char *)binbuf+inbin,formhier,takelen);
		prev[shift]=formhier;

		binstart[shift]=bl*bs+inbin;
		binpos[shift]=takelen+inbin;
		}

	bl+=blnr;
	OFFTYPE first=OFFTYPEINVALID;
	saveregs regs(this);
	while(took>0) {
	for(int shift=0;shift<boundaries;shift++) {
//		const int len=((totlen-binpos[shift])/bytes)*bytes;
		const int len=((took-binpos[shift])/bytes)*bytes;
		int count;
		if(len<bytes) 
			count=0;
		else
			count=datashown->bin2search((unsigned char*)binbuf+binpos[shift],formover,len);
		const int len1= prevlen[shift],ends=len1-1;
			for(int pos=0;pos<len1&&(pos=re_search_2(searchexpr,prev[shift] ,len1 , formover, count, pos, ends-pos,searchregs,len1+count))!=-1;pos+=ellen) {
				if(pos<0) {
					message("re_search_2 at " OFFPRINT " returned %d",binstart[shift],pos);
					break;
					}
				
//					OFFTYPE oppos=(((pos+datashown->apart)/ellen)*bytes)+binstart[shift];
					OFFTYPE oppos=((pos/ellen)*bytes)+binstart[shift];
					if(list)  {
						const int mlen=searchregs->end[0]-searchregs->start[0];
						addmark(oppos,((mlen-1)/ellen+1)*bytes );
						}
								
					else {
						if(oppos<first) {
							regs.save();


							first=oppos;
							};
						goto NEXTSHIFT;
						}	       

					 };
			if(len<bytes)
						goto NEXTSHIFT;
			binstart[shift]=bs*(bl-blnr)+binpos[shift];
			binpos[shift]+=(len-blnr1*bs);
			myswap(formover,prev[shift]);
			prevlen[shift]=count;
			
NEXTSHIFT:
		;

		}
	SEARCHINTER(bl,alignedfree(binbuf))
	if(first!=OFFTYPEINVALID) {
			alignedfree(binbuf);
			searchpos=first;
			regs.putback();
			const int mlen=searchregs->end[0]-searchregs->start[0];
			aim.matchlen=((mlen-1)/ellen+1)*bytes;
			if(onboundary) {
				return (searchpos+=searchbytes,searchpos-searchbytes);
				}
			else
				return searchpos++;
			}
	if(took<(blnr1*bs))
		break;
	memcpy(binrest-bytes,binbuf+bs*blnr-bytes,bytes);
	int bij=mem.getblocks(bl,binrest,blnr1);
	took+=(bij-bs);
	searchpos=bs*bl; //For Not searched
	bl+=blnr1;
	}
alignedfree(binbuf);
	return OFFTYPEINVALID;
	}
/*
extern int re_search_2
  _RE_ARGS ((struct re_pattern_buffer *buffer, const char *string1,
             int length1, const char *string2, int length2,
             int start, int range, struct re_registers *regs, int stop));
van start naar start+range
*/

#include <assert.h>

//OFFTYPE Editor::nextbackshown(ScreenPart *datashown,int *matchlen) {
OFFTYPE Editor::nextbackshown(SearchAim &aim) {
	ScreenPart *datashown=aim.datashown;
	if(!searchexpr) {
	    regexerrorstring=reinterpret_cast<const char *>(-1);
	    message("No previous search");
		return OFFTYPEINVALID;
		}
	OFFTYPE bytestart;
	const int bytes=datashown->bytes;
	short boundaries;
	if(onboundary) {
		bytestart=((searchpos+1)/searchbytes)*searchbytes+searchgrens;
		boundaries=1;
		}
	else {
		bytestart= searchpos+1;
		boundaries=bytes;
		}
	if(bytestart<bytes)
			return OFFTYPEINVALID;
	const int bs=blocksize();
	const int items=bs/bytes;
	const int ellen=datashown->maxres+datashown->apart;
	const int formsize=((items+1)*ellen);
	char  formbuf[(boundaries+1)*formsize];
	const int blnr=2,blnr1=blnr-1;
	BLOCKTYPE bl=bytestart/bs;
	const int inblock=bytestart%bs;
extern unsigned long long startuptime;
SEARCHINTDEC(0x200000000/startuptime,blnr1,bl)
	char *const binbuf=(char *)alignedalloc(bs,bs*blnr);
	int took=mem.getblocks(bl,binbuf,blnr1),take=0;
	assert(took>=take);
	int formlen[boundaries];
	char *forms[boundaries];
	int endbin[boundaries];
	for(auto shift=0;shift<boundaries;shift++) {
		char *const formhier= formbuf+shift*formsize;
		formlen[shift]=0;
		forms[shift]=formhier;
		endbin[shift]=inblock-shift;
		}

	bool list=islist;
	char *formover=formbuf+boundaries*formsize;
	auto first=OFFTYPEINVALID;
	saveregs regs(this);
	do {
		for(auto shift=0;shift<boundaries;shift++) {
			const int binend= endbin[shift];
			const int takelen=(binend/bytes)*bytes;
			const auto start=binend-takelen;
			if(takelen>0) {
				const auto formlen1=datashown->bin2search((unsigned char*)binbuf+start,formover,takelen);
				const auto formlen2=formlen[shift];
				for(int pos=formlen1-1;pos>=0&&(pos=re_search_2(searchexpr,formover,formlen1, forms[shift],formlen2,pos , -pos, searchregs, formlen1+formlen2))!=-1;pos-=ellen){ 
					if(pos==-2) { message("re_search_2 failed: %d",pos);

						alignedfree(binbuf);
						return OFFTYPEINVALID;
						}
		//			OFFTYPE oppos=(((pos+datashown->apart)/ellen)*bytes)+start+bl*bs;
					OFFTYPE oppos=((pos/ellen)*bytes)+start+bl*bs;
					if(list) {
						int const mlen=searchregs->end[0]-searchregs->start[0];
						addmark(oppos, ((mlen-1)/ellen+1)*bytes);
						}
					else {
						if(oppos>first||first==OFFTYPEINVALID) { //Maybe I make OFFTYPE unsigned
							regs.save();
							first=oppos;
							};
						goto NEXTSHIFT;
						}	       

					}
				formlen[shift]=formlen1;
				}
			else
				formlen[shift]=0;
			myswap(formover,forms[shift]); 
			endbin[shift]=blnr1*bs+start;
NEXTSHIFT:
		;
			}
//	signalwant({alignedfree(binbuf);regs.del();mem.searchpos=bl*bs+bytes-1;},OFFTYPEINVALID) ; /*Already interruptable by SEARCHINTER */
	SEARCHINTER(bl,{alignedfree(binbuf);searchpos=(bl+blnr1)*bs+bytes-1;})
	if(first!=OFFTYPEINVALID) {
			alignedfree(binbuf);
			regs.putback();
			int const mlen=searchregs->end[0]-searchregs->start[0];
			aim.matchlen=((mlen-1)/ellen+1)*bytes;
			if(onboundary) {
				searchpos=first-searchbytes;;
				return first;
				}
			else {
				searchpos=first-1;
				return first;
				}
			}
	if(bl<=0)
		break;
	memcpy(binbuf+blnr1*bs,binbuf,bytes);
	searchpos=bl*bs;
	bl-=blnr1;
	took=mem.getblocks(bl,binbuf,blnr1);
	take=took+bytes-1;
	} while(take>0);
alignedfree(binbuf);
return OFFTYPEINVALID;
}

//OFFTYPE Editor::backshownsearch(ScreenPart *datashown,OFFTYPE from,char *ant,short len,int *matchlen) {
OFFTYPE Editor::backshownsearch(SearchAim &aim) {
	const int len=aim.len;
	if(len<=0)
		return OFFTYPEINVALID;
INCDEST();
	if(setsearchgegs(aim)<0 ) {
		message("failure");
		DECDEST();
		return OFFTYPEINVALID;
		}

	DECDEST();

	searchbytes=parts[mode]->bytes;
	searchgrens=(filepos+indatatype)%searchbytes;

	return nextbackshown(aim);
	}
