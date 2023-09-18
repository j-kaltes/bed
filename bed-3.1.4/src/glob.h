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
#ifndef MYGLOB_H
#define MYGLOB_H
#ifdef  __cplusplus
extern "C" {
#endif
#ifndef SPLITGETFILES
#if defined(_WIN32) || defined(_WIN64) || !defined(HAVE_GLOB)
#define SPLITGETFILES
#endif
#endif
#ifdef SPLITGETFILES
extern int getfilessplit(const char *pattern,char ***files,char ***dirs,int *dirend) ;
#else
extern int getfiles(const char *pattern,char ***files,int *back)		;
#endif
extern int isdir(const char *path) ;
#ifdef  __cplusplus
}
#endif
#endif
