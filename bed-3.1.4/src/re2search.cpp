#include "desturbe.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <re2/re2.h>
#include <alloca.h>
#include "editor.h"
#include "main.h"
#include "system.h"
#include "screenpart.h"
#include "config.h"
#include "misc.h"

#undef MEMORY
//#define MEMORY mem.
#define MEMORY 
#include "searchinterrupt.h"
#define islist Stype.List
//#define onunit ((mem.lastforward&32)?1:0)
#define onunit Stype.Unit
#define onboundary Stype.Align
/*
extern void signalwant(void) ;
void signalwant(void) {
		if(wantsignal) {
			int olddont=GETDEST();
			NODEST();
			int wassig=wantsignal;
			wantsignal=0;
			kill(pid,wassig);	
			SETDEST(olddont);
			}
	}
*/

extern int standardascii(ScreenPart *);
#ifdef USE_RE2
extern const char *regexerrorstring;
// bool PossibleMatchRange(string* min, string* max, int maxlen) const
#include <re2/re2.h>
#ifndef HAVE_OLDRE2_RE2_H
#define StringPiec std::string_view
#endif
const char *Editor::replace(char *input,int inlen,int *outlen) {
using namespace std;
using namespace re2;
	StringPiece repl(re2replacestr,replacestrlen);
	StringPiece zoekin(input,inlen);
	static string out;
	if(RE2::Extract(zoekin,*re2pattern, repl, &out)) {
		*outlen= out.length();
		return out.data();
		}
	*outlen=0;
	return NULL;
	}
/*
void re2memcpy(char *const out,const char *const str,const int len) {
	int slash=0;
	for(int i=0;i<len;i++) {
		if(str[i]=='\\') {
			if(slash) {
				slash=0;
				}
			else {
				slash=1;
				}
			}
		else {
			if(slash) {
				slash=0;
				int get=str[i]-'0';
				if(get>=0&&get<9) {
					out[i]=str[i]+1;;
					continue;
					}
			}
		}
		out[i]=str[i];
	}
}
*/
//bool Editor::re2searchgegs(const signed char *const str,int len,bool sense, bool doreplace) {
bool Editor::re2searchgegs(SearchAim &aim) {
int len=aim.len;
	if(len<=0) {
		regexerrorstring=reinterpret_cast<const char *>(-1);
		output("Search string wrong size");
		return false;
		}

using namespace std;
using namespace re2;
 //      mem.setsearchstr(str, len) ;
#if 0
      	char aroundstr[len+3];
	memcpy(aroundstr+1,aim.str,len); /*To get the match position I add () around the searchstr, 
				       because of this references (\num) to matched groups are incremented with re2memcpy.*/
	aroundstr[0]='(';
	aroundstr[++len]=')';
	aroundstr[++len]='\0';
#endif
	RE2::Options opt;
	opt.set_log_errors(false);
	opt.set_case_sensitive(aim.sense);
    	opt.set_never_nl(false);
	if(!standardascii(aim.datashown)) 
//		opt.set_utf8(false);
		opt.set_encoding(RE2::Options::EncodingLatin1);
#ifdef DOTNLREGEX
     	opt.set_dot_nl(true) ; 
#endif
	if(re2pattern)
		delete re2pattern;

	re2pattern = new RE2((char *)aim.str,opt);
	if(!re2pattern->ok()) {
		beep();
		output(re2pattern->error().c_str());
		regexerrorstring=reinterpret_cast<const char *>(-1);
		delete re2pattern;
		re2pattern =NULL;
		return false;
		}
	if(aim.doreplace) {
		if(!replacestr) {
			delete re2pattern;
			re2pattern =NULL;
			output("No replace pattern geven");
			regexerrorstring=reinterpret_cast<const char *>(-1);
			return false;
			}
		if(re2replacestr)
			free( re2replacestr);
		re2replacestr=(char *)malloc(replacestrlen+1);
//		re2memcpy(re2replacestr,replacestr, replacestrlen);
		memcpy(re2replacestr,replacestr, replacestrlen);
		re2replacestr[replacestrlen]='\0';
		StringPiece repl(re2replacestr,replacestrlen);
		static string error;
		if(!re2pattern->CheckRewriteString(repl, &error) ) {
			beep();
			regexerrorstring=reinterpret_cast<const char *>(-1);
			output(error.data());
			delete re2pattern;
			re2pattern =NULL;
			return false;
			}

		}
	return true;
	}
/*
OFFTYPE Editor::re2search(const OFFTYPE beg,const signed char *const str,int len,bool sense,int *matchlen) {
	mem.searchpos=beg;
	if(re2searchgegs(str,len,sense))
	return OFFTYPEINVALID;
		return mem.re2searchnext(matchlen);
	}
*/
/*
OFFTYPE Editor::re2shown(ScreenPart *datashown,OFFTYPE from,char *ant,short len,int *matchlen) {
	if(len<=0)
		return OFFTYPEINVALID;
INCDEST();
		mem.searchpos=from;
	if(!re2searchgegs((signed char *)ant,len,casesearching(&mem))) {
		message("failure");
		DECDEST();
		return OFFTYPEINVALID;
		}
	DECDEST();

	searchbytes=datashown->bytes;
	searchgrens=(filepos+indatatype)%searchbytes;
	return nextre2shown(datashown,matchlen);
	}

*/


OFFTYPE Editor::re2shown(SearchAim &aim) {
	if(!re2searchgegs(aim) )
		return OFFTYPEINVALID;
	return nextre2shown(aim);
	}


//OFFTYPE Editor::nextre2shown(ScreenPart *datashown,int *mlenptr) {
OFFTYPE Editor::nextre2shown(SearchAim &aim) {
ScreenPart *datashown=aim.datashown;
	if(!re2pattern ) {
		regexerrorstring=reinterpret_cast<const char *>(-1);
		output("No previous (re2) search");
		return OFFTYPEINVALID;
		}
	if(onunit) {
		OFFTYPE pos=nextsingleregex(datashown,&Editor::re2match) ;
		aim.matchlen=searchbytes;
		return pos;
		}
	short boundaries;
	OFFTYPE bytestart;
	const int bytes=datashown->bytes;
	if(onboundary) {
		bytestart=(searchpos/searchbytes)*searchbytes+searchgrens;
		boundaries=1;
		}
	else {
		bytestart= searchpos;
		boundaries=bytes;
		}
	OFFTYPE off=bytestart;
	if(off>=filesize())
			return OFFTYPEINVALID;
	const int bs= blocksize();
	BLOCKTYPE blnr=off/bs;
	int inblock=off%bs;
	const int nrblock= ((bytes-1)/bs)+2;
	const int nr1=nrblock-1;

extern unsigned long long startuptime;
SEARCHINTDEC(6000000000/startuptime,nr1,blnr+nrblock)


	const int bufsize=nrblock*bs;
	char * const binbuf=(char *)alignedalloc(bs,bufsize);
	const long items=(bufsize/bytes);
	const int ellen=datashown->maxres+datashown->apart;
	const int formsize=(items*ellen);
        char formbuf[formsize];

OFFTYPE took=mem.getblocks(blnr,binbuf,nrblock); 	 

if(took==OFFTYPEINVALID) {
	alignedfree(binbuf);
	return took;
	}
blnr+=nrblock;
int binlen=took-inblock,takelen=binlen; 

const int patlen=minnum(100,bs-bytes+1);
OFFTYPE first=OFFTYPEINVALID;
int firstlen=0;

bool list=islist;
int terug=0;
#ifdef __clang__
OFFTYPE lastpos[boundaries];
memset(&lastpos,0,sizeof(OFFTYPE)*boundaries);
#else
OFFTYPE lastpos[boundaries] {};
#endif
while(took>0) {
for(int shift=0;shift<boundaries;shift++) {
	const int diff=lastpos[shift]-(off+shift);
//	const int added=shift+((diff>0)?((diff+bytes)/bytes)*bytes:0);
	const int added=shift+((diff>0)?diff:0);
	const int inbuf= inblock+added;
	const int count=datashown->fromascii((unsigned char *)formbuf,(unsigned char *)(binbuf+inbuf),takelen-added);
	int matchlen;
	int startpos=0;
	for(int pos;startpos<count&&((matchlen=re2match(formbuf+startpos,count-startpos,&pos))>0);startpos+=ellen) {
		startpos+=pos;
//		const int binpos=((startpos + datashown->apart)/ellen)*bytes;
		const int binpos=(startpos/ellen)*bytes;
		const OFFTYPE oppos=	off+binpos+added;
		const int binreslen=(1+(startpos+matchlen-1)/ellen)*bytes-binpos;
		if(list)  {
			addmark(oppos,binreslen);
//			lastpos[shift]=oppos+boundaries;
			lastpos[shift]=oppos+bytes;
			}
		else
		if(oppos<first)  {
			first=oppos;
			firstlen=binreslen;
			takelen=binpos+firstlen+patlen+shift;
			unitformlen=matchlen;
			unitinelpos=startpos%ellen;
			if(takelen>binlen)
				takelen=binlen;
			break;
			}
		}
	if(startpos==0)
		lastpos[shift]=0;
	}
	//if(!list&&(first!=OFFTYPEINVALID)) {
	if(!list&&firstlen>0) {
		searchpos=first;
		aim.matchlen=firstlen;
		alignedfree(binbuf);
		if(onboundary) {
			return (searchpos+=searchbytes,searchpos-searchbytes);
			}
		else
			return searchpos++;
		}

	SEARCHINTER(blnr,alignedfree(binbuf));

	searchpos=off=blnr*bs-patlen; /*Maybe a pattern matches with the end of the past block end the beginning of the next block*/
//	 /*to know the last search position during list search*/
	int units=((off- bytestart +1)/searchbytes)*searchbytes;
	off=bytestart+units;
	terug=blnr*bs-off;
	char *const bufbegin=binbuf-terug+bs;
	memcpy(bufbegin,bufbegin+bs*nr1,terug);

	took=mem.getblocks(blnr,binbuf+bs,nr1);
	inblock=bs-terug;
	takelen=took+terug;
	blnr+=nr1;
}
	alignedfree(binbuf);
	return OFFTYPEINVALID;
	}







int Editor::asciire2replace(OFFTYPE pos,int matchlen, char *str,int len  ) {
	INCDEST();
	memorize();
	int bs=blocksize();
	int inblock=pos%bs;
	BLOCKTYPE bl=pos/bs,endblock=(pos+matchlen-1)/bs+1;
	int bnr=	endblock-bl;
	char *bin=alignedalloc(bs,bs*bnr);
	mem.getblocks(bl,bin,bnr);
	int rlen;
	int restlen=bnr*bs-inblock;
	char *begin= bin+inblock;
	const char *rstr=replace(begin,restlen,&rlen);
unsigned char *undo=(unsigned char *)expandchunk(NULL,rlen,0);
int elen=rlen;
	if(rlen>restlen) {
		int meer=rlen-restlen;
		int blks=(meer-1)/bs+1;
		int tot=blks*bs;
		char *bijbuf=alignedalloc(bs,tot);
		mem.getblocks(bl+bnr,bijbuf,blks);
		memcpy(undo+restlen,bijbuf,meer);
		memcpy(bijbuf,rstr+restlen,meer);
		mem.putblocks(bl+bnr,bijbuf,blks);
		alignedfree(bijbuf);
		elen=restlen;
		}
	memcpy(undo,begin,elen);
	memcpy(begin,rstr,elen);
	mem.putblocks(bl,bin,bnr);
	alignedfree(bin);
	inundo(pos,undo,rlen);
	modified++;
	DECDEST();
	return rlen;
}
#include <re2/re2.h>
#include <string>
#include <assert.h>
#if 1

int Editor::formre2replace(OFFTYPE pos,int matchlen, char *str,int len  ) {
using namespace std;
using namespace re2;

	const int bytes=searchpart->bytes;
	INCDEST();
	memorize();
	int bs=blocksize();
	int inblock=pos%bs;
	BLOCKTYPE bl=pos/bs,endblock=(pos+matchlen-1)/bs+1;
	int bnr=	endblock-bl;
	const int binbuflen= bs*bnr;
	unsigned char *bin=(unsigned char *)alignedalloc(bs,binbuflen);
	mem.getblocks(bl,(char *)bin,bnr);
	unsigned char *begin= bin+inblock;

	const long items=(matchlen/bytes);
	const int apart= searchpart->apart;
	const int ellen=searchpart->maxres+apart;
	const size_t formsize=(items*ellen);
        char *formbuf=(char *)alloca(formsize);
	int count=searchpart->bin2search(begin,  formbuf, matchlen )  ;
int rlen;
const char *rstr=replace(formbuf+unitinelpos,unitformlen,&rlen);
int end=(unitinelpos+rlen);
int elnr= (end+ellen-1)/ellen;
int takelen=elnr*ellen;
if(end>count) {
	const int binlen=elnr*bytes;
	const int endbin=inblock+binlen;
	if(endbin>binbuflen) {
		const BLOCKTYPE endbl=(endbin-1)/bs+1;
		const	int bnr2=endbl-bl;
		unsigned char * const bin2=(unsigned char*)alignedalloc(bs,bs*bnr2);	
//		memcpy(bin2+inblock,bin+inblock,binbuflen-inblock);
		memcpy(bin2,bin,binbuflen);
		alignedfree(bin);
		mem.getblocks(endblock,(char *)bin2+binbuflen,bnr2-bnr);
		bin=bin2;
		bnr=bnr2;
		begin=bin+inblock;
		}
	formbuf=(char *)alloca(takelen);
	count=searchpart->bin2search(begin,  formbuf,  binlen)  ;
	}
memcpy(formbuf+unitinelpos,rstr,rlen);
int res;
if(Stype.Unit) {
	appendstrtoundo(pos,(char *)begin, bytes);
	searchpart->search2bin(formbuf,begin,takelen-apart,bytes);
	mem.putblocks(bl,(char *)bin,bnr);
	res=bytes;
	}
else
	res= rstr?insertshownpos(pos,formbuf, takelen-apart) :-1;
//static bool RE2::Extract(const StringPiece &text, const RE2& pattern, const StringPiece &rewrite, string *out);
	alignedfree((char *)bin);


	DECDEST();
	return res;
}
#else
int Editor::formre2replace(OFFTYPE pos,int matchlen, char *str,int len  ) {
using namespace std;
using namespace re2;

	const int bytes=searchpart->bytes;
	INCDEST();
	memorize();
	int bs=blocksize();
	int inblock=pos%bs;
	BLOCKTYPE bl=pos/bs,endblock=(pos+matchlen-1)/bs+1;
	int bnr=	endblock-bl;
	unsigned char *bin=(unsigned char *)alignedalloc(bs,bs*bnr);
	mem.getblocks(bl,(char *)bin,bnr);
	unsigned char *begin= bin+inblock;

	const long items=(matchlen/bytes);
	const int apart= searchpart->apart;
	const int ellen=searchpart->maxres+apart;
	const size_t formsize=(items*ellen);
	string formstring;
	formstring.resize(formsize); /*How do you get rid of this unnecessary filling? I only have to reserve space and set the size*/
        char *formbuf=const_cast<char *>(formstring.data());
	const int count=searchpart->bin2search(begin,  formbuf, matchlen )  ;
	alignedfree((char *)bin);
	formstring.resize(count);
	StringPiece repl(re2replacestr,replacestrlen);
	RE2::Replace(&formstring, *mem.re2pattern, repl);
int rlen=formstring.length();
const char *rstr=formstring.data();
//	const char *rstr=replace(formbuf,count,&rlen);

int res= insertshownpos(pos,rstr, rlen) ;
	DECDEST();
	return res;
}
#endif
int Editor::re2replace(OFFTYPE pos,int matchlen, char *str,int len  ) {
	if(standardascii(searchpart)) 
		return asciire2replace( pos, matchlen, str, len  ) ;
	else 
		return formre2replace( pos, matchlen, str, len  ) ;

	}



#ifdef USE_RE2
#include <re2/re2.h>

		/* According to MS documentation some drives can only be read through RAM addresses on sector boundaries.
		 * And I really have a disk that can only be read with even addresses, so maybe it is serious
		 * We need also to make extra room for patterns overlapping block boundaries, therefor start with the second BLOCK.
		 */

#define SECTORSIZE blocksize()

//OFFTYPE Editor::re2searchnext(int *matchlen) {
OFFTYPE Editor::re2search(SearchAim &aim) {
	if(!re2searchgegs(aim) )
		return OFFTYPEINVALID;
	return re2searchnext(aim);
	}

OFFTYPE Editor::re2searchnext(SearchAim &aim) {
if(!re2pattern) {
	regexerrorstring="No previous (re2) search";
	return OFFTYPEINVALID;
}

extern unsigned long long startuptime;
SEARCHINTDEC( 0x4000000000/startuptime )
using namespace std;
using namespace re2;

char *const buf=(char *)alignedalloc(blocksize(),SECTORSIZE+blocksize());
BEDS_SIZE_T count,count2, begin=searchpos%blocksize();
char *bufbegin=begin+buf;
BLOCKTYPE	bl=searchpos/blocksize(); 
if((count=mem.getblock(bl++,buf))>=blocksize())
 if((count2=mem.getblock(bl,buf+SECTORSIZE))>0)
	count+=count2;


bool list=Stype.List;
for(;count>0;) {
		int bufsize=count-begin;
		int pos,mlen;
		for(int start=0;start<bufsize&&(mlen=re2match(bufbegin+start,bufsize-start ,&pos))>0;) {
			start+=pos;
			searchpos=bl*blocksize()+start++ +bufbegin-buf-SECTORSIZE;
			if(list) {
				addmark(searchpos,mlen);
				}
			else {	
				alignedfree(buf);
				aim.matchlen=mlen;
				
				return searchpos++;
				}
			}
		if(count<blocksize())
			break;
int patlen=100;
		begin=-patlen;
		bufbegin=buf+begin+SECTORSIZE;
		memcpy(bufbegin,bufbegin+blocksize(),patlen);
		count=mem.getblock(++bl,buf+SECTORSIZE);
		searchpos=bl*blocksize(); /*To know where interrupted*/
	SEARCHINTER(bl,alignedfree(buf));
		}
alignedfree(buf);
	return OFFTYPEINVALID;
	}
#endif

#endif
