/*     Bed a Binary EDitor for Linux and for Rxvt running under Linux.       */
/*     Copyright (C) 1998  Jaap Korthals Altes <binaryeditor@gmx.com>      */
/*                                                                           */
/*     Bed is free software; you can redistribute it and/or modify           */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     Bed is distributed in the hope that it will be useful,                */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with bed; if not, write to the Free Software                    */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/*                                                                           */
/* Sun Dec  6 18:34:27 1998                                                  */

#ifndef FAKED_H
#define FAKED_H
#undef prodefname
#undef prodef
//#define prodefname(name,proc) int proc(void);
#define prodefname(name,proc) 

#define prodef(proc)	int proc(void);
#include "mainprocedures.h"
#undef prodefname
#undef prodef
int getselbuf(char **buf); 
int putselbuf(char *buf,int len);

/* Returns length of selection and puts start position in start. 
* start has to point to an OFFTYPE */
OFFTYPE getselregion(OFFTYPE *start); 
#define GeenEditor
///*s/^\([^(]*\)Editor::/\1 GeenEditor /g

OFFTYPE getselect(void) ;
OFFTYPE setselect(OFFTYPE pos) ;
#define putselect( pos) setselect(pos) 



OFFTYPE searchstartpos();

inline OFFTYPE dosearch(char *ant,int len) {
	return dosearch(searchstartpos(),ant,len);
	}
	OFFTYPE dosearch(OFFTYPE pos,char *ant,int len);
	int topos(OFFTYPE pos) ;
	int toborderpos(OFFTYPE pos) ;

char *getbuf(void); 
int screensize(void);
BLOCKTYPE blocksize(void);

int geteditpos(void); 
OFFTYPE getfilepos(void); 
char *getfilename(char *buf);

void filesup(void) ;
int toscreen(void) ;
void erefresh(void);

BEDS_SIZE_T getblock(BLOCKTYPE blocknr,char *buf) ;
OFFTYPE getmem(OFFTYPE pos,char *buf,OFFTYPE len);
OFFTYPE putmem(OFFTYPE pos,char *buf,OFFTYPE len);

int proceskeyonce(void) ;

int extendfile(OFFTYPE  len) ;
int forcetruncate(void) ;
int filetruncate(OFFTYPE newlast) ;

int getmode(DataType type,int len,int order) ;
ScreenPart *activemode(void);

OFFTYPE filesize(void) ; 
int save(void) ; 
 int saveaser(const char *ant) ;
 int savesel(const char *ant) ;

int playmacrofile(const char *);
int playmacro(char *buf,int len);

int message(const char *format ...) ;
int getkeyinput(const char *format ...) ;
int inputget(char *answer,const char *format,...) ;

int installpart(ScreenPart *part) ;
int installpart(ScreenPart *sub,int separ);
int addfilter(Filter *fil);

static int (Editor::*dynproc[])(void) ;
static const char *dynprocnames[];
static int dynprocnr;

//int shouldreinit;
int modechanged(void) ;
int saveshownname(const char *ant) ;
int readshownname(const char *ant) ;
int proconname(char *str) ;
ScreenPart *replacepart(ScreenPart *part,int pos=-1) ;
int sizelabels(ScreenPart *sub,int kind,int nr,...) ;

#endif


