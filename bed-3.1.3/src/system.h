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
#ifndef SYSTEM_H
#define SYSTEM_H

#include <features.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include "defines.h"
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#include "debug.h"
#ifdef _WIN32
#include "win.h"
#if defined(_MINGW32__) || defined(__MINGW64__)
#include <minwindef.h>
#endif
#define BEDSMAXPATH MAX_PATH
#else
#define BEDSMAXPATH MAXPATHLEN
#define CONFDIRWIN CONFDIR
#endif
#if defined(HAS_REALPATH) && !defined(USE_WINFILE)
#define arealpath(in,uit) realpath(in,uit)
#else
#define arealpath(in,uit) 	GetFullPathName(in,BEDSMAXPATH,uit,NULL)
#endif
#if  defined(HAS_GOODREALPATH)&&!defined(__CYGWIN__)
#define expandfilename(outname,inname) realpath(inname,outname)
#else
#if 0 //_WIN32
#define expandfilename(outname,inname) _fullpath( outname, inname, MAX_PATH ) 
#else
extern char *expandfilename(char *filename,const char *name) ;
#endif
#endif

extern int samefilename(char *one,char *two);
#ifdef __cplusplus
#define editcast (int (Editor::*)(void))
#define funcptr(Type,func) &Type::func
class Editor;
extern int (Editor::*helpproc)(void);
#define clcallproc(cl,pr) (helpproc=pr,(cl->*(pr))())
#define callproc(pr) clcallproc(this,pr)
#endif
#include <string.h>
inline char *strcpyn(char *naar,const char *van,size_t n) {
	return (char *)memccpy(naar,van,'\0',n);
	}
#ifdef __CYGWIN__
#define sys_errlist _sys_errlist
#define sys_nerr _sys_nerr
#endif
#ifndef HAS_MAP_FAILED
#define MAP_FAILED (void *)(-1)
#endif
#ifdef _WIN32
#define FILENAMESEP "\\"
#else
#define FILENAMESEP "/"
#endif
#ifdef O_BINARY
#define BINARY_FLAG |O_BINARY 
#else
#define BINARY_FLAG
#endif
extern int newlinelen;
extern char newline[];
#define addnltoar(str) (memcpy(str,newline,newlinelen),newlinelen)
#endif
