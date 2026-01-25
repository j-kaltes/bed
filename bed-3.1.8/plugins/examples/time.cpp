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

#ifdef GLOBAL
#include <stdio.h>
#include "../screenpart.h"
#include <time.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>

static char timeformat[]="%a %b %e %H:%M:%S ";
#ifndef HAS_GMTIME_R
#define	gmtime_r(t, tmbuf) memcpy(tmbuf,gmtime(t),sizeof(struct tm))
#endif
#ifndef HAS_LOCALTIME_R
#define	localtime_r(t, tmbuf) memcpy(tmbuf,localtime(t),sizeof(struct tm))
#endif

typedef    struct tm *(*Convtime_t)(const time_t *timep, struct tm *result);
typedef  time_t (*Mktime_t)(struct tm *tm);

template <Convtime_t convtime,Mktime_t amktime>
class  Time_t : public ScreenPart {	
	public:	 Time_t ( Editor *ed,int x1=0, int y1=0): ScreenPart( ed, 24 ,0, sizeof(time_t) ,x1, y1,1)	{
	struct tm tmbuf;
	tzset();
	time_t t=947585553;
#ifndef OWNTRANSTIME
	convtime(&t, &tmbuf);
	char formal[50]="";
 	strftime (formal,50,timeformat,  &tmbuf);
	maxres=strlen(formal)+5;
#endif
	};	
		declareparent 
//"Wed Oct 12 11:29:38 1988"

virtual int userconfig(int kind) override;
virtual	int chbytes(int b) override {
		int oldbytes=datbytes();
		datbytes(b);
		switch(datbytes()) {
			case 4:
			case 8:break;
			case -1: return 1;
			default: datbytes(oldbytes); return -1;
			};
		return datbytes();
		}
inline void dstlocal( struct tm *tim) const {
	tim->tm_isdst=-1;
	}

#ifndef HAVE_TIMEGM
inline void zonegmt(time_t *t,struct tm *tim) {
#ifdef TIMEZONENEEDED /* TO get GMT */
	*t-=timezone;
#else
	*t+=(tim->tm_gmtoff-(tim->tm_isdst>0?(tim->tm_isdst*60*60):0));
#endif
	}
#endif
virtual	int formal2bin(const char *formal, unsigned char *bin, int len) const override{	
	time_t t;
	struct tm tim;	
#ifdef OWNTRANSTIME
#include "timerommel.h"
#else

	memset(&tim,'\0',sizeof(tim));
	if(char *rest=strptime (formal, timeformat, &tim)) {
		int y=atoi(rest);
		tim.tm_year=y-1900;
		dstlocal(&tim);
		t=amktime(&tim);
if(t==-1)
	return -1;
#ifndef HAVE_TIMEGM
	zonegmt(&t,&tim);
#endif
	//	memmove(bin,(char *)&t,4);
		int byt=getbytes();
	if constexpr( sizeof(time_t)==4) {
		memcpy(bin,(char *)&t,sizeof(time_t));
		if(byt>(int)sizeof(time_t))
			memset(bin+sizeof(time_t),'\0',byt-sizeof(time_t));
			}
else	
		memcpy(bin,(char *)&t,byt);
	
	return 0; 
		}
else 
return -1;
#endif
	}
#define bin2error {errortime(formal);return -1;}
inline void errortime(char *formal) {
const char notshown[31]="Out of range                  ";
memcpy(formal,notshown,maxres);
}
#ifndef OWNTRANSTIME
virtual	int bin2formal(unsigned char *bin, char *formal, int len) override {	
	  struct tm tmbuf;
/*
long long tt=*reinterpret_cast<unsigned long long *>(bin);
const  long long maxfit=253402300799L;
const char notshown[25]="Too late                ";
if(tt>maxfit) 
	memcpy(formal,notshown,sizeof(notshown));
else {
*/	

if constexpr( sizeof(time_t)!=4) {
	int byt=getbytes();
	if(byt==sizeof(time_t)) {
		  if(!convtime(reinterpret_cast<time_t *>(bin), &tmbuf)) bin2error;
		}
			
	else {
		time_t t=0;
		memcpy(reinterpret_cast<void *>(&t),bin,byt);
		if(!convtime(&t, &tmbuf)) bin2error;
		}
		}
else {
	  if(!convtime(reinterpret_cast<time_t *>(bin), &tmbuf)) bin2error;
}
	int y=tmbuf.tm_year+1900;
	if(y>9999||y<-9999) bin2error
else {
 	  int len=strftime (formal,maxres+1,timeformat,  &tmbuf);
#ifdef __FreeBSD__
#warning "necessary??"
len=strlen(formal);
#endif
	if(!len)	 bin2error;
	if(y<0)
	  sprintf(formal+len,"%05d",y);
	else
	  sprintf(formal+len," %04d",y);
	}
	return 0;
	}
#else

	int bin2formal(unsigned char *bin, char *formal, int len) override{	
		char *tim=ctime((time_t*)bin);
		if(tim) {
			char *end= strchr(tim,'\n');
			if(end) {
				short l=end-tim;
				memmove(formal,tim,l);
				formal[l]=' ';
				}
			else
				strcpy(formal,tim);
			}
		return 0;	
		}
#endif
	int isel(const int ch) const override {
			return (isalnum(ch)||ch==':'||ch==' ');
			}
	ScreenPart *newone(void) const override {return new Time_t(*this);}
	};
/*
typedef Time_t<gmtime_r> timeGMT;
typedef Time_t<localtime_r> timeLocal;
*/

#ifndef HAVE_TIMEGM
#define timegm mktime
template<>	 inline void Time_t<localtime_r,mktime>:: zonegmt(time_t *t,struct tm *tim) { }
#endif
template<>	 inline void Time_t<gmtime_r,timegm>::dstlocal( struct tm *tim) const { }

/*

template<>	int Time_t<localtime_r,mktime>::formal2bin(char *formal, unsigned char *bin, int len) {	
	return 0;
	}
*/
//template <Convtime_t convtime,const char name[]> class Parent Time_t<convtime,name>::desc=Parent(name,&ScreenPart::desc);
#define deftype(name,func,make) template<> Parent Time_t<func,make>::desc=Parent(#name,&ScreenPart::desc); ScreenPart *get##name(Editor *ed) { return new Time_t<func,make>( ed); } datatypes deftime##name(#name,&get##name);
deftype(TimeLocal,localtime_r,mktime)
deftype(Time_t,gmtime_r,timegm)
/*
template <Convtime_t convtime,const char name[]>
ScreenPart *getTime_t(Editor *ed) { return new Time_t<convtime,name>( ed); } 
#define deftype(name,func) ScreenPart *get##name(Editor *ed) { return new Time_t<func,#name>( ed); } datatypes deftime##name(#name,&get##name)
deftype(TimeLocal,localtime_r);
deftype(Time_t,gmtime_r);
*/
/*
ScreenPart *gettimeLocal(Editor *ed) { return new timeLocal( ed); } 

datatypes deftimeLocal("timeLocal",&gettimeLocal);
*/
/*
parentd(ScreenPart,timeGMT) 
#include "../typelist.h"
datatype(timeGMT)
*/
#endif
#ifdef UNDER
template <Convtime_t convtime,Mktime_t amktime>
int Time_t<convtime,amktime>::userconfig(int kind) {
	return sizelabelsnl(kind,2,"32~bit",4,"~64bit",8);
	}

#endif
