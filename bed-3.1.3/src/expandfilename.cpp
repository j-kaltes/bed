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
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#ifdef HAVE_DIRECT_H
#include <direct.h>
#else

#ifndef __MINGW32__
#include <pwd.h>
#endif
#endif
#include <limits.h>
#include "offtypes.h"
#include "system.h"
#ifdef _WIN32
#include "win.h"
#endif
#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif
#ifdef __CYGWIN__
extern char * expanddisk(const char * const name,char *const filename) ;
//#define CYGWIN_USES_WINCWD 1
#if defined(CYGWIN_USES_WINCWD)
char *expandfilename(char *filename,const char *name) {
if(name[0]=='\\') {
	return expanddisk(name,filename);
  }
else
	cygwin_conv_path (CCP_POSIX_TO_WIN_A,name ,filename, BEDSMAXPATH);
return filename;
}
#else
char *expandfilename(char *filename,const char *name) {
if(name[0]=='\\') {
		return expanddisk(name,filename);
		}
char tmp[MAX_PATH];
char *ptr;
#if 0
cygwin_conv_to_win32_path(name,tmp);
#else
 winpath(name ,tmp);
#endif
return (GetFullPathName(tmp,BEDSMAXPATH,filename,&ptr),filename);
}
#endif
#else
#ifdef _WIN32
char *expandfilename(char *filename,const char *name) 
{
	char *ptr;
	return (GetFullPathName(name,BEDSMAXPATH,filename,&ptr),filename);
}
#else
#ifndef HAS_GOODREALPATH
char *expandfilename(char *filename,const char *name) {
	switch(name[0]) {
		case '/': strcpyn(filename,name,BEDSMAXPATH);return filename;
		case '~': {
			struct passwd *pas;
			if(name[1]=='/'||!name[1]) {
				uid_t uid=getuid();
				if((pas =getpwuid(uid))) {
					strcpy(filename,pas->pw_dir);
					strcat(filename,name+1);
					break;
					}
				else {
					}
				}
				else {
					const char *sl=strchr(name,'/');
					if(sl) {
						strcpyn(filename,name+1,(sl-name-1));
						filename[(sl-name-1)]='\0';
						pas=getpwnam(filename);
						if(pas) {
							strcpy(filename,pas->pw_dir);
							strcat(filename,sl);
							break;
							}
						}
					else {
						if((pas=getpwnam(name+1))) {
							strcpy(filename,pas->pw_dir);
							break;
							}
						}	
				}
			};
		case '\\': if(name[0]=='\\'&&name[1]=='~')
				   name++;
		default: 
#ifdef _WIN32
{
	char *ptr;
	return (GetFullPathName(name,BEDSMAXPATH,filename,&ptr),filename);

}
}
#else
			if(getcwd(filename, BEDSMAXPATH)) {
				int len=strlen(filename);
				filename[len]='/';
				strcpy(filename+len+1,name);
				return filename;
				}
			else {
				strcpy(filename,name);
				return filename;
				}
		};
#endif
	return filename;
	}
#endif
#endif
#endif

#ifdef _WIN32
int samefilename(char *one,char *two) {
	return !strcasecmp(one,two);
	}
#else
int samefilename(char *one,char *two) {
	return !strcmp(one,two);
	}
#endif
#include <algorithm>
#include <string_view>
#include <string.h>

extern const char versionstr[];
extern const int versionstrlen;


const char *replaceversion(char *tmp,const char *infile) {
        constexpr const std::string_view versionname="BEDVERSION";
        const char *end=infile+strlen(infile);
        const char *iter=infile,*hit=nullptr;
        char *uititer=tmp;
        while((hit=std::search(iter,end,versionname.begin(),versionname.end()))!=end) {
                int len=hit-iter;
                memcpy(uititer,iter,len);
                uititer+=len;
                memcpy(uititer,versionstr,versionstrlen);
                uititer+=versionstrlen;
                iter=hit+versionname.length();
                }
        if(hit!=nullptr) {
                int toend=end-iter;
                memcpy(uititer,iter,toend);
                uititer[toend]='\0';
		return tmp;
                }
	return infile;
	}
char *versionabsolutefilename(char *uitfile,const char *infile) {
        char tmp[BEDSMAXPATH];
        return expandfilename(uitfile,replaceversion(tmp,infile));
        }




#ifdef TRY
int main(void) {
	char name[BEDSMAXPATH];
	char input[BEDSMAXPATH];
	while(1) {
		gets(input);
		 expandfilename(name,input);
		 puts(name);
		 }
	 }
#endif
