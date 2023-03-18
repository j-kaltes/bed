#ifndef SEARCHINTERRUPT_H
#define SEARCHINTERRUPT_H 1
#ifdef INTSEARCH
#include <signal.h>
#include <time.h>

#include "cursesinit.h"
#include "main.h"
extern int thesuspend(bool);
#define DEBUGP(...)
//#define DEBUGP(...) fprintf(stderr,__VA_ARGS__)
#ifdef _WIN32
#include <windows.h>
class WinQuery {
private:
LARGE_INTEGER wastime;
public:
WinQuery(){ 
wastime.QuadPart=-1LL;

}

bool timed(void) {
		switch(wastime.QuadPart) {
			case -1: QueryPerformanceCounter(&wastime);
			case -2: return false;
			default:
				LARGE_INTEGER nowtime;
				QueryPerformanceCounter(&nowtime);
				wastime.QuadPart=nowtime.QuadPart-wastime.QuadPart;
				return true;
			};
		};

void reset(BLOCKTYPE &count) {
extern LARGE_INTEGER  FrequencyPerformanceQuery;
	QueryPerformanceFrequency(&FrequencyPerformanceQuery); 
	long long past=(2*wastime.QuadPart);
	wastime.QuadPart=-2L;
	auto wascount=count;
	if(past>0) {
		if((count=(FrequencyPerformanceQuery.QuadPart*count)/past)>0)
			return ;
		}
	count=3*wascount;
	}
};
using TimeT = WinQuery;
#endif
// Activity to be timed
//usleep(10);

//
// We now have the elapsed number of ticks, along with the
// number of ticks-per-second. We use these values
// to convert to the number of elapsed microseconds.
// To guard against loss-of-precision, we convert
// to microseconds *before* dividing by ticks-per-second.
//

class SearchInterrupt {
private:
BLOCKTYPE INTCOUNT;
#ifdef _WIN32
TimeT timer;
#else
clock_t wastime;
#endif
int op,in;
#if defined(USETHREADS)
int background;
#endif
public:
SearchInterrupt(
#if defined(USETHREADS)
int back,
#endif
BLOCKTYPE jump,int opa=1,BLOCKTYPE first=0): 
INTCOUNT(((jump>0)?
#if  defined(USETHREADS)
jump
#else
jump/3
#endif
:10)*opa) ,
#ifndef _WIN32
wastime(-1),
#endif

op(opa),
in((opa>1)?(opa-first%opa): 0)

#if defined(USETHREADS)
,background(back) {
	if(background) 
		return;
#else
{
#endif
#ifdef PDCURSES
	raw();
#endif
	nodelay(stdscr, TRUE);
	};

~SearchInterrupt() {
#if defined(USETHREADS)
	if(background) 
		return;
#endif
   nodelay(stdscr, FALSE);
//noraw();
#ifdef PDCURSES
curses_init_curses.setscreen();
#endif
}
inline bool getkey(void) {	
	int ch;
	switch((ch=wgetch(stdscr))){
		case 's': 
DEBUGP("getch=s\n");
return true;

#ifdef HAS_SIGSETJMP 
		case 3: siglongjmp(screenjmp,SIGINT);break;
#else
		case 3: longjmp(screenjmp,SIGINT);break;
#endif
		case 26:{ thesuspend(true);
			resized();
			rewriteall();
#ifdef PDCURSES
	raw();
#endif
			 nodelay(stdscr, TRUE);
			;break;
		    };
		};
return false;
};
inline bool equal(BLOCKTYPE bl) {
	if(!((bl+in)%INTCOUNT) ) {
		DEBUGP("bl=" OFFPRINT " ",bl);
		if(getkey())
			return true;
#ifdef _WIN32
		if(timer.timed()) {
			timer.reset(INTCOUNT);	
			INTCOUNT=(INTCOUNT/op)*op;
			}
#else
		switch(wastime) {
			case -1L: wastime=clock();break;
			case -2L: break;
			default:
				const clock_t nowtime=clock();
				auto past=op*5*(nowtime-wastime);
				wastime=-2L;
				auto wascount=INTCOUNT;
				if(past==0||!(INTCOUNT=((op*CLOCKS_PER_SEC*(unsigned long long)INTCOUNT)/past)))
							INTCOUNT=wascount;
DEBUGP("wascount=%ld,past=%ld, op=%d, INTCOUNT=%ld\n",wascount,past,op,INTCOUNT);
				
			};
#endif
		};
	return false;	
 };
};
#ifdef INTSEARCH
#if defined(USETHREADS)
#define SEARCHINTER(b,doe) if((MEMORY backgroundsearch&&MEMORY endthread)||((!MEMORY backgroundsearch)&&inter.equal(b))) {doe;return OFFTYPEINVALID;}
#else
#define SEARCHINTER(b,doe) if(inter.equal(b))  {doe;return OFFTYPEINVALID;}
#endif
#else
#define SEARCHINTER(b,doe) 
#endif
#endif
#ifdef INTSEARCH
#if defined(USETHREADS)
#define SEARCHINTDEC(...) SearchInterrupt inter(MEMORY backgroundsearch,__VA_ARGS__); if((MEMORY backgroundsearch&&MEMORY endthread)||((!MEMORY backgroundsearch)&&inter.getkey()) ) return OFFTYPEINVALID;
//#define SEARCHINTDEC(...) SearchInterrupt inter(MEMORY backgroundsearch,__VA_ARGS__); if((MEMORY backgroundsearch&&MEMORY endthread)||((!MEMORY backgroundsearch)&&inter.getkey()) ) return OFFTYPEINVALID;
#else
//#define SEARCHINTDEC(pos,get,...) SearchInterrupt inter(get/3,__VA_ARGS__);if(inter.getkey() ) return OFFTYPEINVALID;
#define SEARCHINTDEC(...) SearchInterrupt inter(__VA_ARGS__);if(inter.getkey() ) return OFFTYPEINVALID;
//#define SEARCHINTDEC(get) SearchInterrupt inter(get/3 );if(inter.getkey() ) return OFFTYPEINVALID;

#endif
#else
#define SEARCHINTDEC(...) 
#endif


#endif
