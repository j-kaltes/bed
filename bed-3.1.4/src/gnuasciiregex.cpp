#include <stdlib.h>

#include "editor.h"

#include "system.h"
#define noop (void)0
//#if defined(_WIN32) && !defined(USETHREADS)
#undef MEMORY
#define MEMORY
#include "searchinterrupt.h"

#define ablock(blo)	buf+blocksize*(blo)

const char *regexerrorstring=NULL;
/*
int
re_search (bufp, string, size, startpos, range, regs)
     struct re_pattern_buffer *bufp;
     const char *string;
     int size, startpos, range;
     struct re_registers *regs;
{
  return re_search_2 (bufp, NULL, 0, string, size, startpos, range, 
		      regs, size);
}
*/
#define re_search_12(pat,s1,l1,s2,l2,start,range,regs,stop) ((l2>0)?re_search_2(pat,s1,l1,s2,l2,start,range,regs,stop):re_search_2(pat,NULL,0,s1,l1,start,range,regs,l1) )


#define getmatchlen if(ret!=OFFTYPEINVALID) {aim.matchlen=searchregs->end[0]-searchregs->start[0];};
//#define MYSYNTAX ( RE_BK_PLUS_QM              | RE_CHAR_CLASSES                          | RE_HAT_LISTS_NOT_NEWLINE | RE_INTERVALS                             | RE_NEWLINE_ALT)

static const auto  MYSYNTAX =
#ifdef  RE_NO_POSIX_BACKTRACKING
RE_NO_POSIX_BACKTRACKING|
#endif
#ifdef DOTNLREGEX
RE_DOT_NEWLINE|
#endif
 RE_CHAR_CLASSES | RE_HAT_LISTS_NOT_NEWLINE | RE_INTERVALS | RE_NEWLINE_ALT| RE_NO_BK_BRACES|RE_BACKSLASH_ESCAPE_IN_LISTS|RE_NO_BK_VBAR|RE_NO_BK_PARENS ;

void Editor::initsearch( void) {
	if(!searchexpr){
		if(!(searchexpr=myallocp(struct re_pattern_buffer*,struct re_pattern_buffer)))
				return;
		if(!(searchregs=myallocp(struct re_registers*,struct re_registers))) {
			myfree(searchexpr);
			return;
			}
		}
	memset(searchexpr,'\0',sizeof(struct re_pattern_buffer));
	 re_set_syntax(MYSYNTAX);
	 return;
	}

#ifdef HAS_RE_SEARCH
	#define SET_TRANSLATE(ta)ta
#else
	#define SET_TRANSLATE(ta) (char *)(ta)
#endif




//OFFTYPE Editor::backicaseregexsearch(OFFTYPE beg, signed char *str,int len) {
OFFTYPE Editor::backicaseregexsearch(SearchAim &aim) {

	unsigned char *ta;
	int i;
	initsearch();
	searchpos=aim.start;
//	setsearchstr(aim.str,aim.len);
	 searchexpr->fastmap=(char *)tablebase;
	 ta=((unsigned char*)tablebase)+256;
	 for(i=0;i<0400;i++)
	 	ta[i]=i;
	 for(i='a';i<='z';i++)
	 	ta[i]=i-040;	
	 searchexpr->translate=SET_TRANSLATE(ta);
	if((regexerrorstring=re_compile_pattern((char *)aim.str, aim.len, searchexpr))) {
		return OFFTYPEINVALID;
	 	}
	return nextbackregexsearch(aim); 
	}

//OFFTYPE Editor::backregexsearch(OFFTYPE beg, signed char *str,int len) {
OFFTYPE Editor::backregexsearch(SearchAim &aim) {
	initsearch();
	searchpos=aim.start;
//	setsearchstr(aim.str,aim.len);
	searchexpr->translate=NULL;

	searchexpr->fastmap=(char *)tablebase;
	if((regexerrorstring=re_compile_pattern((char *)aim.str, aim.len, searchexpr))) {
		return OFFTYPEINVALID;
	 	}
	return nextbackregexsearch(aim); 
	}






OFFTYPE Editor::nextbackregexsearch(SearchAim &aim) {
	if(!searchexpr) {
		regexerrorstring="No previous (gnu regex) search";
		return OFFTYPEINVALID;
	}

extern unsigned long long startuptime;
SEARCHINTDEC(0x500000000/startuptime )
//SEARCHINTDEC(20000)

//        char buf[3][blocksize];
	auto blocksize=mem.BLOCKSIZE;
        char *buf=alignedalloc(blocksize,2*blocksize);;
	BEDS_SIZE_T count,count0;
	BLOCKTYPE bl,endblock;
	BEDS_SIZE_T pos;
	BEDS_SIZE_T last;
	OFFTYPE end;

	end=(searchpos+(blocksize/2));
	endblock=end/blocksize;

        bl=endblock;
//s/buf\[\([^\]]*\)\]/ablock(\1)/g
        count0=mem.getblock(bl,ablock(bl&1));
	bl--;
        if(bl>=0) {
		count=mem.getblock(bl,ablock(bl&1));
		last=searchpos-bl*blocksize+1;
		}
	else {
		last=searchpos+1;
		count=0;
		}
	auto list=Stype.List;
	auto last1=last-1;
	for(pos=last1;pos>=0&&(pos=re_search_12(searchexpr, ablock((bl)&1), count, ablock((bl+1)&1), count0,pos,-pos,searchregs,count+count0))!=-1;pos--) {
			if(bl<0)	
				searchpos=pos;
			else
				searchpos=pos+((bl)*blocksize);
			
			const auto matchlen=searchregs->end[0]-searchregs->start[0];
			if(list)
				addmark(searchpos,matchlen);
			else {
				alignedfree(buf);
				aim.matchlen=matchlen;
				return searchpos--;
				}
			}

        for(; bl>0;) {
		bl--;
		mem.getblock(bl,ablock(bl&1));
		auto blsize1=blocksize-1;
		for(pos=blsize1;pos>=0&&(pos=re_search_12(searchexpr, ablock((bl)&1), blocksize, ablock((bl+1)&1), blocksize,pos,-pos,searchregs,2*blocksize))!=-1;pos--) {
			searchpos=pos+((bl)*blocksize);
			auto matchlen=searchregs->end[0]-searchregs->start[0];
			if(list)
				addmark(searchpos,matchlen);
			else {
				alignedfree(buf);
				aim.matchlen=matchlen;
				return searchpos--;
				}
			}
		searchpos=bl*blocksize;
		SEARCHINTER(bl,alignedfree(buf));
		}
	alignedfree(buf);
	return OFFTYPEINVALID;
	}
OFFTYPE Editor::regexsearch(SearchAim &aim) {
	initsearch();
	if(!searchexpr) {
	       usesearch={nullptr,nullptr};
		return OFFTYPEINVALID;
	}
	searchpos=aim.start;
//	setsearchstr(aim.str,aim.len);
	searchexpr->translate=NULL;
	searchexpr->fastmap=(char *)tablebase;
	if((regexerrorstring=re_compile_pattern((char *)aim.str, aim.len, searchexpr))) {
		return OFFTYPEINVALID;
	 	}
	return nextforwardregexsearch(aim); 
	}


OFFTYPE Editor::icaseregexsearch(SearchAim &aim) {

	unsigned char *ta;
	int i;
	initsearch();
	searchpos=aim.start;

//	setsearchstr(aim.str,aim.len);
	 searchexpr->fastmap=(char *)tablebase;
	 ta=((unsigned char*)tablebase)+256;
	 for(i=0;i<0400;i++)
	 	ta[i]=i;
	 for(i='a';i<='z';i++)
	 	ta[i]=i-040;	
	 searchexpr->translate=SET_TRANSLATE(ta);

	if((regexerrorstring=re_compile_pattern((char *)aim.str, aim.len, searchexpr))) {
		return OFFTYPEINVALID;
	 	}
	return nextforwardregexsearch(aim); 
	}

OFFTYPE Editor::nextforwardregexsearch(SearchAim &aim) {

	if(!searchexpr) {
		regexerrorstring="No previous (gnu regex) search";
		return OFFTYPEINVALID;
	}
extern unsigned long long startuptime;
SEARCHINTDEC( 0x4000000000/startuptime )

	auto blocksize=mem.BLOCKSIZE;
        char *buf=alignedalloc(blocksize,2*blocksize);
	BEDS_SIZE_T count,count0,pos,begin;
	BLOCKTYPE bl;

	begin=searchpos%blocksize;
auto list=Stype.List;
        for(bl=searchpos/blocksize,count=mem.getblock(bl,ablock(bl&1));	count>0;) {
		bl++;
		count0=count-begin;
		count=mem.getblock(bl,ablock(bl&1));
		int endpos=count0-1;
		for(pos=0;pos<count0&&(pos=re_search_12(searchexpr, ablock(((bl-1)&1))+begin, count0, ablock(bl&1), count, pos, endpos-pos,searchregs,count0+count))!=-1;pos++) {
			searchpos=pos+((bl-1)*blocksize)+begin;
			const auto matchlen=searchregs->end[0]-searchregs->start[0];
			if(list) {
				addmark(searchpos,matchlen);
				}
			else {
				alignedfree(buf);
				aim.matchlen=matchlen;
				return searchpos++;
				}
			}
		begin=0;
		searchpos=bl*blocksize;
	       SEARCHINTER(bl,alignedfree(buf));
		}
	alignedfree(buf);
	return OFFTYPEINVALID;
	}

//int Editor::setsearchgegs(OFFTYPE beg,char *str,int len) {
int Editor::setsearchgegs(SearchAim &aim) {
	DEBUGGING("int Editor::setsearchgegs %.9s#%d\n",aim.str,aim.len);

	initsearch();
	if(!searchexpr) {
	       usesearch={nullptr,nullptr};
			return -1;
		}
	searchpos=aim.start;
     //  setsearchstr((signed char*)aim.str, aim.len) ;
	searchexpr->fastmap=(char *)tablebase;
	if(Stype.Case) 
		searchexpr->translate=NULL;
	else {
		int i;
		unsigned char *ta;
		 ta=((unsigned char*)tablebase)+256;
		 for(i=0;i<0400;i++)
			ta[i]=i;
		 for(i='a';i<='z';i++)
			ta[i]=i-040;	
	 	searchexpr->translate=SET_TRANSLATE(ta);
		}

	if((regexerrorstring=re_compile_pattern((char *)aim.str, aim.len, searchexpr))) {
		return -1;
	 	}

	return 0;
	}



