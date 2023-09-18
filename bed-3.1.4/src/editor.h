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
#ifndef  EDITOR_H
#define EDITOR_H
#include <vector>
#include <features.h>
#ifdef USE_HYPERSCAN
#include <hs/hs.h>
#endif
#define HAS_BOOL
#ifndef NOSCREEN
#include "screen.h"
#endif
#ifdef USETHREADS
#define __GNU_VISIBLE 1
#include <pthread.h>
#include "nepsem.h"
#endif
#include "memory.h"
#include "marks.h"
#include "type.h"
#include "myalloc.h"
/*
#ifdef _WIN32
#include "win.h"
#endif
*/
#define GeenEditor
#define MAXANT 256
#include "maxmenus.h"
#define STARTMAXUNDO 5000
#include "viewbuf.h"
struct undo {
	OFFTYPE pos;
	OFFTYPE usedondisk;
	unsigned char *str;
	int len;
	};

typedef  int  ChTrans(const int ch);
template <ChTrans givelow>
inline int unequal(const void *onev,const void *twov,size_t len) {
	const char * one=static_cast<const char *>(onev);
	const char * two=static_cast<const char *>(twov);
	for(size_t i=0;i<len;i++)
		if(one[i]-givelow(two[i]))
			return 1;
	return 0;
//	return strncasecmp(static_cast<const char *>(one), static_cast<const char *>(two),len);
	}
inline int isup(const int ch) { return (ch>='A'&&ch<='Z');}
inline int islow(const int ch) { return (ch>='a'&&ch<='z');}
inline int tolow(const int ch) {return ch+(-'A'+'a');}
inline int toup(const int ch) {return ch+(-'a'+'A');}
inline int mklow(const int ch) { return (isup(ch)?tolow(ch):ch);}
//#define mklow tolower
inline int mkup(const int ch) { return (islow(ch)?toup(ch):ch);}
//#define mkup toupper

//extern void *memlowcpy (void *__restrict __dest, const void *__restrict __src, size_t __n) __THROW __nonnull ((1, 2));
inline void *memlowcpy(void * dest, const void * src, size_t len)   {
	char *const out=(char *)dest;
	const char *const in=(const char *)src;
	for(size_t i=0;i<len;i++)
		out[i]=mklow(in[i]);
	return dest;
	};

using cpytype = void *(void * , const void * , size_t ) ;
typedef int Memcmp_t(const void *one,const void *two,size_t len);
class Filter ;
class ScreenPart;

class Empty;
class keylookup;

class Editor;
#include "searchaim.h"

class Viewbuf;

#ifdef USE_HYPERSCAN
static int UnitHandler(unsigned int id, unsigned long long from,
		                        unsigned long long to, unsigned int flags, void *ctx) {
	*((int*)ctx)=to;
	return 0;
}
#endif
enum	 RegexT:uint8_t  {
	None=0,
	RegGNU=1
#ifdef USE_RE2
	,RegRE2
#endif
#ifdef USE_HYPERSCAN
		,RegHS
#endif
};
//typedef  struct {uint8_t  Forward=true; RegexT Regex=None;uint8_t Raw=true,Case=true, Align=false, Unit=false; } ProcelT ;
typedef  struct {uint8_t  Forward; RegexT Regex;uint8_t Raw,Case, Align, Unit; } ProcelT ;

typedef struct SearchTyper{
union {
struct {uint8_t  Forward; RegexT Regex;uint8_t Raw,Case, Align, Unit; };
ProcelT procel;
};
uint8_t List=0, Edge=false, Keep=false,keepAlign=false,keepUnit=false;
RegexT RegexSearch=RegGNU,RegexReplace=RegGNU; 

SearchTyper(): Forward(true),  Regex(None), Raw(true),Case(true), Align(false), Unit(false) {};
} SearchType;
typedef SearchType SearchOpts;
typedef RegexT RegType;

//typedef Memory::SearchType::RegexT  RegType;
#include "selectsearch.h"
#ifdef USETHREADS
struct threadgegs {
	Editor *ed;
	SearchAim aim;
	};
#endif
class Editor {

	public:
		WINDOW *editscreen;
		int fromtop,numberof;
int visualwidth,visualoffset,virtualwidth, visualunit;
// Declaraties
struct Menugeg {
	 const char *name;
	 const char *key;
	int (Editor::*proc)(void);
	};
//Static 
	static  int (Editor::*proc[])(void);
	static  const char *procnames[];

	static  int (Editor::**dynproc)(void);
	static  const char **dynprocnames;
	static int dynprocnr;

	static int procnr;
//DATA:
	Memory mem;
/*s/^\([^(]*\)Editor::/\1 GeenEditor /g

*/
void  GeenEditor enlargeparts(void);
	ScreenPart **parts;
	int maxparts;
	int nrparts;
	int mode;
//	enum {ASCII,HEX} mode;
//	char *viewbuf;
	Viewbuf view;
	int changed,modified;
	OFFTYPE filepos ,lastgotopos;
	long editpos,oldpos,updated;
	int dofromfile;
	int commandmode;
	int allheight,Screenheight,Screenwidth,edittop;
	int shouldreinit;
int modechanged(void); 
	int nrx,nry;
	int getcols(void) ;
	int distribute(void) ;

static SearchType defaultsearch;
SearchType Stype=defaultsearch;


 //inline searchert searchproc(const SelectProcT proc) const ;
 
 searchert searchproc( ProcelT  proc) const  ;
searchert  usesearch={nullptr,nullptr};
	OFFTYPE searchpos;
      struct re_pattern_buffer *searchexpr=NULL;
      struct re_registers *searchregs=NULL;
	int tablebase[256];
	int *table;
	int searchlen=0;
	char *searchstr=NULL;
	ScreenPart *searchpart;

	 SearchAim lastaim;
int backgroundsearch;
//template <void *cpy(void *, const void *, size_t n)=memcpy>
//void setsearchstr(const signed char *const str,const int len);
/*
template <void *cpy(void *, const void *, size_t n)=memcpy>
void setsearchstr(signed char *& str,const int len) { 
       	if(searchstr)
       		myfree( searchstr);
       	searchstr =myallocarp(char *,char,len+1);
	cpy(searchstr,str,len);
       	searchstr[len]='\0';
	searchlen=len;
	str=searchstr;
	}
*/
//template <auto cpy=memcpy>
template <void *cpy(void * , const void * , size_t ) =memcpy>
void setsearchstr(SearchAim &aim) {
       	if(searchstr)
       		myfree( searchstr);
       	searchstr =myallocarp(char *,char,aim.len+1);
	DEBUGGING("setsearchstr(%.9s#%d)\n",aim.str,aim.len);
	cpy(searchstr,aim.str,aim.len);
       	searchstr[aim.len]='\0';
	searchlen=aim.len;
	}
void initsearch( void) ;
//OFFTYPE search(OFFTYPE beg,signed char *str,int len) ;
int setsearchgegs(SearchAim &aim);


//OFFTYPE nextsearch(SearchAim &aim);

 OFFTYPE nextsearcher(SearchAim &aim) ;
int nextsearch(SearchAim &aim);
OFFTYPE backicaseregexsearch(SearchAim &aim);
OFFTYPE backregexsearch(SearchAim &aim);
OFFTYPE nextbackregexsearch(SearchAim &aim);



OFFTYPE icaseregexsearch(SearchAim &aim);
OFFTYPE regexsearch(SearchAim &aim);
OFFTYPE nextforwardregexsearch(SearchAim &aim);
#ifdef USE_RE2
RE2 *re2pattern=NULL;

OFFTYPE re2search(SearchAim &aim) ;
OFFTYPE re2searchnext(SearchAim &aim);
//int re2match(const char *data,const int len,int *offset) ;
inline int re2match(const char *data,const int len,int *offset) {
using namespace re2;
	StringPiece word,input(data,len);    
	if(re2pattern->Match(input, 0, len, RE2::UNANCHORED, &word, 1)) {
		*offset= word.data()-data;
		return word.length();
	}
	return -1;
}
/*
inline int re2match(const char *data,const int len,int *offset) {
using namespace re2;
	StringPiece word,input(data,len);    
	if(RE2::PartialMatch(input, *re2pattern,&word)) {
		*offset= word.data()-data;
		return word.length();
	}
	return -1;
}
*/
#endif

template <Memcmp_t amemcmp,ChTrans givelow> inline OFFTYPE nextforwardsearch(SearchAim &aim);

template <Memcmp_t amemcmp> inline OFFTYPE nextbacksearch(SearchAim &aim);

//template <Memcmp_t amemcmp=memcmp> inline OFFTYPE forwardbytealign(SearchAim &aim) ;
/*
template <bool forward,bool sens>
inline OFFTYPE bytealign(SearchAim &aim); 
*/
/*
template <bool forward,bool sens,cpytype cpy = sens?memcpy:memlowcpy, Memcmp_t cmp = sens?memcmp:unequal<mklow> >
inline OFFTYPE bytealign(SearchAim &aim) ;
*/

//template <bool forward,cpytype cpy, Memcmp_t cmp> inline OFFTYPE bytealign2(SearchAim &aim) ;

//template <OFFTYPE (Editor::*proc)(SearchAim &),cpytype cpy> inline OFFTYPE bytealign2(SearchAim &aim) ;

template <bool forward,bool sens> inline OFFTYPE bytealign(SearchAim &aim) ;
//	return bytealign2<forwardbytealign<unequal<mklow>>,memlowcpy>(aim);
	
/* {
	return bytealign2<forward,memcpy,memcmp>(aim);
	} */
template <bool sens> inline OFFTYPE forwardbytealign(SearchAim &aim) ;

template <Memcmp_t amemcmp> inline OFFTYPE nextforwardbytealign(SearchAim &aim) ;

template <bool sens> inline OFFTYPE backwardbytealign(SearchAim &aim) ;
template <Memcmp_t amemcmp> inline OFFTYPE nextbackwardbytealign(SearchAim &aim) ;
//s/(.*$/(SearchAim \&aim);/g
OFFTYPE icasesearch(SearchAim &aim);
//template <decltype(&Editor::icasesearch) proc,cpytype cpy>
//inline OFFTYPE bytealign2(SearchAim &aim) ;

template <cpytype cpy>
inline void bytealigngegs(SearchAim &aim) ;
OFFTYPE nextforwardicasesearch(SearchAim &aim);
OFFTYPE casesearch(SearchAim &aim);
OFFTYPE nextforwardsearch(SearchAim &aim);
OFFTYPE backsearch(SearchAim &aim);
OFFTYPE nextbacksearch(SearchAim &aim);
OFFTYPE backicasesearch(SearchAim &aim);
OFFTYPE nextbackicasesearch(SearchAim &aim);


#ifdef USETHREADS
#ifndef USE_PTHREAD_CANCEL
bool endthread=false;
#endif
	sem_t sema;
	pthread_t threadsearch;
struct  threadgegs gegsthread;
	void stopthread(void); 
#endif
	int found;
#define MAXNUMSTR 12
/* Byte number procedures */
	int maxnumstr, numbase;
	char numstr[MAXNUMSTR];
	inline int startnum(void) {return 0;};
	int putnumbase(int base);
	void number(void);
	void numline(int y);

	int  GeenEditor nrnumber(void);
	inline int top(void) {return 0;};

int resetnumbase(void) ;
OFFTYPE standout1=0, standout2=0;
bool searchhit(int pos) ;
/*
struct block {
	unsigned char *begin;
	int last,free,len,lowest;
	struct block *prev;
	};
*/
//struct block *now,*chunkbefore;


//struct  undo undobuf[MAXUNDO];
int MAXUNDO;
struct  undo *undobuf;
int lastundo,redos,beginundo,undohalf;
void initundo(void) ;
void delundo(void) ;


inline unsigned char *expandchunk(unsigned char *ptr,int len,int oldlen) {
	return myrealloc(unsigned char *,ptr,unsigned char,len,oldlen);
	}
inline struct undo * GeenEditor getnextundo(void) ;


//int  GeenEditor popundo(unsigned char **buf,OFFTYPE *pos) ;

int popundo(unsigned char **buf,OFFTYPE *pos,OFFTYPE *diskused) ;
int undoone(void) ;
int  GeenEditor strtoundoer(OFFTYPE pos,int size) ;
int  GeenEditor strtoundo(OFFTYPE pos,int size) ;
int  GeenEditor addundo(OFFTYPE pos,unsigned char ch) ;
int  GeenEditor undopos(void)  ;

int inundo(OFFTYPE pos,unsigned char *buf,int size) ;
int  GeenEditor appendstrtoundo(OFFTYPE pos,char *str, int size) ;
int  GeenEditor appendtoundo(OFFTYPE pos, int size) ;
int      GeenEditor extendtoundo(OFFTYPE oldsize);

int	 GeenEditor extendtoundoonly(OFFTYPE oldsize) ;


//PROCEDURES

#undef prodefname
//#define prodefname(name,pro)  int pro(void);
#define prodefname(name,pro)  //int pro(void);
#undef prodef
#define prodef(pro)  int pro(void);
#include "procedures.h"
#undef prodef
	int processkey(void);
int processkeyonce(void) ;

	Editor(const char *filename);
	Editor() = delete;
	Editor(Editor const &) = delete;
	Editor& operator=(Editor const&) = delete;
	~Editor() ;
	int reinit(void);
	int place(int);
//	int changechar(int pos,unsigned char ch);
inline int  GeenEditor printchar(int pos) ;
	int changestr(int pos,unsigned char *str,int size);
	void doexit(void);
	void nocursor(void);
int  GeenEditor fromfile(OFFTYPE pos);
	int fromfile(void) ;
	int geteditpos(void);
	int lowborder(void);

	OFFTYPE getfilepos(void);

/* Geometry */

int  GeenEditor toborderpos(OFFTYPE pos) ;
	int  GeenEditor toborder(OFFTYPE pos) ;
	inline int cols(void) { return nrx;};
	inline int rows(void) { return nry;};

	inline int x(void) { return (editpos%nrx);};
	inline int y(void) { return (editpos/nrx);};
	int writebuf(void);
	void cursorpos(void) ;
	inline void resetpos(void);
	int topos(OFFTYPE pos) ;
int  GeenEditor pretopos(OFFTYPE pos);
	void putline(int y) ;
	int nomessage;
	inline void textovermessage(void);
	inline void restoremessageline(void);
	 void clearmessage(void) ;

	int getinput(const char question[],char *answer) ;
	int inputget(char *answer,const char *format,...) ;

#define NEXTFILE 888
#define PREVFILE 889
#define ASKFILE  890
#define LASTFILE 891
#define FIRSTFILE 891
#define FILENR	892
#define MAXFILES 60


bool searchdontedit(void) ;
int askmenufile(void) ;

	inline int switchcursor(void) ;
	int nextfast(void) ;

	int save(void);
	int  GeenEditor saveaser(const char *ant) ;

//	int extendfile(int len) ;
int extendfile(OFFTYPE len) ;



int  GeenEditor toscreen(void) ;


OFFTYPE  GeenEditor getmem(OFFTYPE pos,char *buf,OFFTYPE len);
OFFTYPE  GeenEditor putmem(OFFTYPE pos,char *buf,OFFTYPE len);



	short searchbytes,searchgrens,indatatype;

FILEHANDLE openfile(const char *ant) ;

FILEHANDLE  GeenEditor tmpopenfile(const char *ant) ;
	char *formatsearch=nullptr,*searchdataname;
int formatsearchlen=0;


inline OFFTYPE dosearch(char *ant,int len) {
	return dosearch(searchstartpos(),ant,len);
	}
	OFFTYPE dosearch(OFFTYPE pos, char *ant,int len) ;

//typedef SearchType::RegexT RegType;
//OFFTYPE dosearcher(ScreenPart *part,OFFTYPE pos, char *ant,int len,int *matchlen) ;
//OFFTYPE dosearcher(ScreenPart *part,OFFTYPE pos, char *ant,int len,int *matchlen,bool doreplace=false) ;
/*
	int searchregex(short yes);
	int searchforward(short yes);
	int searchcase(short yes) ;
	int searchonboundary(short yes) ;
	int searchedge(short yes);
	*/
//	int searchlist(char *ant,int len) ;

#if !defined( USETHREADS) || defined(BIOSDISK) ||defined(LONGTHREADS)
int  GeenEditor searchlist(ScreenPart *part,char *ant, int len) ;
#endif
	int  GeenEditor searcher(int askrepl);
//int  GeenEditor searchlister(char *ant,int len,OFFTYPE pos) ;
//int  GeenEditor searchlister(ScreenPart *part,char *ant,int len,OFFTYPE pos) ;
int searchlist(SearchAim &aim) ;
int searchlister(SearchAim &aim) ;
OFFTYPE dosearcher(SearchAim &aim) ;
int replacer(char *rep,OFFTYPE foundpos,SearchAim &aim) ;
#ifdef USETHREADS

int startthreadsearch(SearchAim &aim) ;
#endif
	char *replacestr=NULL;
	int replacestrlen=0;
int unitinelpos=0,unitformlen=0;
#ifdef USE_RE2
char *re2replacestr=NULL;
int re2replace(OFFTYPE pos,int matchlen, char *str,int len  ) ;
int formre2replace(OFFTYPE pos,int matchlen, char *str,int len  ) ;
int asciire2replace(OFFTYPE pos,int matchlen, char *str,int len  ) ;
OFFTYPE re2shown(SearchAim &aim) ;
OFFTYPE nextre2shown( SearchAim &aim) ;

//bool re2searchgegs(const signed char *const str,int len,bool sense) ;
bool re2searchgegs( SearchAim &aim) ;
#endif
const char *replace(char *input,int inlen,int *outlen) ;
//inline void replacehere(OFFTYPE pos, int matchlen,char *str,int len  ) ;

inline void replacehere(OFFTYPE pos, SearchAim &aim,char *str,int len  ) ;
OFFTYPE searchswitch(SearchAim &aim) ;
OFFTYPE nextsearchlast(SearchAim &aim) ;
//int replacer(char *rep,OFFTYPE foundpos,int matchlen) ;
//int  GeenEditor replacer(ScreenPart *part,char *rep,OFFTYPE foundpos) ;

	int endedit(void);
	int closefile(void);

	int addbefore(DataType type,int to,int len,int order);
	inline int addafter(DataType type,int to,int len,int order) { return addbefore(type,to+1, len,order); }
 
	int tomode(DataType type,int len,int order) ;
	int  GeenEditor tomodenr(int newmode) ;
//	int tomodeget(void) ;
 void editup(void) ;

	void filesup(void);



//	long digfrom(char *ant); to OFFTYPE??
//s/maxedit/screenmax/g
	inline int screenmax(void) { return ((nrx)*(nry));}

	int screensize(void);
	 void memorize(void);


//	long selects[2];
	static OFFTYPE lensel;
	static	int	seloffset;
	static char *selbuf;
	OFFTYPE selectpos;
	int el;
int  GeenEditor selected(OFFTYPE pos) ;

OFFTYPE  GeenEditor getselregion(OFFTYPE *start) ;
//int  GeenEditor setmark(unsigned char *keys,int nr,OFFTYPE pos) ;

int setmark(unsigned char *keys,int nr,OFFTYPE pos,uint32_t len) ;

int addmark(OFFTYPE pos,int len) ;
int mkstandout(const OFFTYPE pos,const int len) ;
int rmmarkpos(OFFTYPE pos) ;
keylookup * const marksptr=new keylookup;

template <int vers> struct keylookup::mark * markselect(const char *prompt) ;

template <int typ> int showmarks(void) ;


//struct keylookup::mark * markselectwhole(const char *prompt) ;
//struct keylookup::mark *markselectinc(const char *prompt) ;
/*
inline int defaultmark(void) {
	marksptr->defaultmark(editpos+filepos);
	return 0;
	};
*/
#undef getch
 int getch(void) ;


#define scrollon(x,y) scrollok(x,y)
inline void overline();
/*
inline int output(const char info[]) { }
*/
int output(const char info[]) ;
int message(const char *format ...) ;
 int getkeyinput(const char *format ...) ;

 int  GeenEditor showposition(void) ;



int gotonrwin(int nr) ;


void windel(void) ;



struct menuel {
	int key;
	int (Editor::*proc)(void);
	char *name;
	char *alt;
	char keypos;
	};
struct menu {
	WINDOW *win;
	int len,nu,key;
	struct menuel *el;
	char *name;
	char keypos;
	int maxname;
	int barpos;
	};
void editontop(void) ;

int showmenu(void) ;
static void selectmenu(struct menu *m,int dir) ;
static struct menu *menumk(int x,int el,struct Menugeg pro[], const char *under) ;
static int  GeenEditor menustructs(void) ;
static WINDOW * GeenEditor rewritemenu(struct Editor::menu *m) ;

static int  GeenEditor deletemenu(int menunr) ;
static int  GeenEditor deletemenuitem(int menunr,int pos) ;
static int  GeenEditor addmenuitem(int menunr,const char *name,int pos, int ( Editor::*proc)(void),int insert=0,const char *keystr="") ;
static int  addmenu(const char *name,int menunr,int insert=0) ;


static struct menu *menulist[maxmenus];

static int menuiter,menunu;
//int addtobar(char *name) ;
static int  GeenEditor addtobar(struct Editor::menu *m) ;
static int menus(void) ;
int  GeenEditor fileok(void) ;
int  GeenEditor saveinode(void) ;
int  GeenEditor discard(void) ;
  OFFTYPE  GeenEditor getpart(OFFTYPE begin, OFFTYPE len, char * buf); 
   OFFTYPE  GeenEditor putpart(OFFTYPE begin, OFFTYPE len, char * buf);

OFFTYPE filesize(void);
void erefresh(void);


static int (Editor::*ineditproc[])(void);
static int (Editor::*inmenuproc[])(void);
int  GeenEditor therepeat(int nr) ;
char * GeenEditor getfilename(char *buf);

OFFTYPE  GeenEditor getselbuf(char **buf) ;
int  GeenEditor putselbuf(char *buf,int len) ;

OFFTYPE  GeenEditor getselect(void) ;
OFFTYPE  GeenEditor setselect(OFFTYPE pos) ;

OFFTYPE  GeenEditor posputbuf(OFFTYPE pos,char *buf,OFFTYPE len);
OFFTYPE  GeenEditor putbuf(char *buf,OFFTYPE buflen);


void  GeenEditor splitpos(OFFTYPE pos) ; 

int  GeenEditor menuresized(void);
int  GeenEditor getmode(DataType type,int len,int order) ;
int  GeenEditor installpart(ScreenPart *sub) ;
int  GeenEditor installpart(ScreenPart *sub,int separ) ;

int sizelabels(ScreenPart *sub,int kind,int nr,...) ;
int sizelabels(ScreenPart *sub,int kind,int nr,va_list ap,bool) ;
int  GeenEditor forcetruncate(void) ;
int  GeenEditor filetruncate(OFFTYPE vanaf);
int  GeenEditor savesel(const char *ant) ;
//int  GeenEditor saveshownname(const char *ant) ;

ssize_t saveshownname(const char *ant) ;
int  GeenEditor readshownname(const char *ant) ;
//OFFTYPE formsearch(OFFTYPE from,char *ant,short len) ;

//int nextsearch(int *matchlen) ;
//OFFTYPE memnextsearch(int *matchlen) ;
//OFFTYPE  GeenEditor memnextsearch(ScreenPart *datashown) ;
//OFFTYPE  GeenEditor memsearch(OFFTYPE pos,char *str,int len) ;
//OFFTYPE  GeenEditor memsearch(ScreenPart *part,OFFTYPE pos,char *str,int len,int *matchlen) ;

//OFFTYPE searchswitch(SearchAim &aim) ;
//OFFTYPE nextsearchlast(SearchAim &aim) ;

//int startthreadsearch(SearchAim &aim) ;
OFFTYPE memsearch(ScreenPart *part,OFFTYPE pos,char *str,int len,int *matchlen,bool doreplace=false) ;

OFFTYPE memnextsearcher(int *matchlen) ;
OFFTYPE searchsel(OFFTYPE , signed char *,int ,int *) ;
//int  GeenEditor nextsearching(ScreenPart *part) ;
ScreenPart *activemode(void) ;

int  GeenEditor getanswer(const char *question,char *answ,int anslen,int ansroom=5) ;
int  GeenEditor addtocontain(ScreenPart *part) ;
BLOCKTYPE blocksize(void);

int  GeenEditor addfilter(Filter *fil) ;
int  GeenEditor playmacrofile(const char *name) ;
int  GeenEditor playmacro(char *buf,int len);

int  GeenEditor savecontain(void) ;
FILEHANDLE  GeenEditor getsavedatahandler(const char *question) ;
int  GeenEditor savealldatatypes(void) ;
int  GeenEditor loaddatatype(char *gegs,int endf,int addfield) ;
int  GeenEditor proconname(char *str) ;
int  GeenEditor configASCII(ScreenPart *sub,int kind);
int  GeenEditor partconfig(ScreenPart *sub,int kind=1) ;
ScreenPart * GeenEditor replacepart(ScreenPart *part,int pos=-1) ;

int gnureplacehere(OFFTYPE pos, char *str,int len  ) ;
int  GeenEditor makevisible(ScreenPart *part,int pos) ;

//int GNUhit(const char *data,const int len,int*) ;
//int GNUhit(const char *data,const int len,int *offset) ;

OFFTYPE gnubackunit(SearchAim &aim) ;
inline int GNUhit(const char *data,const int len,int *offset) {
	*offset= re_search(searchexpr, data, len, 0, len-1, searchregs);
	if(*offset<0)
		return -1;
	return searchregs->end[0]-searchregs->start[0];
	}
OFFTYPE GNUformsearch(SearchAim &aim);
OFFTYPE nextGNUregexshown(SearchAim &aim);
OFFTYPE nextbackshown(SearchAim &aim);
OFFTYPE backshownsearch(SearchAim &aim);



//OFFTYPE nextsingleregex(ScreenPart *datashown,int (*matcher)(const char *const,const int))  ;

	OFFTYPE backunit(ScreenPart *part,OFFTYPE pos,char *pattern,int len);
OFFTYPE nextbackunit(void);
//OFFTYPE nextbacksingleregex(ScreenPart *const datashown,int (Editor::* const matcher)(const char *const,const int,int*))  ;

template <int (Editor::* const matcher)(const char *const,const int,int *)> OFFTYPE nextbacksingleregex(SearchAim &aim)  ;

OFFTYPE re2backunit(SearchAim &aim) ;
OFFTYPE nextsingleregex(ScreenPart *const datashown,int (Editor::* const matcher)(const char *const,const int,int*))  ;
//OFFTYPE nextsingleregex(ScreenPart *datashown,int (Editor::*matcher)(const char *const,const int))  ;
//OFFTYPE nextsingleregex(ScreenPart *datashown) ;

int  GeenEditor emptyconfig(Empty *emp,int kind) ;
BEDS_SIZE_T getblock(BLOCKTYPE blocknr,char *buf) ;
#ifdef USE_HYPERSCAN
hs_database_t *hyperdata;
hs_scratch_t *hyperscratch ;

OFFTYPE asciihyperscan(SearchAim &) ;
OFFTYPE nextasciihyperscan(SearchAim &) ;
OFFTYPE nexthypershown(SearchAim &) ;
OFFTYPE hypershown( SearchAim &) ;
OFFTYPE hyperbackunit(SearchAim &aim);
//int hyperstart(const char *const pattern,const int patlen,const bool sense,const unsigned int mode) ;

int hyperstart(SearchAim &aim,const unsigned int mode) ;
int hyperhit(const char *buf,const int len,int *offset) {
	*offset=0;
	if(hs_scan(hyperdata, buf,len, 0, hyperscratch ,UnitHandler, offset)!= HS_SUCCESS) {
		    message("hyperhit: Unable to scan input buffer");
		    return -1;
		}
	if(*offset>0)
		return len;
	else
		return -1;
    }
//int hyperhit(const char *buf,const int len,int *) ;
#endif
std::vector<ScreenPart *> progenitors(void) ;
private:
//					    pos=(mem.lastforward&1)?(edge?0:filepos+editpos+1): (edge?(mem.size()-1):(filepos+editpos-1));
//#define edge mem.Stype.Edge
OFFTYPE searchstartpos();
int ismessage;
int  GeenEditor showmessage(void) ;
int  GeenEditor realoutput(const char info[]) ;
OFFTYPE *previouspos;
int previouspositer, previousposhead;
public:
OFFTYPE getpreviouspos(void);
char justopened;

int insertshownpos(const OFFTYPE posf,const char *const gegs, const off_t endf) ;
int  GeenEditor insertshown(char *gegs, off_t endf) ;
#define mkmenunum(num) int GeenEditor menu##num##show(void);
#include "mkmenus.h"
#undef mkmenunum
};
#endif
