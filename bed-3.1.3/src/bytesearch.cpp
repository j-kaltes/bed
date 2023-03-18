#include "desturbe.h"
#include <features.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include "editor.h"
#include "system.h"
#include "myalloc.h"
#include "main.h"
#ifdef USE_MMAP
const long mmap_page_size = (long) sysconf (_SC_PAGESIZE);
#define	FREEMMAP(str, len) {if(nommap){ databuffree(str);}else {MUNMAP(str,len);}}
#define BIGGERMMAPBLOCK
#endif
#if defined(USE_MMAP)&&!defined(BIGGERMMAPBLOCK)
int STBLOCKSIZE=mmap_page_size;

#else
//int STBLOCKSIZE=4096;
#ifdef _WIN32
int STBLOCKSIZE=2048;
#else
int STBLOCKSIZE=4096;
#endif
#endif
#if defined(BIGGERMMAPBLOCK) && defined(USE_MMAP)
#define MMAPBLOCKSIZE (mmap_page_size*4L)
#define MMAPBLOCKTYPE BLOCKTYPE
#else
#define MMAPBLOCKSIZE blocksize()
#define MMAPBLOCKTYPE BLOCKTYPE
#endif /*BIGGERMMAPBLOCK */
#ifdef USE_MMAP
/*
Uses a other MMAPBLOCKSIZE larger than the general blocksize.
blocknr is given in terms of MMAPBLOCKSIZE

*/
BEDS_SIZE_T Memory::mmapgetblock(MMAPBLOCKTYPE blocknr,char *buf) {

	BEDS_SIZE_T takesize,disktakesize;
	OFFTYPE offset=(blocknr*MMAPBLOCKSIZE);
	if(offset>=filesize)
		return 0;
	caddr_t ptr;
	takesize=minnum(MMAPBLOCKSIZE,filesize-offset);
	disktakesize=minnum(MMAPBLOCKSIZE,useddiskfilesize-offset);
	if(mmapfailed||(disktakesize>0&&((ptr=(caddr_t)MMAP(buf, disktakesize , PROT_READ|PROT_WRITE ,  MAP_PRIVATE|MAP_FIXED , filehandle, offset))== MAP_FAILED ))) {
		mmapfailed=1;
		int len=MMAPBLOCKSIZE/ BLOCKSIZE;
		BLOCKTYPE start=blocknr*len;
		takesize=0;
		for(int iter=0;iter<len;iter++) {
			takesize+=getblock(iter+start, buf+(BLOCKSIZE*iter));
			}
		return takesize;
		}
	else {
		int len=MMAPBLOCKSIZE/ BLOCKSIZE;
		BLOCKTYPE start=blocknr*len;
		for(int iter=0;iter<len;iter++) {
			Treel *el=tree.search(iter+start);
			if(el!=tree.NIL) {
				memcpy(buf+(BLOCKSIZE*iter),el->data.buf,el->data.size);
				}
			}
		return takesize;
		}

	}
#else

inline BEDS_SIZE_T Memory::mmapgetblock(BLOCKTYPE blocknr,char *buf) {return getblock(blocknr,buf);}
#endif
#ifndef USE_MMAP
	#undef FREEMMAP
	#define FREEMMAP(buf,y) databuffree(buf)
	#undef MMAP
	#define MMAP(take,len ,prot  ,flag, file, off) databufalloc(3*blocksize())
	#define mmapgetblocker(bl) ( mem.getblock(bl,(((char *)buf+(bl&1)*blocksize()))))
	#define STARTMAPOFF 1
#else
	#define mmapgetblocker(bl) ( mmapgetblock(bl,(((char *)buf+(bl&1)*MMAPBLOCKSIZE))))
	#define STARTMAPOFF 0
#endif

#ifndef BIGGERMMAPBLOCK
	#define  TAKEBLOCKS 3
#else
	#define  TAKEBLOCKS 2
#endif

#include "misc.h"
#define ablock(blo)	buf+blocksize()*(blo)
#define EXTRAMMAP 0
#define databuffree(x) alignedfree(x)
#define databufalloc(size) (char *)alignedalloc(blocksize(), size)
//#if defined(_WIN32) && !defined(USETHREADS)
#undef MEMORY
#define MEMORY
#include "searchinterrupt.h"






inline int nochange(const int ch)  {return ch;};
//strncasecmp,(const char *, const char *, size_t));
inline int iunequal(const void *onev,const void *twov,size_t len) {
	return unequal<mklow>(onev,twov,len);
	}

/*
endblock: laatste begin positie in block -1
ending: laatste begin positie van search waarvan het eind in dit bloc zit
endstart: laastste start positie waarvn het eind in dit block zit
*/

template <Memcmp_t amemcmp,ChTrans givelow> inline OFFTYPE Editor::nextforwardsearch(SearchAim &aim) {
extern unsigned long long startuptime;
SEARCHINTDEC(((0xF000000000/startuptime)*blocksize())/MMAPBLOCKSIZE )



#ifdef USE_MMAP
	int nommap=STARTMAPOFF;
#endif

        BEDS_SIZE_T DBUFSIZE=2*MMAPBLOCKSIZE, count;
        signed char *start,*endstart,*buf;
	BLOCKTYPE bl;
        int i,bij=0;
	OFFTYPE endblock,ending;

#ifdef USE_MMAP
	if(mmapfailed||(buf=(signed char *)MMAP(0, TAKEBLOCKS*MMAPBLOCKSIZE+EXTRAMMAP,PROT_WRITE|PROT_READ  ,MAP_PRIVATE| MAP_ANON, -1, 0))==MAP_FAILED) {
		mmapfailed=1;
		nommap=1;
#endif
//		buf=(signed char *)malloc(TAKEBLOCKS*MMAPBLOCKSIZE+EXTRAMMAP);
		buf=(signed char *)databufalloc(TAKEBLOCKS*MMAPBLOCKSIZE+EXTRAMMAP);
#ifdef USE_MMAP
		}
#endif

	int endstr=searchlen-1;
        for(bl=searchpos/MMAPBLOCKSIZE,count=mmapgetblocker(bl),endblock=bl*MMAPBLOCKSIZE;	count>0;) {
                endblock+=count;
		bl++;
		count=mmapgetblocker(bl);
		endblock+=minnum(0,(count-endstr));
		ending=endblock-endstr;
                for(start=buf+(searchpos%DBUFSIZE),endstart=start+(ending-searchpos);start<endstart;) {
                		if(table[start[endstr]]) {
                			if((bij=table[start[endstr]])==searchlen) {
						if(!amemcmp(searchstr,start++,searchlen)) {
							searchpos=(ending-(endstart-start+1));
							if(Stype.List)
								addmark(searchpos, searchlen);
							else {
								FREEMMAP(buf, TAKEBLOCKS*MMAPBLOCKSIZE);
								return searchpos++;
								}
							}
						}
					else
						start+=(searchlen-bij);

					}
				else
					start+=searchlen;
                                }

                for(searchpos=(ending-(endstart-start));searchpos<endblock;) {
                		if(table[buf[(searchpos+endstr)%(DBUFSIZE)]]) {
					if((bij=table[buf[(searchpos+endstr)%(DBUFSIZE)]])==searchlen) {
						for(i=0;(unsigned char)searchstr[i]==(unsigned char)givelow(buf[(searchpos+i)%(DBUFSIZE)]);i++) 
								if(i==endstr) {
									if(Stype.List)
										addmark(searchpos, searchlen);
									else {
										FREEMMAP(buf, TAKEBLOCKS*MMAPBLOCKSIZE);
										return searchpos++;
										}
									}
						searchpos++;
						}
					else
						searchpos+=(searchlen-bij)	;
					}
				else
					searchpos+=searchlen;
				}
                SEARCHINTER(bl,FREEMMAP(buf, TAKEBLOCKS*MMAPBLOCKSIZE));
		}
	FREEMMAP(buf, TAKEBLOCKS*MMAPBLOCKSIZE);
	return OFFTYPEINVALID;
	}

OFFTYPE Editor::nextforwardicasesearch(SearchAim &aim) {
return nextforwardsearch<unequal<mklow>,mklow>(aim) ;
}
//static_cast <type-id> ( expression ) 

OFFTYPE Editor::nextforwardsearch(SearchAim &aim) {
//return nextforwardsearch<unequal<nochange>,nochange>() ;
return nextforwardsearch<memcmp,nochange>(aim) ;
}

//OFFTYPE Editor::nextsearch(int *matchlen,bool sense) {
///
#if 0
OFFTYPE Editor::nextsearch(SearchAim &aim) {
	if(!searchstr)
		return OFFTYPEINVALID;
// *matchlen=searchlen;
#ifdef DONTINTCURSES 
DECPDDEST();
#endif
	OFFTYPE ret= OFFTYPEINVALID;
	if(Stype.Forward)  {
		if(aim.sense) 
		 ret= nextforwardsearch(aim);
		else
		 ret= nextforwardicasesearch(aim);

		}
	else {
		if(Stype.Regex)  {
			 ret= nextbackregexsearch(aim);
			getmatchlen
			}
		else {
			if(aim.sense) 
			 ret= nextbacksearch(aim);
			else
			 ret= nextbackicasesearch(aim);
			}
		};
#ifdef DONTINTCURSES 
INCPDDEST();
#endif
	return ret;
	}
#endif
//OFFTYPE Editor::icasesearch(OFFTYPE beg,signed char *str,int len) {
OFFTYPE Editor::icasesearch(SearchAim &aim) {

const	 signed char *const str=aim.str;
const	 int len=aim.len;
	memset(tablebase,0,sizeof(tablebase));
	setsearchstr<memlowcpy>(aim);
       	for(int i=0;i<len;i++) {
       		signed char ch=str[i];
		table[ch]=(char)i+1;
       		if(isup(ch)) {
       			ch=tolow(ch);
	//		searchstr[i]=ch;
			table[ch]=(char)i+1;
			}
		else {
	//		searchstr[i]=ch;
			if(islow(ch))
				table[toup(ch)]=i+1;
			}
       		}
	searchpos=aim.start;
	return nextforwardicasesearch(aim); 
	}

#include "screenpart.h"
OFFTYPE Editor::casesearch(SearchAim &aim) {
	searchpos=aim.start;
	const signed char *const str=aim.str;
	const int len=aim.len;
	setsearchstr(aim);
	int i;
	memset(tablebase,0,sizeof(tablebase));
       	for(i=0;i<len;i++) 
       		table[str[i]]=i+1;
	return nextforwardsearch(aim); 
	}
//OFFTYPE Editor::backsearch(OFFTYPE beg,signed char *str,int len) {
OFFTYPE Editor::backsearch(SearchAim &aim) {
	const signed char *const str=aim.str;
	const int len=aim.len;
	int i;
	for(i=SCHAR_MIN;i<=SCHAR_MAX;i++)
		table[i]=-1;
       	for(i=len-1;i>=0;i--) 
       		table[str[i]]=i;
       setsearchstr(aim) ;
	searchpos=aim.start;
	return nextbacksearch(aim); 
	}



//OFFTYPE Editor::backicasesearch(OFFTYPE beg,signed char *str,int len) {
OFFTYPE Editor::backicasesearch(SearchAim &aim) {
	const signed char *const str=aim.str;
	const int len=aim.len;
	int i;
	for(i=SCHAR_MIN;i<=SCHAR_MAX;i++)
		table[i]=-1;
       	for(i=len-1;i>=0;i--)  {
       		table[mklow(str[i])]=i;
       		table[mkup(str[i])]=i;
       		}
	setsearchstr<memlowcpy>(aim);
	searchpos=aim.start;
	return nextbackicasesearch(aim); 
	}

OFFTYPE Editor::nextbackicasesearch(SearchAim &aim) {
return nextbacksearch<iunequal>(aim) ;
//return nextbacksearch<unequal<mklow>>() ;
}
OFFTYPE Editor::nextbacksearch(SearchAim &aim) {
return nextbacksearch<memcmp>(aim) ;
//return nextbacksearch<unequal<nochange>>() ; 
/*
Some test comparisons seemed to show that this one is faster then memcmp, but I didn't take a representative sample of the kind of comparisons done with this procedure in the reference population. The ethics commission will probably not agree with collecting data from users without their knowledge and sending it to my computer.
*/
}

#define ablock(blo)	buf+blocksize()*(blo)
template <Memcmp_t amemcmp> inline OFFTYPE Editor::nextbacksearch(SearchAim &aim) {
extern unsigned long long startuptime;
SEARCHINTDEC(0x4000000000/startuptime )
//        signed char buf[3][blocksize()];
//        signed char *buf=(signed char *)databufalloc(3*blocksize());
#ifdef USE_ALIGNAS /*Doesn't work blocksize() has to be constand*/
#undef databuffree
#define databuffree(x)
        alignas(blocksize()) signed char buf[2*blocksize()];
#else
        signed char *buf=(signed char *)databufalloc(2*blocksize());
#endif
        signed char *start,*vorig,*huidig;
	BLOCKTYPE bl;
        int bij,hierin;
	BEDS_SIZE_T count=searchlen;
//	int endstr=searchlen-1;

	bl=((searchpos+searchlen)/blocksize());
	hierin=((searchpos+searchlen)%blocksize());

	vorig=ablock(bl&1);
	if(((count=mem.getblock(bl,(char *)vorig))>0)&&(hierin>=searchlen)) {
			for(start=vorig+hierin-searchlen;start>=vorig;) {
					if(table[*start]<0)
						start-=searchlen;
					else  {
						if((bij=table[*start])) 
							start-=bij;
						else {
							if(!amemcmp(searchstr,start--,searchlen)) {
								searchpos=bl*blocksize()+(start-vorig+1);
								if(Stype.List)
									addmark(searchpos, searchlen);
								else  {
									databuffree(buf);
									return searchpos--;
									}
								}
							}

						}
					}
		}
	hierin=maxnum(1,searchlen-hierin);

        for(bl-- ;(bl>=0&&(huidig=ablock(bl&1),((count=mem.getblock(bl,(char *)huidig))>0)));bl--,vorig=huidig) {
		for(start=huidig+blocksize();hierin<searchlen;) {
			if(table[*(start-hierin)]<0)
				hierin+=searchlen;
			else  {
				if((bij=table[*(start-hierin)])) 
					hierin+=bij;
				else {
					if(!(amemcmp(searchstr+hierin,vorig,(searchlen-hierin))||amemcmp(searchstr,start-hierin,hierin))) {
									searchpos=bl*blocksize()+blocksize()-hierin;
									if(Stype.List)
										addmark(searchpos, searchlen);
									else {
										databuffree(buf);
										return searchpos--;
										}
									}
					hierin++;
					}
				}
			}
		start-=hierin;
		hierin=1;

                for(;start>=huidig;) {
					if(table[*start]<0)
						start-=searchlen;
					else  {
						if((bij=table[*start])) 
							start-=bij;
						else {
							if(!amemcmp(searchstr,start--,searchlen)) {
								searchpos=bl*blocksize()+(start-huidig+1);
								if(Stype.List)
									addmark(searchpos, searchlen);
								else {
									databuffree(buf);
									return searchpos--;
									}
								}
							}

						}

                                }
		searchpos=bl*blocksize();
	 SEARCHINTER(bl,databuffree(buf));
		}
	return OFFTYPEINVALID;

	}

/*
template <uint8_t>
struct sense_t {
inline  int  low(const int ch)  const noexcept {return ch;};
inline int cmp(const void *onev,const void *twov,size_t len) const noexcept {return memcmp(onev,twov,len);}
inline void *cpy(void *dest, const void *src, size_t len) const noexcept {
	return memcpy(dest,src,len);
	}	
};


template <>
struct sense_t<0> {
inline  int  low(const int ch) const  noexcept {return ch;};
inline int cmp(const void *onev,const void *twov,size_t len)  const noexcept{ return unequal<mklow>(onev,twov,len);};
inline void *cpy(void *dest, const void *src, size_t len)  const noexcept{
	return memlowcpy(dest,src,len);
	}	

};
template <bool sens>
inline OFFTYPE Editor::forwardbytealign(SearchAim &aim) {
constexpr auto cpy = sens?memcpy:memlowcpy;
constexpr auto cmp = sens?memcmp: unequal<mklow>;

	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
	return nextforwardbytealign<cmp>(aim) ;

	}
*/
/*
extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
                     size_t __n) __THROW __nonnull ((1, 2));
template <bool forward,bool sens> inline OFFTYPE Editor::bytealign(SearchAim &aim) {
constexpr  auto cpy = sens?memcpy:memlowcpy;
constexpr auto cmp = sens?memcmp: unequal<mklow>;
	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
	return forward?nextforwardbytealign<cmp>(aim):nextbackwardbytealign<cmp>(aim) ;

	}
*/

//template <bool forward,bool sens, void *cpy(void *__restrict , const void *__restrict , size_t )__THROW =sens?memcpy:memlowcpy, Memcmp_t amemcmp=sens?memcmp: unequal<mklow>>
/*
template <bool forward,bool sens, cpytype cpy , Memcmp_t cmp>
inline OFFTYPE Editor::bytealign(SearchAim &aim) {
//constexpr  auto cpy = sens?memcpy:memlowcpy;
//constexpr auto cmp = sens?memcmp: unequal<mklow>;
	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
	return forward?nextforwardbytealign<cmp>(aim):nextbackwardbytealign<cmp>(aim) ;

	}
*/
//constexpr  auto cpy = sens?memcpy:memlowcpy;
//constexpr auto cmp = sens?memcmp: unequal<mklow>;
/*
template <bool sense> class casefunc ;
template <> struct casefunc<true> {
static constexpr  auto cpy = memcpy;
static constexpr auto cmp = memcmp;
	};
template <> struct casefunc<false> {
static constexpr  auto cpy = memlowcpy;
static constexpr auto cmp = unequal<mklow>;
	};

	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
	return forward?nextforwardbytealign<casefunc<sens>::cmp>(aim):nextbackwardbytealign<casefunc<sens>::cmp>(aim) ;
template <> inline OFFTYPE Editor::bytealign<true,false>(SearchAim &aim) {
	return bytealign2<&Editor::nextforwardbytealign<unequal<mklow>>,memlowcpy>(aim);
	}
template <> inline OFFTYPE Editor::bytealign<true,true>(SearchAim &aim) {
	return bytealign2<&Editor::nextforwardbytealign<memcmp>,memcpy>(aim);
	};

template <> inline OFFTYPE Editor::bytealign<false,false>(SearchAim &aim) {
	return bytealign2<&Editor::nextbackwardbytealign<unequal<mklow>>,memlowcpy>(aim);
	}
template <> inline OFFTYPE Editor::bytealign<false,true>(SearchAim &aim) {
	return bytealign2<&Editor::nextbackwardbytealign<memcmp>,memcpy>(aim);
	}

template <decltype(&Editor::icasesearch) proc,cpytype cpy>
inline OFFTYPE Editor::bytealign2(SearchAim &aim) {
	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
//	return forward?nextforwardbytealign<cmp>(aim):nextbackwardbytealign<cmp>(aim) ;
	return (this->*proc)(aim);

	}


*/

template <> inline OFFTYPE Editor::bytealign<true,false>(SearchAim &aim) {
	bytealigngegs<memlowcpy>(aim) ;
	return nextforwardbytealign<unequal<mklow>>(aim);
	}
template <> inline OFFTYPE Editor::bytealign<true,true>(SearchAim &aim) {
	bytealigngegs<memcpy>(aim) ;
	return nextforwardbytealign<memcmp>(aim);
	};

template <> inline OFFTYPE Editor::bytealign<false,false>(SearchAim &aim) {
	bytealigngegs<memlowcpy>(aim) ;
	return nextbackwardbytealign<unequal<mklow>>(aim);
	}
template <> inline OFFTYPE Editor::bytealign<false,true>(SearchAim &aim) {
	bytealigngegs<memcpy>(aim) ;
	return nextbackwardbytealign<memcmp>(aim);
	}
//	return forward?nextforwardbytealign<cmp>(aim):nextbackwardbytealign<cmp>(aim) ;
//	return (this->*proc)(aim);

template <cpytype cpy>
inline void Editor::bytealigngegs(SearchAim &aim) {
	setsearchstr<cpy>(aim);
	aim.align=searchpart->bytes;
	searchbytes=aim.align;
	searchgrens=(filepos+indatatype)%searchbytes;
	}
template <Memcmp_t amemcmp> inline OFFTYPE Editor::nextforwardbytealign(SearchAim &aim) {

int align=aim.align;
extern unsigned long long startuptime;
SEARCHINTDEC( align*0x1E00000000ULL/startuptime )
int BLOCKSIZE=blocksize();
OFFTYPE startpos=(aim.start/align)*align+searchgrens;
BLOCKTYPE bl=startpos/BLOCKSIZE;
int inblock=startpos%BLOCKSIZE;

sectorbuf almem(BLOCKSIZE ,BLOCKSIZE ) ;
char *const allbuf=almem.data();


for(int got=mem.getblock(bl,allbuf);got>=(inblock+searchlen);) {
	int over= got-inblock;
	char *startbuf=allbuf+inblock;
	int items=over/align;
	int takelen=items*align;
	char *endbuf=startbuf+takelen;
	char *buf= startbuf;
	for(;buf<endbuf;buf+=align)
		if(!amemcmp(buf,searchstr,searchlen)) {
			searchpos=buf-allbuf+BLOCKSIZE*bl;
			if(Stype.List)
				addmark(searchpos, searchlen);
			else
				return searchpos++;
			}
	int nog=over-takelen;
	bool match=!amemcmp(buf,searchstr,nog);
	int rest=searchlen-nog;	
	if((got	=mem.getblock(++bl,allbuf))<rest)
		return OFFTYPEINVALID;
	if(nog>0) {
		if(match) {
			if(!amemcmp(startbuf,searchstr+nog,rest)) {
				searchpos=BLOCKSIZE*bl-nog;
				if(Stype.List)
					addmark(searchpos, searchlen);
				else
					return searchpos++;
				}
			}
		inblock=align-nog;
		}
	else
		inblock=0;
 	SEARCHINTER(bl,{});
	searchpos=bl*BLOCKSIZE;
	} ;
return OFFTYPEINVALID;
}

template <Memcmp_t amemcmp> inline OFFTYPE Editor::nextbackwardbytealign(SearchAim &aim) {

int align=aim.align;
extern unsigned long long startuptime;
SEARCHINTDEC( align*0x1000000000ULL/startuptime )
int BLOCKSIZE=blocksize();
OFFTYPE startpos=(aim.start/align)*align+searchgrens;
BLOCKTYPE bl=startpos/BLOCKSIZE;
int inblock=startpos%BLOCKSIZE;

sectorbuf almem(BLOCKSIZE ,BLOCKSIZE ) ;
char *const allbuf=almem.data();

mem.getblock(bl,allbuf);
if(inblock>=searchlen)
	goto FIRST;

while(bl>0) {
{
auto start=searchlen-inblock;
auto match=start?!amemcmp(allbuf,searchstr+start,inblock):0;
mem.getblock(--bl,allbuf);
if(match&&!amemcmp(allbuf+BLOCKSIZE-start,searchstr,start)) {
		searchpos=BLOCKSIZE*(bl+1)-start;
		if(Stype.List)
			addmark(searchpos, searchlen);
		else
			return searchpos++;
		}
inblock=BLOCKSIZE-start-align;
}
FIRST:
for(;inblock>=0;inblock-=align)
	if(!amemcmp(allbuf+inblock,searchstr,searchlen)) {
		searchpos=BLOCKSIZE*bl+inblock;
		if(Stype.List)
			addmark(searchpos, searchlen);
		else
			return searchpos++;
		}
searchpos=bl*BLOCKSIZE;
 SEARCHINTER(bl,{});
}	
return OFFTYPEINVALID;
}



#include "selectsearch.cpp"
