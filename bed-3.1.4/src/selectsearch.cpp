
#include "editor.h"
#include "screenpart.h"
#include "ascii.h"

using hashtype=uint8_t;
constexpr hashtype hasher() { return 0; }
template<typename T> constexpr hashtype hasher(T el) { return el; }
template<typename T, typename... Ts> constexpr hashtype hasher(T el, Ts... els) {  return el |(hasher(els...)<<1); }
template<typename... Ts> constexpr hashtype hasher(uint8_t el, Ts... els) { return el |(hasher(els...)<<1); }
template<typename... Ts> constexpr hashtype hasher(RegexT el, Ts... els) { return el |(hasher(els...)<<2); }

constexpr hashtype gethash( uint8_t forw,RegexT reg,uint8_t raw,uint8_t cas,uint8_t al, uint8_t un) { return hasher(  forw, reg, raw, cas, al,  un);}

//#include "selectsearch.h"

//#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT) {{FORWARD,REGEX,RAW,CASE,ALIGN,UNIT },{&Editor::INIT,&Editor::NEXT}},
//#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT)  case FORWARD|(REGEX<<8)|(RAW<<16)|(CASE<<24)|(ALIGN<<32)|(UNIT<<40) : return {&Editor::INIT,&Editor::NEXT};
//#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT)  case FORWARD|(REGEX<<8)|(RAW<<16)|(CASE<<24)|(ALIGN<<32)|(UNIT<<40) : cout <<#INIT<<endl;
//#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT)  case FORWARD|(REGEX<<8)|(RAW<<16)|((unsigned long long)CASE<<24)|((unsigned long long)ALIGN<<32)|((unsigned long long)UNIT<<40) : return {&Editor::INIT,&Editor::NEXT};
#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT)  case gethash((uint8_t)FORWARD,REGEX,(uint8_t)RAW,(uint8_t)CASE,(uint8_t)ALIGN,(uint8_t)UNIT): return {&Editor::INIT,&Editor::NEXT};

#define	allforms1(FORW,REG,RAW,UNIT,INIT,NEXT)   sear(FORW,REG,RAW, 0,0, UNIT,INIT,NEXT)\
	sear(FORW,REG,RAW, 0,1, UNIT,INIT,NEXT)\
	sear(FORW,REG,RAW, 1,0, UNIT,INIT,NEXT)\
	sear(FORW,REG,RAW, 1,1, UNIT,INIT,NEXT)

#define	allforms(FORW,REG,RAW,INIT,NEXT)  \
	allforms1(FORW,REG,RAW,0,INIT,NEXT)  \
	allforms1(FORW,REG,RAW,1,INIT,NEXT)  



#define unitsearch(FOR,REG,INIT,NEXT) allforms1(FOR,REG,0,1,INIT,NEXT)


#ifdef USE_RE2
#define	re2searchforms allforms(1,RegType::RegRE2, 0,re2shown , nextre2shown)\
	unitsearch(0,RegType::RegRE2,re2backunit,nextbacksingleregex<&Editor::re2match>)
#define re2asciisearch sear(1,RegType::RegRE2, 1,0,0, 0, re2search,re2searchnext) sear(1,RegType::RegRE2, 1,1,0, 0, re2search,re2searchnext)
#else
#define	re2searchforms
#define re2asciisearch 
#endif
//Forward = 0 '\000', Regex = RegHS, Raw = 0 '\000', Case = 0 '\000', Align = 1 '\001', Unit = 1 '\001'}
#ifdef USE_HYPERSCAN
#define hyperforms allforms(1,RegType::RegHS,0, hypershown,nexthypershown)\
	sear(1,RegType::RegHS,1,0,0,0,asciihyperscan,nextasciihyperscan)\
	sear(1,RegType::RegHS,1,1,0,0,asciihyperscan,nextasciihyperscan)\
	unitsearch(0,RegType::RegHS,hyperbackunit,nextbacksingleregex<&Editor::hyperhit>)
#else
#define hyperforms
#endif


#define COMMA ,
#define searchselect sear(0,TNone,1,0,0,0, backicasesearch, nextbackicasesearch)\
	sear(0,TNone,1,0,1,0, bytealign<false  COMMA false>, nextbackwardbytealign<iunequal>)\
	sear(0,TNone,1,1,0,0, backsearch,nextbacksearch)\
	sear(0,TNone,1,1,1,0, bytealign<false COMMA true>, nextbackwardbytealign<memcmp>)\
	allforms1(0,GNU,0, 0,backshownsearch , nextbackshown)\
	sear(0,GNU, 1,0,0, 0, backicaseregexsearch, nextbackregexsearch)\
	sear(0,GNU, 1,1,0, 0, backregexsearch, nextbackregexsearch)\
	unitsearch(0,GNU,gnubackunit,nextbacksingleregex<&Editor::GNUhit>)\
	allforms( 1,GNU, 0 , GNUformsearch , nextGNUregexshown)\
	re2searchforms\
	sear(1,TNone,1,0,0,0, icasesearch,nextforwardicasesearch)\
	sear(1,TNone,1,0,1,0, bytealign<true COMMA false  >, nextforwardbytealign<iunequal>)\
	sear(1,TNone,1,1,0,0, casesearch,nextforwardsearch)\
	sear(1,TNone,1,1,1,0, bytealign<true COMMA true >, nextforwardbytealign<memcmp>)\
	sear(1,GNU, 1,0,0, 0, icaseregexsearch, nextforwardregexsearch)\
	sear(1,GNU, 1,1,0, 0, regexsearch, nextforwardregexsearch)\
	re2asciisearch \
	hyperforms


//hasher(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT)
//
searchert Editor::searchproc( ProcelT proc) const  {
	if( !(proc.Regex || instance(searchpart,Ascii)) ) 
		proc.Case=true;  
//	switch(*reinterpret_cast<const unsigned long long*>(&proc)) {
	switch(gethash(proc.Forward,proc.Regex,proc.Raw,proc.Case,proc.Align,proc.Unit)) {
		searchselect
		default:return  {nullptr,nullptr};
		};
	}

#if 0
#undef sear
//#define AANH "
#define sear(FORWARD,REGEX,RAW,CASE,ALIGN,UNIT,INIT,NEXT)  { gethash((uint8_t)FORWARD,REGEX,(uint8_t)RAW,(uint8_t)CASE,(uint8_t)ALIGN,(uint8_t)UNIT), AANH INIT AANH,AANH NEXT AANH },
struct SearchAll{
uint8_t hash;
const char *init;
const char *next;
} ;
/*
SearchAll allsearch[]={
searchselect
{0,"",""}
};
*/
SearchAll allsearch[]={
{ gethash((uint8_t)0,RegType::None,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " backicasesearch "," nextbackicasesearch " }, { gethash((uint8_t)0,RegType::None,(uint8_t)1,(uint8_t)0,(uint8_t)1,(uint8_t)0), " bytealign<false , false> "," nextbackwardbytealign<iunequal> " }, { gethash((uint8_t)0,RegType::None,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " backsearch "," nextbacksearch " }, { gethash((uint8_t)0,RegType::None,(uint8_t)1,(uint8_t)1,(uint8_t)1,(uint8_t)0), " bytealign<false , true> "," nextbackwardbytealign<memcmp> " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)0), " backshownsearch "," nextbackshown " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)0), " backshownsearch "," nextbackshown " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)0), " backshownsearch "," nextbackshown " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)0), " backshownsearch "," nextbackshown " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " backicaseregexsearch "," nextbackregexsearch " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " backregexsearch "," nextbackregexsearch " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " gnubackunit "," nextbacksingleregex<&Editor::GNUhit> " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " gnubackunit "," nextbacksingleregex<&Editor::GNUhit> " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " gnubackunit "," nextbacksingleregex<&Editor::GNUhit> " }, { gethash((uint8_t)0,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " gnubackunit "," nextbacksingleregex<&Editor::GNUhit> " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)0), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)0), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)0), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)0), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " GNUformsearch "," nextGNUregexshown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)0), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)0), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)0), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)0), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " re2shown "," nextre2shown " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " re2shown "," nextre2shown " }, { gethash((uint8_t)0,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " re2backunit "," nextbacksingleregex<&Editor::re2match> " }, { gethash((uint8_t)0,RegType::RegRE2,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " re2backunit "," nextbacksingleregex<&Editor::re2match> " }, { gethash((uint8_t)0,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " re2backunit "," nextbacksingleregex<&Editor::re2match> " }, { gethash((uint8_t)0,RegType::RegRE2,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " re2backunit "," nextbacksingleregex<&Editor::re2match> " }, { gethash((uint8_t)1,RegType::None,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " icasesearch "," nextforwardicasesearch " }, { gethash((uint8_t)1,RegType::None,(uint8_t)1,(uint8_t)0,(uint8_t)1,(uint8_t)0), " bytealign<true , false > "," nextforwardbytealign<iunequal> " }, { gethash((uint8_t)1,RegType::None,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " casesearch "," nextforwardsearch " }, { gethash((uint8_t)1,RegType::None,(uint8_t)1,(uint8_t)1,(uint8_t)1,(uint8_t)0), " bytealign<true , true > "," nextforwardbytealign<memcmp> " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " icaseregexsearch "," nextforwardregexsearch " }, { gethash((uint8_t)1,RegType::RegGNU,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " regexsearch "," nextforwardregexsearch " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " re2search "," re2searchnext " }, { gethash((uint8_t)1,RegType::RegRE2,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " re2search "," re2searchnext " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)0), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)0), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)0), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)0), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " hypershown "," nexthypershown " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)1,(uint8_t)0,(uint8_t)0,(uint8_t)0), " asciihyperscan "," nextasciihyperscan " }, { gethash((uint8_t)1,RegType::RegHS,(uint8_t)1,(uint8_t)1,(uint8_t)0,(uint8_t)0), " asciihyperscan "," nextasciihyperscan " }, { gethash((uint8_t)0,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)1), " hyperbackunit "," nextbacksingleregex<&Editor::hyperhit> " }, { gethash((uint8_t)0,RegType::RegHS,(uint8_t)0,(uint8_t)0,(uint8_t)1,(uint8_t)1), " hyperbackunit "," nextbacksingleregex<&Editor::hyperhit> " }, { gethash((uint8_t)0,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1), " hyperbackunit "," nextbacksingleregex<&Editor::hyperhit> " }, { gethash((uint8_t)0,RegType::RegHS,(uint8_t)0,(uint8_t)1,(uint8_t)1,(uint8_t)1), " hyperbackunit "," nextbacksingleregex<&Editor::hyperhit> " }
};

extern int searshow(void) ;
int searshow(void)  {
	for(int i=0;i<(sizeof(allsearch)/sizeof(allsearch[0]));i++) {
		SearchAll *el= &allsearch[i];
		printf("%d %s %s\n",el->hash,el->init,el->next);
		}
	}


#endif
