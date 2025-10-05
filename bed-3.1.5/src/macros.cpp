#include "defines.h"
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
#include <stdio.h>
#include "screen.h"
#include "myalloc.h"
#include "macros.h"
#include <string.h>
#ifdef HAVE_SYS_MMAN_H 
#include <sys/mman.h>
#endif
#include "system.h"
extern int writemacroheader(int handle) ;
#ifdef HAVE_SYS_MMAN_H 
static int extendfile(int file, int newmax) {
		unsigned long endstr[]={0xFAFAFAFA};
		int newpos=newmax-sizeof(endstr);
		if(lseek(file,newpos,SEEK_SET)!=newpos) {
			return -1;
			}
		if(write(file,endstr,sizeof(endstr))!=sizeof(endstr)) {
			return -1;};
	return 0;
	}
#endif
	void Keystrokes::flush(void) {
		*inp=out=0;
		}	
	Keystrokes::Keystrokes(char *na):name(na) {
		inp=&inbuf;
		*inp=out=max=0;
		buf=NULL;
		file=-1;
		mmaptofile(na);
		}


#ifdef HAS_PREAD 
#define poswrite(handle,off,str, len) pwrite(handle,str,len,off)
#else
#define	poswrite(handle,off,str, len) ((lseek(handle,off,SEEK_SET),write(handle,str,len))
#endif

extern const int startmacrodata;

ssize_t getmacroheader(char  *macroheader);

#ifdef HAVE_SYS_MMAN_H 
	int Keystrokes::mmaptofile(char *name) {
		if(name) {
			file=open(name,O_RDWR|O_CREAT BINARY_FLAG,0600);
			if(file>0) {

					Strokekeytype  *tmp;
				int in=*inp;
				int newmax=((max*sizeof(Strokekeytype)+4096)/4096)*4096;
				if(extendfile(file,newmax)<0) {
					close(file);file=-1;return -1;
					}
			       if((tmp= ( Strokekeytype*)mmap(NULL, newmax,  PROT_WRITE|PROT_READ ,  MAP_SHARED, file,0))==MAP_FAILED) {
			       		close(file);
				       file=-1;
					return file;
					}
				getmacroheader(tmp);
				memcpy(((char *)tmp)+50,buf+out,(in-out)*sizeof(Strokekeytype));	
				in-=out;out=0;
				inp=(int *)(((char *)tmp)+46);
				*inp=in;
				max=(newmax/sizeof(Strokekeytype))-50;
				buf=(Strokekeytype *)((char *)tmp+50);
				}
			return file;
			}
		return -1;
		}
#else
int Keystrokes::mmaptofile(char *name) {
		return -1;
	}
#endif
	int Keystrokes::fill(Strokekeytype *keys,int len) {
		if(len<=0||!keys)
			return len;
		alloc(len);
		memmove(buf+(*inp),keys,len*sizeof(Strokekeytype));	
		(*inp)+=len;
		return len;
		}
	int Keystrokes::fill(Keystrokes &other) {
		return fill(other.buf+other.out,*(other.inp)-other.out);
		}
	Keystrokes::~Keystrokes()  {

#ifdef HAVE_SYS_MMAN_H 
		if(file>0) {
			if(munmap(((char *)buf)-50,(max*sizeof(Strokekeytype))+50)<0) {
				perror("munmap failed");
				}
			close(file);
			file=-1;
			}
		else 
#endif
{
			*inp=out=max=0;
			myfree(buf);
			}
		if(name) {
		//	remove(name);
			unlink(name);
			free(name);
			name=NULL;
			}
		}
/*
file format
0-49: header
50,..: data
max: dataitems Strokekeytype size

*/
	void Keystrokes::alloc(int len) {
		if(max<(*inp+len)) {

#ifdef HAVE_SYS_MMAN_H 
			if(file>0) {
				int newmax=2*((max*sizeof(Strokekeytype))+50),in=*inp;
				Strokekeytype  *ptr;	
				if(extendfile(file,newmax)<0|| 
#ifdef HAS_MREMAP
((ptr=(Strokekeytype  *) mremap (((char *)buf)-50,max*sizeof(Strokekeytype)+50, newmax, MREMAP_MAYMOVE))==MAP_FAILED)
#else
((ptr=(Strokekeytype  *)mmap(NULL,newmax, PROT_WRITE|PROT_READ ,  MAP_SHARED, file,0))==MAP_FAILED)|| (memcpy(ptr,((char *)buf)-50,max*sizeof(Strokekeytype)+50),munmap(((char*)buf)-50,(max*sizeof(Strokekeytype))+50),0)
#endif
) {
					ptr=myrealloc(Strokekeytype *,NULL,Strokekeytype,newmax,0) ;
					memcpy(ptr,buf+out,sizeof(Strokekeytype)*(*inp-out));
					file=-1;
					inp=&inbuf;
					*inp=in-out;
					out=0;
					buf=ptr;
					max=newmax/sizeof(Strokekeytype);
					}
				else {
					inp=(int *)(((char *)ptr)+46);
					buf=(Strokekeytype*)(((char *)ptr)+50);
					max=newmax/sizeof(Strokekeytype)-50;
					}
				}
			else 
#endif
{
				Strokekeytype *tmp=buf;
				int newmax=2*((max*sizeof(Strokekeytype))+100+len);
				buf=myrealloc(Strokekeytype *,tmp,Strokekeytype,newmax,max*sizeof(Strokekeytype)) ;
				max=newmax/sizeof(Strokekeytype);
				}
			}
		}
	void Keystrokes::add(int key) {
		if(*inp==out)
			*inp=out=0;
		alloc(1);
		buf[(*inp)++]=key;	
		}
	int Keystrokes::last(void) {
		if((*inp)<1)
			return KEYSTROKE_ERROR;
		return buf[(*inp)-1];	
		}
	int Keystrokes::get(void) {
		if(pending()) {
			int key= buf[out++];
			if(*inp==out)
				*inp=out=0;
			return key;
			}
		else
			return KEYSTROKE_ERROR;
		}
	int Keystrokes::peek(void) {
		if(pending()) {
			return buf[out];
			}
		else
			return  KEYSTROKE_ERROR;
		}

Keystrokes playkeys;

/*
int *keystrokes;
int keystrokeout=0,keystrokein,keystrokemax=0;
int *keystrokes;
#define isrecording
int recordkeysstrokes;
int recordkeystrokeout=0,recordkeystrokein,recordkeystrokemax=0;
*/
/*
int addstroke(int key) {
	if(keystrokein==keystrokeout)
		keystrokein=keystrokeout=0;
	if(keystrokemax<=keystrokein) {
		int *tmp=keystrokes;
		keystrokemax=2*(keystrokemax+10);
		keystrokes= myallocar(int ,keystrokemax);
		int keytmp=keystrokein-keystrokeout;
		memcpy(keystrokes,tmp+keystrokeout,sizeof(int)*(keytmp));
		keystrokeout=0;
		keystrokein=keytmp;
		}
	keystrokes[keystrokein++]=key;	
	}
*/
extern int mywgetch(WINDOW *win)  ;
extern int endrecolen;
