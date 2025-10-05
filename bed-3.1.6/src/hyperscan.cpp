/*
Uses hyperscan regex library:
http://intel.github.io/hyperscan/dev-reference/intro.html
It is easy to use with very large files, but has also some drawbacks for this editor:
- returns only the end of the match.  HS_FLAG_SOM_LEFTMOST doesn't work.
- no backward search.
- no replace
- very large
*/
#include <hs/hs.h>
#include "editor.h"
#include "screenpart.h"
#include "desturbe.h"
#include "misc.h"
#undef MEMORY
#define MEMORY
//#define MEMORY mem.
#include "searchinterrupt.h"
extern const char *regexerrorstring;
typedef struct {
	Editor *ed;
	OFFTYPE pos;
} ToHShandler;
static int hsasciionce(unsigned int id, unsigned long long from, unsigned long long endpos, unsigned int flags, void *ctx) {
	 ToHShandler *gegs=(  ToHShandler *)ctx;
	gegs->pos=endpos;
    	return 0;
}
static int hsHandler(unsigned int id, unsigned long long from, unsigned long long endpos, unsigned int flags, void *ctx) {
	 ToHShandler *gegs=(  ToHShandler *)ctx;
	gegs->ed->addmark(gegs->pos+endpos-1,1);
    return 0;
}

//#define islist ((mem.lastforward>>4)&1)
//#define islist (mem.lastforward&0x10)
#define islist (Stype.List)
//OFFTYPE Editor::nextasciihyperscan(const OFFTYPE pos) {
OFFTYPE Editor::nextasciihyperscan(SearchAim &aim) {
const OFFTYPE pos=aim.start;
extern unsigned long long startuptime;
SEARCHINTDEC( 0x10000000000/startuptime )
	if(!hyperdata) {
		regexerrorstring=reinterpret_cast<const char *>(-1);
		output("No previous (hyperscan) search");
		return OFFTYPEINVALID;
		}
	memorize();
hs_stream_t *astream=NULL;
    if (hs_open_stream(hyperdata, 0, &astream) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate stream space\n");
	hs_free_scratch(hyperscratch);
        hs_free_database(hyperdata);
        hyperdata=NULL;
        return OFFTYPEINVALID;
    }

const int bs=blocksize();
char *buf=alignedalloc(bs,bs);
    BLOCKTYPE bl=pos/bs;
    int inblock=pos%bs;
int len=mem.getblock(bl,buf);
char *ptr=buf+inblock;
match_event_handler handler;
ToHShandler tohandler;
bool list=islist;
if(list) {
	tohandler.ed=this;
	tohandler.pos=pos;
	handler=hsHandler;
}
else {
	tohandler.pos=OFFTYPEINVALID;
	handler=hsasciionce;
	}
if (hs_scan_stream(astream, ptr, len-inblock,0,hyperscratch, handler, &tohandler) != HS_SUCCESS) {
	fprintf(stderr, "ERROR: Unable to scan input buffer\n");
	 hs_close_stream(astream, hyperscratch, NULL, NULL);
	hs_free_scratch(hyperscratch);
	alignedfree(buf);
	hs_free_database(hyperdata);
        hyperdata=NULL;
		return OFFTYPEINVALID;
    }

while((len=mem.getblock(++bl,buf))>0) {
	searchpos=bs*bl;
			
	    if (hs_scan_stream(astream, buf, len,0,hyperscratch, handler, &tohandler) != HS_SUCCESS) {
		fprintf(stderr, "ERROR: Unable to scan input buffer\n");
	 	hs_close_stream(astream, hyperscratch, NULL, NULL);
		hs_free_scratch(hyperscratch);
		alignedfree(buf);
		hs_free_database(hyperdata);
		hyperdata=NULL;
		return OFFTYPEINVALID;
	    }
    	if(!list&&tohandler.pos!=OFFTYPEINVALID)
			break;
	SEARCHINTER(bl,{alignedfree(buf); hs_close_stream(astream, hyperscratch, NULL,NULL);});
	}
hs_close_stream(astream, hyperscratch, handler, &tohandler);
alignedfree(buf);
if(list) {
    hs_free_scratch(hyperscratch);
    hs_free_database(hyperdata);
    hyperdata=NULL;
}
else
   if(tohandler.pos!=OFFTYPEINVALID){
	 searchpos=tohandler.pos;
	aim.matchlen=1;
	return (pos+tohandler.pos-1);
   	}
    return OFFTYPEINVALID;
}
/*s/-1/OFFTYPEINVALID/g*/
//OFFTYPE Editor::asciihyperscan(const OFFTYPE pos,const char *const pattern,const int patlen,const bool sense) {
OFFTYPE Editor::asciihyperscan(SearchAim &aim) {
const char *const pattern=(char *)aim.str;
    hs_compile_error_t *compile_err;
    if(hyperdata) {
        hs_free_database(hyperdata);
    	hs_free_scratch(hyperscratch);
        hyperdata=NULL;
    	}
//    mem.setsearchstr((const signed char *)pattern,patlen);
   if (hs_compile(pattern,HS_FLAG_MULTILINE| HS_FLAG_UTF8 |

#ifdef DOTNLREGEX
			   HS_FLAG_DOTALL |
#endif
			   (aim.sense?0:HS_FLAG_CASELESS) |(islist?0:HS_FLAG_SINGLEMATCH ), HS_MODE_STREAM, NULL, &hyperdata, &compile_err) != HS_SUCCESS) {
	regexerrorstring=reinterpret_cast<const char *>(-1);
        message( "%s: \"%s\"", compile_err->message,pattern);
        hs_free_compile_error(compile_err);

        return OFFTYPEINVALID;
    }
    hyperscratch = NULL;
    if (hs_alloc_scratch(hyperdata, &hyperscratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space\n");
        hs_free_database(hyperdata);
        hyperdata=NULL;
        return OFFTYPEINVALID;
    }

return nextasciihyperscan(aim) ;
}

class Toshownhandler {
	public:
	short ellen,bytes;	
	int shift;
	OFFTYPE prevpos=OFFTYPEINVALID,startpos;
	union {
		Editor *ed;
		OFFTYPE pos;
	};
Toshownhandler(OFFTYPE st,short el,short by):ellen(el),bytes(by), startpos(st){};
inline	OFFTYPE getpos(unsigned long long endpos) {
		return startpos+((endpos-1)/ellen)*bytes+shift;
	}
};//Toshownhandler;
static int listshowhandler(unsigned int id, unsigned long long from, unsigned long long endpos, unsigned int flags, void *ctx) {
	Toshownhandler * const gegs=( Toshownhandler *)ctx;
	OFFTYPE oppos= gegs->getpos(endpos);

/* Better change hs_scan_stream so it jumps ellen before making the next match*/
	if(oppos!=gegs->prevpos) {
		gegs->ed->addmark(oppos,gegs->bytes);
		gegs->prevpos=oppos;
		}

    return 0;
}

static int onceshowhandler(unsigned int id, unsigned long long from, unsigned long long endpos, unsigned int flags, void *ctx) {
	Toshownhandler *const gegs=( Toshownhandler *)ctx;
	const OFFTYPE newpos=gegs->getpos(endpos);
	if(newpos<gegs->pos)
		gegs->pos=newpos;
	    return 1;
	}

#define onboundary Stype.Align


//OFFTYPE Editor::nexthypershown(const OFFTYPE bytestart)  {
OFFTYPE Editor::nexthypershown(SearchAim &aim)  {
    if(!hyperdata)  {
	 regexerrorstring=reinterpret_cast<const char *>(-1);
	output("No previous (hyperscan) search");
	return OFFTYPEINVALID;
    }
	ScreenPart *const datashown =aim.datashown;
	memorize();
	const int bytes=datashown->bytes;
	if(Stype.Unit) {

		aim.matchlen=bytes;
		return nextsingleregex(datashown,&Editor::hyperhit) ;
		}
	short boundaries;
	OFFTYPE off;
	if(onboundary) {
		off=(aim.start/searchbytes)*searchbytes+searchgrens;
		boundaries=1;
		}
	else {
		boundaries=bytes;//aim.align?
		off= aim.start;
		}
	if(off>filesize())
			return OFFTYPEINVALID;

	const int bs= blocksize();
	const int nrblock= ((bytes-1)/bs)+2;
	BLOCKTYPE blnr=off/bs;
	int inblock=off%bs;
const int nr1=nrblock-1;
extern unsigned long long startuptime;
SEARCHINTDEC(6000000000/startuptime,nr1,blnr+nrblock)
 hs_stream_t *streams[bytes];
 for(int i=0;i<boundaries;i++) {
    if (hs_open_stream(hyperdata, 0, streams+i) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate stream space\n");
	for(int j=0;j<i;j++)
		 hs_close_stream(streams[j], NULL, NULL, NULL);
	hs_free_scratch(hyperscratch);
        hs_free_database(hyperdata);
        hyperdata=NULL;
	return OFFTYPEINVALID;
    	}
	}

	const int bufsize=nrblock*bs;
	char * const binbuf=(char *) alignedalloc(bs, bufsize);
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
#warning Check on unitlen
int binlen=took-inblock;
bool list=islist;
//bool list=0;
Toshownhandler  toHandler(off,ellen,bytes);
match_event_handler handler;
void *const context=&toHandler;
if(list) {
	toHandler.ed=this;
	handler=listshowhandler;
	}
else {
	toHandler.pos=OFFTYPEINVALID;
	handler=onceshowhandler;
	}
int  takelen;
while((takelen=(((binlen-bytes+1)/bytes)*bytes))>0) {
	SEARCHINTER(blnr,{alignedfree(binbuf); for(int i=0;i<boundaries;i++) { hs_close_stream(streams[i], hyperscratch, NULL, NULL); }} );
	
for(int shift=0;shift<boundaries;shift++) {
	const int count=datashown->bin2search((unsigned char *)(binbuf+inblock+shift),formbuf,takelen);
	toHandler.shift=shift;
hs_error_t  ret;
    if ((ret=hs_scan_stream(streams[shift], formbuf, count,0,hyperscratch, handler, context)) != HS_SUCCESS) {
		for(int i=0;i<boundaries;i++)
			hs_close_stream(streams[i], hyperscratch, NULL, NULL);
		alignedfree(binbuf);
		if(ret!= HS_SCAN_TERMINATED) {
		    regexerrorstring=reinterpret_cast<const char *>(-1);
		    message("ERROR: Unable to scan input buffer");
			}
		else
		if(!list) {
			searchpos=toHandler.pos+1;
			aim.matchlen=1;
			return toHandler.pos;
			}
		hs_free_scratch(hyperscratch);
		hs_free_database(hyperdata);
        	hyperdata=NULL;
		return OFFTYPEINVALID;
	    }

	}
		
	int leftlen=binlen-takelen;
	memcpy(binbuf+bs-leftlen,binbuf+inblock+takelen,leftlen);
	searchpos= off=blnr*bs-leftlen;

	binlen=mem.getblocks(blnr,binbuf+bs,nr1)+leftlen;
	inblock=bs-leftlen;
	blnr+=nr1;
	}
int left=binlen-takelen;
for(int shift=0;shift<boundaries&&left>=bytes;shift++,left--) {
	const int count=datashown->bin2search((unsigned char *)(binbuf+inblock+shift+takelen),formbuf,left);
	toHandler.shift=shift;
hs_error_t  ret;
    if ((ret=hs_scan_stream(streams[shift], formbuf, count,0,hyperscratch, handler, context) )!= HS_SUCCESS) {
		for(int i=0;i<boundaries;i++)
			hs_close_stream(streams[i], hyperscratch, NULL, NULL);
		alignedfree(binbuf);
		if(ret!= HS_SCAN_TERMINATED)
		    message("ERROR: Unable to scan input buffer");
		else
			if(!list) {
				searchpos=toHandler.pos+1;
				aim.matchlen=1;
				return toHandler.pos;
				}
		hs_free_scratch(hyperscratch);
		hs_free_database(hyperdata);
        hyperdata=NULL;
	return OFFTYPEINVALID;
	    }

	}
for(int i=0;i<boundaries;i++) {
	toHandler.shift=i;
	hs_close_stream(streams[i], hyperscratch, handler, context);
	}
alignedfree(binbuf);
if(!list) {
	searchpos=toHandler.pos+1;
	aim.matchlen=1;
	return toHandler.pos;
	}
hs_free_scratch(hyperscratch);
hs_free_database(hyperdata);
        hyperdata=NULL;
return OFFTYPEINVALID;
}




//int Editor::hyperstart(const char *const pattern,const int patlen,const bool sense,const unsigned int mode) {
int Editor::hyperstart(SearchAim &aim,const unsigned int mode) {
    if(hyperdata) {
        hs_free_database(hyperdata);
    	hs_free_scratch(hyperscratch);
        hyperdata=NULL;
    	}
    hs_compile_error_t *compile_err;
//mem.setsearchstr((const signed char *)pattern,patlen);
   if (hs_compile((char *)aim.str,HS_FLAG_MULTILINE|
#ifdef DOTNLREGEX
			   HS_FLAG_DOTALL |
#endif
			   (aim.sense?0:HS_FLAG_CASELESS) |(islist?0:HS_FLAG_SINGLEMATCH ), mode, NULL, &hyperdata, &compile_err) != HS_SUCCESS) {
        message( "%s: \"%s\"", compile_err->message,aim.str);
	regexerrorstring=reinterpret_cast<const char *>(-1);
        hs_free_compile_error(compile_err);
        return -1;
    }
    hyperscratch = NULL;
    if (hs_alloc_scratch(hyperdata, &hyperscratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space\n");
        hs_free_database(hyperdata);
        hyperdata=NULL;
        return -1;
    }
    return 0;
}
//OFFTYPE Editor::hypershown(const OFFTYPE bytestart,const char *const pattern,const int patlen,const bool sense) {
OFFTYPE Editor::hypershown(SearchAim &aim) {

if((Stype.Unit?hyperstart(aim,HS_MODE_BLOCK):hyperstart(aim,HS_MODE_STREAM)) <0)
	return OFFTYPEINVALID;
return nexthypershown(aim);
}

