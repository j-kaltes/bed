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

#ifndef MEMORY_H
#define MEMORY_H
#include <features.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <string.h>
#include <stdlib.h>

#ifdef USE_RE2
#include <re2/re2.h>
#endif
#include "defines.h"
#include "rbtree.h"
#include "offtypes.h"
#include "myalloc.h"
#if defined(__FreeBSD__)||defined( _WIN32 )
#define NEEDS_BLOCKMODE 1
#ifdef _WIN32
#define ALIGNMEMORY 1
#endif
#endif
#include "alignmem.h"
#ifdef  __cplusplus
extern "C" {
#endif
#ifdef HAS_RE_SEARCH
#ifdef HAVE_GNUREGEX_H
#include <gnuregex.h>
#else
#include <regex.h>
#endif
#else
#include "../regex-0.13/xregex.h"
#endif


#ifdef  __cplusplus
}
#endif
#define DEBUG DEBUGGING
#include "debug.h"
#define BIGEST   INT_MAX

#if !defined(USE_PREAD) && defined(USETHREADS)
#include "nepsem.h"

#ifdef HAS_SEMAPHORE 
#include <semaphore.h>
#define DESTROYUSELSEEK  sem_destroy(&lseeksem)
#else
#define DESTROYUSELSEEK  
#endif
#define DEFINEUSELSEEK sem_t lseeksem
#define INITUSELSEEK sem_init(&lseeksem,0,1)
#define STARTUSELSEEK sem_wait(&lseeksem)
#define	STOPUSELSEEK sem_post(&lseeksem)
#define	GSTOPUSELSEEK(memp) sem_post(&((memp)->lseeksem))
#else
#define	GSTOPUSELSEEK(memp)
#define DEFINEUSELSEEK 
#define DESTROYUSELSEEK  
#define INITUSELSEEK 
#define STARTUSELSEEK
#define STOPUSELSEEK 
#endif

extern int STBLOCKSIZE;


//#define DEBUG(...) fprintf(stderr,__VA_ARGS__)
//#define DEBUG(x...)
#undef minnum
#undef maxnum
#define minnum( x,y) ((x)>(y)?(y):(x))
#define maxnum( een, twee) (((een)>(twee))?(een):(twee))
typedef char Flag;
#ifdef BIOSDISK
#include "biosdisk.h"
#endif
#ifndef _WIN32
typedef int BOOL ;
#endif

class Editor;
class Memory {
#ifdef USE_WINFILE
bool	used_dasd_io;
#endif
public:
#ifdef BIOSDISK
	Diskinfo *infodisk;	
#endif
private:
#ifdef NEEDS_BLOCKMODE
	int blockreadonly;
#endif

        public:

DEFINEUSELSEEK ;
int BLOCKSIZE;
	FILEHANDLE filehandle;
	int errors,readonly;
Treel *gettreel(BLOCKTYPE blockiter) ;

#ifdef _WIN32
	BY_HANDLE_FILE_INFORMATION FileInfo;

int getfileinfo( BY_HANDLE_FILE_INFORMATION *FInfo ) ;
#else
	 STATTYPE statdata;

bool statmis() ;
#endif

	OFFTYPE filesize, diskfilesize{},useddiskfilesize{};


	char *filename;
	int filenamelen;
	Tree tree;
	int error(void) const {return errors;}
	inline bool isnewfile() { return filehandle== MY_INVALID_HANDLE_VALUE;};
	OFFTYPE size(void) const {return filesize;}
explicit	Memory(const char *name=NULL);

	Memory() = delete;
	Memory(Memory const &) = delete;
	Memory& operator=(Memory const&) = delete;
	~Memory();


//void setsearchstr(const signed char *const str,const int len) ;
void openfile(void) ;
void init(const char *name) ;
void initall(void) ;
//int reinit(const char *name) ;
//int reinit(void) ;

void endfile(void);
int empty(void) ;

int savenode(FILEHANDLE output, Treel *node) ;
	int saveall(void);
#if (defined(HAVE_SENDFILE) && defined(SEEK_DATA)) ||defined(USE_WINFILE)
	int nonsparsesaveto(FILEHANDLE handle);

#endif
	int saveto(FILEHANDLE handle);
	BEDS_SIZE_T saveto(FILEHANDLE handle,OFFTYPE beg,OFFTYPE len) ;

#if (defined(HAVE_SENDFILE) && defined(SEEK_DATA)) ||defined(USE_WINFILE)
BEDS_SIZE_T nonsparsesaveto(FILEHANDLE handle,OFFTYPE beg,OFFTYPE len) ;
#endif
	int touched(void) ;
	BEDS_SIZE_T getblock(BLOCKTYPE blocknr,char *buf) ;
OFFTYPE  getblocks(const BLOCKTYPE een,char *buf,const BLOCKTYPE nr) ;
OFFTYPE  putblocks(const BLOCKTYPE een,char *buf,const BLOCKTYPE nr) ;

	Flag mmapfailed=1;
BEDS_SIZE_T mmapgetblock(BLOCKTYPE blocknr,char *buf) ;
	BEDS_SIZE_T putblock(BLOCKTYPE blocknr,char *buf) ;
	BEDS_SIZE_T getblockpiece(OFFTYPE beg,BEDS_SIZE_T len,char *get) ;

#ifdef _WIN32
OFFTYPE getpartaligned(const OFFTYPE beg,const OFFTYPE len,char *const gegs) ;
#endif
	OFFTYPE getpart(OFFTYPE beg,OFFTYPE len,char *gegs) ;
BEDS_SIZE_T putblockpiece(OFFTYPE  beg,BEDS_SIZE_T len,char *get) ;
OFFTYPE putpart(OFFTYPE beg,OFFTYPE len,char *gegs) ;
void	setborder(OFFTYPE pos) ;
void extend(OFFTYPE newsize) ;
void undotruncate(OFFTYPE newsize,OFFTYPE useddisk) ;
void truncatefile(OFFTYPE pos) ;			/* v2 */
//f c r

inline BLOCKTYPE blocksize(void) const { return BLOCKSIZE;};

BEDS_SIZE_T realblockreadat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) ;
BEDS_SIZE_T blockreadat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) ;
inline	BEDS_SIZE_T readat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) ;

BEDS_SIZE_T writeat(FILEHANDLE output,char *ptr,BEDS_SIZE_T size,OFFTYPE offset) ;

int zerochange(void) ;
int changed(void) ;
int changednode(Treel *node) ;
void exploresize(FILEHANDLE file,OFFTYPE begin=0, OFFTYPE end=MAXDISCSIZE) ;


#ifdef BIOSDISK
OFFTYPE getregistrysize(Diskinfo *infodisk,OFFTYPE &begin,OFFTYPE &end) ;
BOOL setregistrysize(Diskinfo *infodisk,OFFTYPE filesize);
inline BOOL isbiosdisk() {return infodisk!=NULL;};

inline bool opendosdrive(const char *const filename,OFFTYPE &begin, OFFTYPE &end) ;
#endif
inline BOOL sizehigher(OFFTYPE sizefile) ;
inline BOOL testsize(OFFTYPE sizetest);
inline BOOL checkfilesize(OFFTYPE filsize,OFFTYPE &begin,OFFTYPE &end) ;
OFFTYPE win32diskfilesize(FILEHANDLE filehandle,OFFTYPE *,OFFTYPE *) ;
#ifdef __linux__
bool getsizelinux(OFFTYPE &begin,OFFTYPE &end) ;
#endif

inline void regfilesize(void ) ;

#ifdef USE_WINFILE
inline FILEHANDLE openreading(const char *filename) ;
inline FILEHANDLE openreadwrite(const char *filename) ;

#endif

FILEHANDLE reopen() ;
	};


extern BEDS_SIZE_T writetofile(FILEHANDLE output,const char *buf,BEDS_SIZE_T nr) ;

#ifdef _WIN32
inline ssize_t sgetfilesize(FILEHANDLE han) {
        return GetFileSize(han,NULL);
        }
inline OFFTYPE lgetfilesize(FILEHANDLE han) {
        DWORD high,low;
        low=GetFileSize(han,&high);
        return (OFFTYPE)high<<32|(OFFTYPE)low;
        }
/*
inline OFFTYPE lgetfilesize(FILEHANDLE han) {
LARGE_INTEGER siz;
GetFileSizeEx(han,&siz);
return siz.QuadPart;
}
*/

/*

LARGE_INTEGER siz;
GetFileSizeEx(han,&siz);
return siz.QuadPart;
BOOL WINAPI GetFileSizeEx(
  __in   HANDLE hFile,
  __out  PLARGE_INTEGER lpFileSize
);
ypedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } ;
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

*/
#else


inline ssize_t sgetfilesize(FILEHANDLE handle) {
        ssize_t ret=         lseek(handle,0L,SEEK_END);
        lseek(handle,0L,SEEK_SET);
	return ret;
        }
inline OFFTYPE lgetfilesize(FILEHANDLE handle) {
        OFFTYPE ret=          LSEEK(handle,(OFFTYPE)0L,SEEK_END);
         LSEEK(handle,(OFFTYPE)0L,SEEK_SET);
	return ret;
        }
#endif
inline int closehandle(FILEHANDLE han) {
#if defined(Windows)&&defined(USE_WINFILE) 
return CloseHandle(han);
#else
return (LCLOSE(han)>=0);
#endif
}

OFFTYPE setfilepos(FILEHANDLE file,OFFTYPE offset) ;
#define INTSEARCH
class sectorbuf {
	private:
	char *buf;
	public:
inline	sectorbuf(int bs,size_t len) {
		buf=alignedalloc(bs,len);
		}
inline	~sectorbuf() {
		alignedfree(buf);
		}
inline	char *data(void) {return buf;}
};
#endif
