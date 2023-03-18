#include <assert.h>
#include "editor.h"
#include "screenpart.h"
#include "misc.h"
#undef MEMORY
#define MEMORY 
#include "searchinterrupt.h"

#define onunit Stype.Unit
#define onboundary Stype.Align
#define islist Stype.List
OFFTYPE Editor::nextsingleregex(ScreenPart *const datashown,int (Editor::* const matcher)(const char *const,const int,int *))  {

extern unsigned long long startuptime;
SEARCHINTDEC(0x200000000/startuptime)
	short grens;
	OFFTYPE bytestart;
	const int bytes=datashown->bytes;
	if(onboundary) {
		bytestart=(searchpos/searchbytes)*searchbytes+searchgrens;
		grens=bytes;
		}
	else {
		bytestart= searchpos;
		grens=1;
		}
	const int maxres=datashown->maxres;
	const int ellen=maxres+datashown->apart;
	char formbuf[ellen];
	OFFTYPE fs=filesize();
	if((bytestart+bytes)>fs)
			return OFFTYPEINVALID;
	int bs=blocksize();
	BLOCKTYPE bl=bytestart/bs;	
	int binoff=bytestart%bs;
	char *binbuf= alignedalloc(bs,bs);

	int took=mem.getblock(bl,binbuf);

bool list=islist;
 while(took>=bytes) {
	int offend=took-bytes+1;
	for(;binoff<offend;binoff+=grens) {
#ifndef NDEBUG
		int count =
#endif
datashown->bin2search((unsigned char*)binbuf+binoff,  formbuf, bytes )  ;
		assert(count>=maxres);
		int len,offset;
		if((len=(this->*matcher)(formbuf,maxres,&offset ))>0) {
			const OFFTYPE off=bs*bl+binoff;
			if(list) {
				addmark(off,bytes);
				}
			else {
				unitformlen=len;
				unitinelpos=offset;
				alignedfree(binbuf);
				if(onboundary) {
					searchpos=off +searchbytes;
					}
				else
						searchpos=off+1;
				return off;
				}
			}
	    }
	if(took<bs)
		break;
	SEARCHINTER(bl,alignedfree(binbuf));
	int start=bs-binoff;
	if(start>0) {
		const int binslen=bytes+start-1;
		char bins[binslen];
		memcpy(bins,binbuf+binoff,start);
		took=mem.getblock(++bl,binbuf);
		int innext=minnum(binslen-start,took);
		memcpy(bins+start,binbuf,innext);
		int itend=innext+start-bytes+1;
		int it=0;
		for(;it<itend;it+=grens) {
#ifndef NDEBUG
		int count =
#endif
			datashown->bin2search((unsigned char*)bins+it,  formbuf, bytes )  ;
			assert(count>=maxres);
			int len,offset;
			if((len=(this->*matcher)(formbuf,maxres,&offset))>0) {
				const OFFTYPE off=bs*bl-start+it;
				if(list) {
					addmark(off,bytes);
					}
				else {
					unitformlen=len;
					unitinelpos=offset;
					alignedfree(binbuf);
					searchpos=off+1;
					return off;
					}
				}
			}
		binoff=it-start;
		}
	else {
		took=mem.getblock(++bl,binbuf);
		binoff=0;
		}
	searchpos=bs*bl;
 	};
		
alignedfree(binbuf);
return OFFTYPEINVALID;
}
template <int (Editor::* const matcher)(const char *const,const int,int *)>
OFFTYPE Editor::nextbacksingleregex(SearchAim &aim)  {
ScreenPart *const datashown=aim.datashown;

extern unsigned long long startuptime;
SEARCHINTDEC(0x450000000/startuptime)
	short grens;
	OFFTYPE bytestart;
	const int bytes=datashown->bytes;
	if(onboundary) {
		bytestart=(searchpos/searchbytes)*searchbytes+searchgrens;
		grens=bytes;
		}
	else {
		bytestart= searchpos;
		grens=1;
		}
	if(bytestart<0)
		return OFFTYPEINVALID;
	const int maxres=datashown->maxres;
	const int ellen=maxres+datashown->apart;
	char formbuf[ellen];
	int bs=blocksize();
	BLOCKTYPE bl=bytestart/bs;	
	int binoff=bytestart%bs;
	char *binbuf= alignedalloc(bs,bs);

	int took=mem.getblock(bl,binbuf);
if(took<binoff) {
	if(onboundary)
		binoff-=((binoff-took+bytes-1)	/bytes)*bytes;
	else
		binoff=took;
	if(binoff<0) 
		return OFFTYPEINVALID;
	}

bool list=islist;
 while(bl>=0) {
	SEARCHINTER(bl,alignedfree(binbuf));
	for(;binoff>=0;binoff-=grens) {
#ifndef NDEBUG
		int count =
#endif
	datashown->bin2search((unsigned char*)binbuf+binoff,  formbuf, bytes )  ;
		assert(count>=maxres);
		int len,offset;
		if((len=(this->*matcher)(formbuf,maxres,&offset ))>0) {
			const OFFTYPE off=bs*bl+binoff;
			if(list) {
				addmark(off,bytes);
			}
			else {
				unitformlen=len;
				unitinelpos=offset;
				alignedfree(binbuf);
				searchpos=off-1;
				aim.matchlen=bytes;
				return off;
			}
			}
	    }
	if(bl<=0)
		break;
	const int bmin=bytes-1;
	const int binslen=2*bmin;
	char bins[binslen];
	memcpy(bins+bmin,binbuf,bmin);
searchpos=bl*bs;
	took=mem.getblock(--bl,binbuf);
	const auto inprev=bs-bmin;
	memcpy(bins,binbuf+inprev,bmin);
	auto it=bmin+binoff;
	for(;it>=0;it-=grens) {
#ifndef NDEBUG
		const auto count =
#endif
datashown->bin2search((unsigned char*)bins+it,  formbuf, bytes )  ;
		assert(count>=maxres);
		int len,offset;
		if((len=(this->*matcher)(formbuf,maxres,&offset ))>0) {
			const OFFTYPE off=bs*bl+inprev+it;
			if(list) 
				addmark(off,bytes);
			else {
				unitformlen=len;
				unitinelpos=offset;
				alignedfree(binbuf);
				searchpos=off-1;
				aim.matchlen=bytes;
				return off;
				}
			}
		}
	binoff=inprev+it;
//	binoff=bs-bytes;
 	};
		
alignedfree(binbuf);
return OFFTYPEINVALID;
}

/*
template OFFTYPE Editor::nextbacksingleregex<&Editor::GNUhit>(SearchAim &);
template OFFTYPE Editor::nextbacksingleregex<&Editor::re2match>(SearchAim &);
template OFFTYPE Editor::nextbacksingleregex<&Editor::hyperhit>(SearchAim &);
*/

OFFTYPE Editor::gnubackunit(SearchAim &aim) {
	if(setsearchgegs(aim )<0) 
			return OFFTYPEINVALID;
	return Editor::nextbacksingleregex<&Editor::GNUhit>(aim)  ;
	}

#ifdef USE_RE2
OFFTYPE Editor::re2backunit(SearchAim &aim) {
	if(re2searchgegs(aim)) {
		return Editor::nextbacksingleregex<&Editor::re2match>(aim)  ;
		}
	return OFFTYPEINVALID;
	}
#endif


#ifdef USE_HYPERSCAN
OFFTYPE Editor::hyperbackunit(SearchAim &aim) {
	if(hyperstart(aim,HS_MODE_BLOCK)<0)
			return OFFTYPEINVALID;
	return Editor::nextbacksingleregex<&Editor::hyperhit>(aim)  ;
	}
#endif
