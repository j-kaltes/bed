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
/***
 *  v2  2000.08.23  Jim Blackson	added definiton of TRUNCATE from memory.cpp.
 */
#define NOSTAT
#ifndef OFFTYPES_H
#define OFFTYPES_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <features.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifndef HAVE_TRUNCATE64
	#define truncate64 mytruncate64
#endif
#ifdef HAS_LSEEK64
	#ifdef USE_WINFILE
		#include <windows.h>
		//#define HAS_PREAD 1
		#define LPREAD mypread64
		#define LPWRITE mypwrite64
//		#define USE_PREAD 1
		#define FILEHANDLE HANDLE 
		#define MY_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
		#undef OFFTYPE
		#define OFFTYPE LONGLONG
		#define LONGLONGOFFSET 8
	 #ifndef LARGEFILES
		#define LARGEFILES 8
	 #endif
		extern int myftruncate(FILEHANDLE output,OFFTYPE size) ;
		#define FTRUNCATEHANDLE myftruncate


	#define MMAP mmap64
	#define LSEEK lseek64
	#define OPEN open64
	#define STATTYPE struct stat64
	#define FSTAT fstat64
	#define STAT stat64

	#define FTRUNCATE ftruncate64
	#define TRUNCATE truncate64

#else // USE_WINFILE
	#define MMAP mmap64
	#define LSEEK lseek64
	#define LPREAD pread64
	#define LPWRITE pwrite64
#ifdef HAVE_OPEN64 
	#define OPEN open64
#else
	#define OPEN open
#endif
	#ifdef HAVE_OFF64_T 
		#define OFFTYPE off64_t
	#else
	#ifdef HAVE____OFF64_T 
		#define OFFTYPE __off64_t
	#endif
	#endif
#ifdef HAVE_STAT64 
	#define STATTYPE struct stat64
	#define FSTAT fstat64
	#define STAT stat64
#else
	#define STATTYPE struct stat
	#define FSTAT fstat
	#define STAT stat
#endif
	#define FTRUNCATE ftruncate64
	#define FTRUNCATEHANDLE FTRUNCATE 
	#define TRUNCATE truncate64
	#define LONGLONGOFFSET 8
		#define FILEHANDLE int
		#define MY_INVALID_HANDLE_VALUE -1
#endif
#else // HAS_LSEEK64
	//#define MY_O_LARGEFILE 0
	#define MMAP mmap
	#define LSEEK lseek
	//#define OFFTYPE   __off_t
	#define OFFTYPE   off_t
	#define STATTYPE struct stat
	#define FSTAT fstat
	#define STAT stat
	#define OPEN open
	#define FTRUNCATE ftruncate
	#define TRUNCATE truncate
	#if SIZEOF_OFF_T>=8
	#define LONGLONGOFFSET 8

	#endif
	#ifdef USE_WINFILE
		#include <windows.h>
		//#define HAS_PREAD 1
		#define LPREAD mypread64
		#define LPWRITE mypwrite64
//		#define USE_PREAD 1
		#define FILEHANDLE HANDLE 
		#define MY_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
		#undef OFFTYPE
		#define OFFTYPE LONGLONG
		#define LONGLONGOFFSET 8
	 #ifndef LARGEFILES
		#define LARGEFILES 8
	 #endif
		extern int myftruncate(FILEHANDLE output,OFFTYPE size) ;
		#define FTRUNCATEHANDLE myftruncate
	#else /*USE_WINFILE*/
		#define FTRUNCATEHANDLE FTRUNCATE
		#define FILEHANDLE int
		#define MY_INVALID_HANDLE_VALUE -1
   #ifdef HAS_PREAD
	#define LPREAD pread
	#define LPWRITE pwrite
   #endif
	#endif /*USE_WINFILE*/

#endif //HAS_LSEEK64


#if defined(LARGEFILES) && ((defined(_WIN32)&&OFFTYPE==LONGLONG) ||(SIZEOF_LONG<8))
//#if defined(LARGEFILES)

#define OFFPRINT "%lld"
#define OFFPRINTONLY "ll"
#define BLOCKPRINT "%lld"

#ifdef LLONG_MAX 
#define OFFTYPEMAX LLONG_MAX
#else
#define OFFTYPEMAX LONG_LONG_MAX
#endif

#ifdef ATOIBUG
#define STRTOO strtoll
#else
#define STRTOO strtoll
#endif
#else
#define OFFPRINT "%ld"
#define OFFPRINTONLY "l"
#define BLOCKPRINT "%ld"
#define OFFTYPEMAX LONG_MAX
#ifdef ATOIBUG
#define STRTOO strtol
#else
#define STRTOO strtol
#endif
#endif

#define NUMDIGETS_USED 23 // maximal using oct
#define LARGERTYPE  long long
/*
#ifdef LARGEFILES
#define MAXDISCSIZE (OFFTYPE) 1023*1024*1024*1024LL
#else
#define MAXDISCSIZE OFFTYPEMAX 
#endif
*/
#define MAXDISCSIZE OFFTYPEMAX 



#define	MUNMAP 	munmap
#define BLOCKTYPE OFFTYPE

#define OFFTYPEINVALID OFFTYPEMAX
#define SEARCHFAILURE OFFTYPEINVALID
#define LREAD read
#define LWRITE write
#define LOPEN OPEN
#define	LCLOSE close
#define BEDS_SIZE_T ssize_t
#ifdef  PRI_SIZE_T_MODIFIER
#define SIZE_TPRINT "%" PRI_SIZE_T_MODIFIER "d"
#else
//#if SSIZE_T_SIZE>=8
#if SIZEOF_SSIZE_T>=8
#define SIZE_TPRINT "%lld"
#else
#define SIZE_TPRINT "%d"
#endif
#endif

#ifdef  O_LARGEFILE
#define LARGEFILE_FLAG |O_LARGEFILE
#else
#define LARGEFILE_FLAG 
#endif

#if defined(USE_WINFILE) ||defined(HAS_PREAD)
#if !defined(RUNONWIN95) ||defined(USETHREADS)
#define USE_PREAD 1
#endif
#endif
#if defined(USETHREADS) && defined(RUNONWIN95)
#warning inconstent options  USETHREADS and  RUNONWIN95 inconsistent, undef RUNONWIN95
#undef RUNONWIN95
#endif
#if !defined(RUNONWIN95) && defined(BIOSDISK)
#warning inconstent options  BIOSDISK makes no sense without RUNONWIN95, undef BIOSDISK
#undef BIOSDISK
#endif
#if SIZEOF_OFF_T<8
typedef	 long long Offlarge;
#else
typedef	OFFTYPE Offlarge;
#endif
#endif
