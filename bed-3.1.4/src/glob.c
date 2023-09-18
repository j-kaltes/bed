#include "defines.h"
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


#include <features.h>
#include <stdio.h>
#include <unistd.h>
#if defined(HAVE_GLOB) && !defined( _WIN32)
#include <glob.h>
#endif
#include <sys/stat.h>
#include "glob.h"

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32

#include <windows.h>
#endif

extern int isdir(const char *path) ;
int isdir(const char *path) {
#ifdef _WIN32
 DWORD dwAttrs;
 dwAttrs = GetFileAttributes(path); 
return((dwAttrs!=INVALID_FILE_ATTRIBUTES)&&(FILE_ATTRIBUTE_DIRECTORY&dwAttrs));
#else
struct stat stbuf;
return (stat(path,&stbuf)==0 &&S_ISDIR(stbuf.st_mode)) ;
#endif
}

#if defined(HAVE_GLOB) && !defined(_WIN32) && !defined(_WIN64) 
int getfiles(const char *pattern,char ***files,int *back) {

	int end,beg;
static	  char **was=NULL;
	  char *tmp;
#ifdef __CYGWIN__
#define STARTFILES 100

	static char **allfiles=NULL;
	static int allfileslen=0,filenr=0;
 WIN32_FIND_DATA  fdata;

 HANDLE han;
if(allfileslen) {
	int i;
	for(i=0;i<filenr;i++)
		free(allfiles[i]);
	filenr=0;
	}
//PVOID OldValue = NULL;

//     Wow64DisableWow64FsRedirection(&OldValue);

	 if((han=FindFirstFile(pattern,&fdata))==INVALID_HANDLE_VALUE) {
 //       	Wow64RevertWow64FsRedirection(OldValue) ;
		*back=0;
		return 0;
		}
do {
	int len=strlen(fdata.cFileName)+1;
	if(filenr>=allfileslen) {
		allfileslen=allfileslen*2+50;
		allfiles=(char **)realloc(allfiles,sizeof(char*)*allfileslen);
		}
	allfiles[filenr]=(char *)malloc(len);
	strcpy(allfiles[filenr],fdata.cFileName);
	filenr++;
	 } while(FindNextFile(han,&fdata));
//Wow64RevertWow64FsRedirection(OldValue) ;
was=allfiles;
#else
	int filenr;
          static glob_t globbuf;
	static int globed=-1;
	
if(globed>=0) {
	globfree(&globbuf);
	}
else
	globed=1;
#ifdef GLOB_PERIOD
// GLOB_TILDE

          glob(pattern,GLOB_PERIOD|GLOB_NOSORT, NULL, &globbuf);
	filenr=globbuf.gl_pathc;
#else

        glob(pattern,GLOB_NOSORT, NULL, &globbuf);
	switch(pattern[0]) {
		case '?': {
			int len=strlen(pattern);		
			char newp[len+1];
			memcpy(newp+1,pattern+1,len);
			newp[0]='.';
			glob(newp,GLOB_APPEND|GLOB_NOSORT, NULL, &globbuf);break;
			break;
			}
		case '*': {
			int len=strlen(pattern);		
			char newp[len+2];
			memcpy(newp+1,pattern,len+1);
			newp[0]='.';
			glob(newp,GLOB_APPEND|GLOB_NOSORT, NULL, &globbuf);break;
			break;
			};
		default:;	
		}	;
	filenr=globbuf.gl_pathc;
#endif

	/*
	size_t memlen= globbuf.gl_pathc*sizeof(globbuf.gl_pathv[1]);
	was=(char **)realloc(was,memlen);
	memcpy(was, globbuf.gl_pathv,memlen);
	*/
	was= globbuf.gl_pathv;
#endif
	beg=-1;
	end=filenr;

	  while(1) {
	  	do {
			beg++;
nodiragain:
	  		if(beg>=end) {
	  			beg--;
	  			goto uit;
	  			}
		} while(!isdir(was[beg]));
	if(was[beg][0]=='.'&&!was[beg][1]) {
		filenr--;
		if(filenr<end)
			end=filenr;
		if(filenr>beg) { 
			char *tmp= was[beg];
			was[beg]=was[filenr];
			was[filenr]=tmp;
			}
		goto nodiragain;
			
		}
		do {
			end--;
	  		if(beg>=end) {
	  			beg--;
	  			goto uit;
	  			}
	if(was[end][0]=='.'&&!was[end][1]) {
		filenr--;
		if(filenr>end) {
			char *tmp= was[end];
			was[end]=was[filenr];
			was[filenr]=tmp;
			}
		else
			end=filenr;
		continue;
		}
		} while(isdir(was[end]));
		tmp=was[end];
		was[end]=was[beg];
		was[beg]=tmp;
  		}

  	uit:
  	*files=was;
  	*back=end;
	return filenr;
	}
#endif
#ifdef TEST	
void main(int argc, char** argv) {
	char **files;
	int len,end,i;
	len=getfiles(argv[1],&files,&end);
	puts("Files: ");
	for(i=0;i<end;i++) 
		puts(files[i]);
	puts("Directories: ");
	for(i=end;i<len;i++) 
		puts(files[i]);
	len=getfiles(argv[1],&files,&end);
	puts("Files: ");
	for(i=0;i<end;i++) 
		puts(files[i]);
	puts("Directories: ");
	for(i=end;i<len;i++) 
		puts(files[i]);


	}
#endif
#if defined(_WIN32) || defined(_WIN64)
int getfilessplit(const char *pattern,char ***files,char ***dirs,int *dirend) {

#define STARTFILES 150
#define STARTDIRS 50
	static char **allfiles=NULL;
	static int allfileslen=0,filenr=0;

	static char **alldirs=NULL;
	static int alldirslen=0,dirnr=0;
 WIN32_FIND_DATA  fdata;

 HANDLE han;
if(allfileslen) {
	int i;
	for(i=0;i<filenr;i++)
		free(allfiles[i]);
	filenr=0;
	}
if(alldirslen) {
	int i;
	for(i=0;i<dirnr;i++)
		free(alldirs[i]);
	dirnr=0;
	}
//PVOID OldValue = NULL;


	 if((han=FindFirstFile(pattern,&fdata))==INVALID_HANDLE_VALUE) {
		*dirend=0;
		return 0;
		}
do {
	if(fdata.cFileName[1]||fdata.cFileName[0]!='.') {
		int len=strlen(fdata.cFileName)+1;
		if(fdata.dwFileAttributes&  FILE_ATTRIBUTE_DIRECTORY) {
			if(dirnr>=alldirslen) {
				alldirslen=alldirslen*2+STARTDIRS;
				alldirs=(char **)realloc(alldirs,sizeof(char*)*alldirslen);
				}
			alldirs[dirnr]=(char *)malloc(len);
			strcpy(alldirs[dirnr],fdata.cFileName);
			dirnr++;
			}
		else {
			if(filenr>=allfileslen) {
				allfileslen=allfileslen*2+STARTFILES;
				allfiles=(char **)realloc(allfiles,sizeof(char*)*allfileslen);
				}
			allfiles[filenr]=(char *)malloc(len);
			strcpy(allfiles[filenr],fdata.cFileName);
			filenr++;
			}
		}
	 } while(FindNextFile(han,&fdata));
//Wow64RevertWow64FsRedirection(OldValue) ;
  	*files=allfiles;
	*dirs=alldirs;
	*dirend=dirnr;
	return filenr;
	}
#else
#ifdef  SPLITGETFILES
#include <fnmatch.h>
#include <dirent.h>

int getfilessplit(const char *pattern,char ***files,char ***dirs,int *dirend) {

#define STARTFILES 150
#define STARTDIRS 50
	static char **allfiles=NULL;
	static int allfileslen=0,filenr=0;

	static char **alldirs=NULL;
	static int alldirslen=0,dirnr=0;

DIR *dir;

struct dirent *ent;
if(allfileslen) {
	int i;
	for(i=0;i<filenr;i++)
		free(allfiles[i]);
	filenr=0;
	}
if(alldirslen) {
	int i;
	for(i=0;i<dirnr;i++)
		free(alldirs[i]);
	dirnr=0;
	}

	if(!(dir=opendir("."))) {
		*dirend=0;
		return 0;
		}
	while(( ent=readdir(dir))) {
		const char *name= ent->d_name;
		if(name[1]||name[0]!='.') {
//			if(!fnmatch(pattern, name, FNM_PERIOD|FNM_NOESCAPE) ) {
			if(!fnmatch(pattern, name, FNM_NOESCAPE) ) {
				int len= strlen(name)+1;
				switch(ent->d_type) {
				case	DT_LNK: 
						if( !isdir(name) ) 
							goto NODIR;

				case DT_DIR: {
					if(dirnr>=alldirslen) {
						alldirslen=alldirslen*2+STARTDIRS;
						alldirs=(char **)realloc(alldirs,sizeof(char*)*alldirslen);
						}
					alldirs[dirnr]=(char *)malloc(len);
					strcpy(alldirs[dirnr],name);
					dirnr++;
					};break;
				default:  {
NODIR:
					if(filenr>=allfileslen) {
						allfileslen=allfileslen*2+STARTFILES;
						allfiles=(char **)realloc(allfiles,sizeof(char*)*allfileslen);
						}
					allfiles[filenr]=(char *)malloc(len);
					strcpy(allfiles[filenr],name);
					filenr++;
					}
				}
			}
		}
	 } ;
	closedir(dir);
  	*files=allfiles;
	*dirs=alldirs;
	*dirend=dirnr;
	return filenr;
	}
#endif
#endif
