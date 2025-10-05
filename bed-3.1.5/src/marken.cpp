#include "defines.h"
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
#include "editor.h"
#include "main.h"
#include "dialog.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include "screenpart.h"
#include "system.h"
#include "marks.h"
#include "screen.h"
#include "config.h"
const int MAXOFFTYPE=(sizeof(OFFTYPE)<8?8:sizeof(OFFTYPE));
extern void                     mychdir(const char *path);
extern char	*mygetcwd(char *name,int len) ;
//#define marx (*marksptr)

#ifdef USETHREADS
#define startwait sem_wait(&sema);
#define postwait sem_post(&sema);
#else
#define startwait 
#define postwait 
#endif
int Editor::addmark(OFFTYPE pos,int len) {
unsigned char r27[]={27,27,27};
#ifdef USETHREADS
		sem_wait(&sema);
#endif
		found++;	
		setmark( r27 ,3,pos,len);
#ifdef USETHREADS
	sem_post(&sema);
#endif
	return 0;
	}
int Editor::setmark(unsigned char *keys,int nr,OFFTYPE pos,uint32_t len) {
	char *des=myallocarp(char *,char,MAXMARKDAT+1);
	if((pos+MAXMARKDAT+1)>filesize()) {
		int atfile= filesize()-pos;
		getmem(pos,des,atfile);
		memset(des+atfile,0,MAXMARKDAT+1-atfile);
		}
	else {
		getmem(pos,des,MAXMARKDAT);
		des[MAXMARKDAT]='\0';
		}
	marksptr->putkeys(keys, nr, pos,des,len) ;
		return 0;
}
extern int mywgetch(WINDOW *win)  ;
extern bool mykbhit(void) ;
int Editor::keymark(void) {
	int i,key;
	unsigned char keys[100];
	output("Mark: Type key");
	wrefresh(editscreen);
	keys[0]=mywgetch(stdscr);
//#ifdef USE_KEYPAD
#if 1
	i=0;
	do {
		i++;
		keys[i]=(unsigned char)ERR;
		for(int z=0;z<2;z++) {
			if(mykbhit()) {
				keys[i]=mywgetch(stdscr);
				break;
				}
			usleep(5000);
			}
		} while((unsigned char)keys[i]!=(unsigned char)ERR);
	flushinp();
#else
	ndelay;
	wrefresh(stdscr);
	for(i=1;(key=mywgetch(stdscr))!=ERR;i++) {
		ndelay;  /*mywgetch does nocbreak */
	 	keys[i]=(unsigned char)key;
		}
	if(i==1&&*keys==27)
		keys[i++]=secondescape;
	flushinp();
	nocbreak();
	cbreak();
	refresh();
#endif
	OFFTYPE first,len;
	if(!(len=getselregion(&first))||len>UINT32_MAX) {
		output("Position marked");
		len=activemode()->bytes;
		first= filepos+editpos;
		}
	else {
		output("Selection marked");
		}
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	key= setmark(keys,i,first,(uint32_t)len);
#ifdef USETHREADS
	sem_post(&sema);
#endif
	return key;
	};

int Editor::tomark(void) {
	int key;
	struct keylookup::mark *mark;
	output("Goto mark: type key");
	wrefresh(editscreen);

while(1) {
	key=ggetch(stdscr);
	#ifdef USETHREADS
		sem_wait(&sema);
	#endif
	if((mark=marksptr->lookup(key)))
		break;
	if(marksptr->isstart()) {
		flushinp();
		beep();
		output("Goto mark: Unknown key");
		#ifdef USETHREADS
			sem_post(&sema);
		#endif
		return -1;
		};
	#ifdef USETHREADS
		sem_post(&sema);
	#endif
	}
	#ifdef USETHREADS
		sem_post(&sema);
	#endif
	flushinp();
	refresh();
	output("Goto mark: Moved");

   return mkstandout( mark->pos, mark->len);
	};

int Editor::nextmark(void) {
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	struct keylookup::mark *mark;
	do {
		mark=marksptr->next();
		if(!mark)  {
			beep();
			#ifdef USETHREADS
			sem_post(&sema);
			#endif
			return -1;
			};
		} while((editpos+filepos)==(OFFTYPE)mark->pos);
#ifdef USETHREADS
sem_post(&sema);
#endif

   return mkstandout( mark->pos, mark->len);
	};
   int Editor::mkstandout(const OFFTYPE pos,const int len) {
const	ScreenPart *activepart=parts[mode]->getsubpart();
const int off=getindatatype(activepart);
standout1= pos;
standout2= pos+len;
updated=0;
	if(off<pos)
		return toborderpos(pos-off) ;
	else
		return topos(pos) ;
}
int Editor::prevmark(void) {
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	struct keylookup::mark *mark;
	do {
		mark=marksptr->prev();
		if(!mark)  {
			beep();
			#ifdef USETHREADS
			sem_post(&sema);
			#endif
			return -1;
			};
		} while((editpos+filepos)==(OFFTYPE)mark->pos);
#ifdef USETHREADS
sem_post(&sema);
#endif

   return mkstandout( mark->pos, mark->len);
	};
#define STARTSHOW 10

extern int getindatatype(ScreenPart *thesub) ;
int Editor::sortmarks(void) {
	startwait
	marksptr->sort() ;
	postwait
	message("Sorted");
	return 0;
	}

int Editor::clearmarks(void) {
	int nr;
	if(marksptr->head==&marksptr->begin) {
		message("No marks to clear");
		return -1;
		}
	switch(getkeyinput("Do you really want to clear all marks? (y/n)")) {
		case 'y':
		case 'Y': 
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	nr=marksptr->deleteall() ; 
#ifdef USETHREADS
sem_post(&sema);
#endif
			message("%d marks deleted",nr);
			break;
		default: message("Cancelled!");
		}

	return 0;
	}

int Editor::deletemark(void) {
	struct keylookup::mark *mark= markselect<2>("Press return to delete mark") ;
	if(mark) {
		marksptr->select(mark);
		marksptr->rmmark(mark);
		marksptr->prev();
		return 0;
		}
	return -1;
	}

const int MARKFILEGEGSIZE=300;
char marksign[]="\276\320bed marks of ";
char marksend[]="\276\320bed end";
const int marklen=sizeof(marksign)-1;
const int marksendlen=sizeof(marksend)-1;

char markdirectory[BEDSMAXPATH]=".";	
#ifdef _WIN32
inline  unsigned  long long FILETIME2time_t( FILETIME *filetime)
   {
unsigned long long tim;
unsigned long long ltmp= (filetime->dwLowDateTime |((unsigned long long)filetime->dwHighDateTime << 32))-116444736000000000LL;
tim=ltmp/10000000L;
return tim;
   }
#endif
inline void writeofftype(const FILEHANDLE  han,const OFFTYPE get) {
const Offlarge off=get;
//	const char  * const pos=reinterpret_cast<const char *const>(&off);
	const char  * const pos=(const char *const)(&off);
	writetofile(han,pos,MAXOFFTYPE);
}
uint32_t MARKSVERSION=0;
int Editor::savemarks(void) {
FILEHANDLE handle;
	char cwdold[BEDSMAXPATH];
	char buf[BEDSMAXPATH];
	const char *ant="*.bedm";
	uint32_t nrmarks;
	strcpyn(buf,ant,BEDSMAXPATH);
	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(markdirectory);
	do {
		ant=getfile("Save Marks",buf) ;
		editup();
		if(!ant) {
			mygetcwd(markdirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return -1;
			}
		} while((handle=openfile(ant))==MY_INVALID_HANDLE_VALUE);
	mygetcwd(markdirectory,BEDSMAXPATH);
	mychdir(cwdold);
	struct keylookup::mark *mark,*first;
	startwait
	first=marksptr->next();
	marksptr->tobegin();
	int emptylen= MARKFILEGEGSIZE;
	char empty[MARKFILEGEGSIZE];
	memset(empty,0xAF,emptylen);
	writetofile(handle,marksign,marklen);	
	int flen=mem.filenamelen+1;
	writetofile(handle,mem.filename,flen);

//	writetofile(handle,empty,(BEDS_SIZE_T)MARKFILEGEGSIZE-flen-marklen-28);
//

	writetofile(handle,empty,(BEDS_SIZE_T)MARKFILEGEGSIZE-flen-marklen-32);
	writetofile(handle,(const char *)&MARKSVERSION,4);
	writeofftype(handle, filesize());
#ifdef _WIN32
	uint64_t  tim=(uint64_t)FILETIME2time_t(&mem.FileInfo.ftLastWriteTime);
#else
	uint64_t  tim=(uint64_t)mem.statdata.st_mtime;
#endif
	writetofile(handle,(char *)&tim,8);
	time((time_t *)&tim);
	writetofile(handle,(char *)&tim,8);
	writetofile(handle,empty,4);	
	
/*
4: Aantal marks
8: Datum
256: Filename
268
32
*/
	for(nrmarks=0;(mark=marksptr->next());nrmarks++) {
/*		*((OFFTYPE *)&pos	)=mark->pos;
		writetofile(handle,pos, MAXOFFTYPE);
*/
		writeofftype(handle, mark->pos);
		writetofile(handle,mark->des, MAXMARKDAT);
		writetofile(handle,empty, MAXMARKALL-MAXMARKDAT-4);
		writetofile(handle,(char *) &(mark->len), 4); /*added length*/
		writetofile(handle,&mark->nrkeys, 1);
		writetofile(handle,(char *)mark->keys, mark->nrkeys);
		if(mark->nrkeys<10)
			writetofile(handle,empty,10-mark->nrkeys);
		}
	writetofile(handle,marksend,marksendlen);
	setfilepos(handle,(OFFTYPE) (MARKFILEGEGSIZE-4L));
	writetofile(handle,(char *)&nrmarks,4);
	closehandle(handle);
	marksptr->select(first);
	marksptr->prev();
	postwait
	editup();
	message("%d marks saved to %s",nrmarks,ant);
	return 0;
	}
#include <sys/stat.h>
 #include <unistd.h> 


int Editor::readmarks(void) {
	char cwdold[BEDSMAXPATH];
	char buf[BEDSMAXPATH];
	const char *ant="*.bedm";
	int handle;
	strcpyn(buf,ant,BEDSMAXPATH);

	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(markdirectory);
	do {
		ant=getfile("Read Marks",buf) ;
		editup();
		if(!ant) {
			mygetcwd(markdirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return -1;
			}
#ifdef _WIN32
char namebuf[BEDSMAXPATH];
		arealpath(ant,namebuf);
#else
#define namebuf ant
#endif
		if((handle=open(namebuf,O_RDONLY BINARY_FLAG))<0) {
			message("Can't open %s",namebuf);
			}
		} while(handle<0);
	mygetcwd(markdirectory,BEDSMAXPATH);
	mychdir(cwdold);
	char regstr[marklen];
		
	if(read(handle,regstr,marklen)<marklen||memcmp(regstr,marksign,marklen)) {
		message("No bed marks file");
		close(handle);
		return -1;
		}
	struct stat sta;
        if(fstat(handle, &sta)==-1) {
		message("Stat failed"); return -1;
		}
	if(lseek(handle,sta.st_size-marksendlen,SEEK_SET)!=(sta.st_size-marksendlen)) {
		message("error seeking end of file");return -1;
		}
	char endstr[marksendlen];
	if(read(handle,endstr,marksendlen)<(marksendlen)||memcmp(endstr,marksend,marksendlen)) {
		message("Can't recognise find end marks file");
		close(handle);
		return -1;
		}
		
	uint32_t marksversion,oldversion;
	memset((char *)&oldversion,0xAF,sizeof(oldversion));
lseek(handle,MARKFILEGEGSIZE-32,SEEK_SET);
	read(handle,&marksversion,4);

uint32_t  nrmarks;

	if(lseek(handle,MARKFILEGEGSIZE-4,SEEK_SET)!=(MARKFILEGEGSIZE-4)) {
		message("lseek nr marks failed");return -1;
		}
	if(read(handle,&nrmarks,4)!=4) {
		message("read nrmarks failed");return -1;
		}
	struct {
	Offlarge pos;
	char rest[MAXMARKALL+1];
	} gegs;
	int alg= MAXOFFTYPE+MAXMARKALL+1;
//	char *bufptr=reinterpret_cast<char *>(&gegs.pos);
	char *bufptr=(char *)(&gegs.pos);
	OFFTYPE filsiz=filesize();
	char  *des;		
	char nrkeys=0;
	int errors=0;
	unsigned char keys[256];
	uint64_t i;
	int mlen=1;
	for(i=0;read(handle,bufptr,alg)>=alg;i++) {
//		pos=*((OFFTYPE*)bufptr);
		nrkeys=bufptr[MAXOFFTYPE+MAXMARKALL];
		if(marksversion!=oldversion)
		 	mlen=*reinterpret_cast<uint32_t *>(bufptr+MAXOFFTYPE+MAXMARKALL-4);
		if(gegs.pos<0||gegs.pos>=filsiz) {
			message("Offset " OFFPRINT " outside file limits",gegs.pos);
			lseek(handle,maxnum(10,nrkeys),SEEK_CUR);
			errors++;
			}
		else {
			des=myallocarp(char *,char,MAXMARKDAT+1);
			memcpy(des,bufptr+MAXOFFTYPE,MAXMARKDAT);
			des[MAXMARKDAT]='\0';
			if(read(handle,keys,nrkeys)<nrkeys) {
				message("Error reading keys for pos "   OFFPRINT,gegs.pos); 
				myfree (des);
				return -1;
				}
			if(nrkeys<10) {
				lseek(handle,10-nrkeys,SEEK_CUR);
				}
			startwait
			marksptr->putkeys(keys, nrkeys, gegs.pos,des,mlen) ;
			postwait
			}
		}
	close(handle);
	editup();
	if(i!=nrmarks) {
		message("Found %d instead of %d marks",i,nrkeys);
		}
	if(errors)
		message("%d marks: %d added, %d outside limits",nrmarks,nrmarks-errors,errors);
	else
		message("%d marks added ",nrmarks);
	return 0;
	}


#include <assert.h>
int Editor::subtractmarks(void) {
	char cwdold[BEDSMAXPATH];
	char buf[BEDSMAXPATH];
	const char *ant="*.bedm";
	int handle;
	strcpyn(buf,ant,BEDSMAXPATH);

	mygetcwd(cwdold,BEDSMAXPATH);
	mychdir(markdirectory);
	do {
		ant=getfile("Subtract Marks",buf) ;
		editup();
		if(!ant) {
			mygetcwd(markdirectory,BEDSMAXPATH);
			mychdir(cwdold);
			return -1;
			}
#ifdef _WIN32
char namebuf[BEDSMAXPATH];
		arealpath(ant,namebuf);
#else
#define namebuf ant
#endif
		if((handle=open(namebuf,O_RDONLY BINARY_FLAG))<0) {
			message("Can't open %s",namebuf);
			}
		} while(handle<0);
	mygetcwd(markdirectory,BEDSMAXPATH);
	mychdir(cwdold);
	char regstr[marklen];
		
	if(read(handle,regstr,marklen)<marklen||memcmp(regstr,marksign,marklen)) {
		message("No bed marks file");
		close(handle);
		return -1;
		}
	struct stat sta;
        if(fstat(handle, &sta)==-1) {
		message("Stat failed"); return -1;
		}
	if(lseek(handle,sta.st_size-marksendlen,SEEK_SET)!=(sta.st_size-marksendlen)) {
		message("error seeking end of file");return -1;
		}
	char endstr[marksendlen];
	if(read(handle,endstr,marksendlen)<(marksendlen)||memcmp(endstr,marksend,marksendlen)) {
		message("Can't recognise find end marks file");
		close(handle);
		return -1;
		}
uint32_t  nrmarks;

	if(lseek(handle,MARKFILEGEGSIZE-4,SEEK_SET)!=(MARKFILEGEGSIZE-4)) {
		message("lseek nr marks failed");return -1;
		}
	if(read(handle,&nrmarks,4)!=4) {
		message("read nrmarks failed");return -1;
		}
	struct {
	Offlarge pos;
	char rest[MAXMARKALL+1];
	} gegs;
	int alg= MAXOFFTYPE+MAXMARKALL+1;
//	char *bufptr=reinterpret_cast<char *>(&gegs.pos);
	char *bufptr=(char *)(&gegs.pos);
	OFFTYPE filsiz=filesize();
	char nrkeys=0;
	int errors=0;
	unsigned char keys[256];
	uint64_t i,del=0,notdel=0;;
	for(i=0;read(handle,bufptr,alg)>=alg;i++) {
		nrkeys=bufptr[MAXOFFTYPE+MAXMARKALL];
		if(gegs.pos<0||gegs.pos>=filsiz) {
			message("Offset " OFFPRINT " outside file limits",gegs.pos);
			lseek(handle,maxnum(10,nrkeys),SEEK_CUR);
			errors++;
			}
		else {
			if(read(handle,keys,nrkeys)<nrkeys) {
				message("Error reading keys for pos "   OFFPRINT,gegs.pos); 
				return -1;
				}
			if(nrkeys<10) {
				lseek(handle,10-nrkeys,SEEK_CUR);
				}
			if(rmmarkpos(gegs.pos)>=0)
				del++;
			else
				notdel++;
			}
		}
	close(handle);
	editup();
	if(errors) {
		message("%d marks deleted, %d not present, %d outside limits",del,notdel,errors);
		assert(nrmarks==(del+notdel+errors));
		}
	else {
		message("%d marks deleted, %d not present",del,notdel);
		assert(nrmarks==(del+notdel));
		}
	return 0;
	}


int Editor::rmmarkpos(OFFTYPE pos) {
	struct keylookup::mark *mark,*first;
startwait	
	first=marksptr->next();
	marksptr->tobegin();
	while((mark=marksptr->next())) {
 		if(mark->pos==pos) {
			marksptr->select(mark);
			marksptr->rmmark(mark);
			marksptr->prev();
			postwait
			return 0;
			}
		}
	marksptr->select(first);
	marksptr->prev();
	postwait
	return -1;
	}

template<>  struct keylookup::mark *Editor::markselect<1>(const char *prompt)  {
//struct keylookup::mark * Editor::markselectwhole(const char *prompt) {
	static int size=STARTSHOW;
	static char **names=myallocarpr(char **,char*,STARTSHOW);
	struct keylookup::mark *mark,*first;
	int i,take=0,ant,tot;
	ScreenPart *activepart=parts[mode]->getsubpart();
#ifdef USETHREADS
	sem_wait(&sema);
#endif
	first=marksptr->iter->next; 
	mark= &marksptr->begin;
      int aloc=maxnum(((MAXMARKDAT/activepart->bytes)+1)*(activepart->maxres+activepart->apart+1),MAXFILES)+1+sizeof(mark);
	for(i=0;mark!=marksptr->head;i++) {
		mark=mark->next;
		if(first==mark)
			take=i;
		if(i>=size) {
			const int oldsize __attribute__ ((unused)) =size;
			size*=2;
			names=myrealloc(char **,names,char*,size,oldsize);
			}
		names[i]=myallocar(char,aloc);
		int slen=activepart->fromascii((unsigned char*)names[i], (unsigned char *)mark->des,MAXMARKDAT);
		if(slen<MAXMARKDAT)
			for(int s=slen;s<MAXMARKDAT;s++)
				names[i][s]=' ';
		for(int s=MAXMARKDAT;s<(MAXMARKDAT+3);s++)
			names[i][s]=' ';
		snprintf(names[i]+MAXMARKDAT+3,MAXFILES,numstr  ,mark->pos); 
		memcpy(names[i]+MAXFILES+1,((char *)&mark),sizeof(mark));
		}
#ifdef USETHREADS
sem_post(&sema);
#endif
	tot=i;
mark=nullptr;
	while((ant=selectitemindex(prompt,names,i,take))!=INT_MAX) {
			if(ant==(INT_MAX-1)) {
				menuresized();
				wrefresh(editscreen);
				}
			else {
				memcpy((char *)&mark	,names[ant]+MAXFILES+1,sizeof(mark));
				goto end;
				break; 
				 }
		}
end:
	for(i=0;i<tot;i++)
		myrefree(names[i]);

	editup();
	return mark;
	}

template <int typ> int Editor::showmarks(void) {
	int bufsize=30;
	char buf[bufsize];
	auto total= marksptr->gettotal();
	struct keylookup::mark *mark= markselect<typ>(total>1?(snprintf(buf,bufsize,"%d marks",total),buf):"1 mark") ;
	if(mark) {	
		mkstandout( mark->pos, mark->len);
#ifdef USETHREADS
	sem_wait(&sema);
#endif
		marksptr->select(mark);
		marksptr->prev();
#ifdef USETHREADS
sem_post(&sema);
#endif
		return 0;
		}
	return 1;
	}


//int Editor::showmarks(void) {return showmarks<0>();};
//int Editor::showmarkswhole(void) {return showmarks<1>();};
//int Editor::showmarksinc(void) {return showmarks<2>();};
template int Editor::showmarks<0>();
template int Editor::showmarks<1>();
template int Editor::showmarks<2>();
