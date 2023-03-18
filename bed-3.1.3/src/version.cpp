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
#include <stdlib.h>
#include <sys/param.h>
#ifdef CYGWIN
#include <cygwin/version.h>
#include <sys/cygwin.h>
#endif
#ifndef HAVE_SETENV
#include "setenv.h"
#endif
#include "system.h"
#include "offtypes.h"


int sourcenumber=
#include "sourcenumber"
;

extern const char versionstr[];
extern const int versionstrlen;
constexpr const char versionstr[]=VERSION;
constexpr const int versionstrlen=sizeof(versionstr)-1;

#include "md5sums.cpp"
#include "copy.h"
#include "help.h"
char description[]="Binary editor";

#include "date.c"
extern int margc,miter;
extern char **margv;




void showversion(void) {
	printf("%s - %s\n\n",PROGRAM,description);
	printf("Version %s, Source number %d %s\n",versionstr,sourcenumber,srcdate);
	fwrite(copy,sizeof(copy)-1,1,stdout);


	}
void details(void) {
	fwrite(md5sums,sizeof(md5sums)-1,1,stdout);
	}
extern char *getconfig(char *configname) ;
#ifdef __CYGWIN__
#include "win.h"
#endif
inline void showhelp(void) {
	fwrite(help,sizeof(help)-1,1,stdout);

	char configname[BEDSMAXPATH];
	if(getconfig(configname)) {
#ifdef __CYGWIN__
		char winname[BEDSMAXPATH];
		abswinpath(configname ,winname);
		printf("Current configuration file: %s",winname);
#else
		printf("Current configuration file: %s",configname);
#endif
		}
	else {
		const char conf[]="No configuration file found.";
		fwrite(conf,sizeof(conf)-1,1,stdout);
		}
#ifdef __CYGWIN__
	HMODULE cygwindll=GetModuleHandle(CYGWIN_VERSION_DLL_IDENTIFIER ".dll");
	if(cygwindll) {
		char cygname[MAX_PATH];
		int len;
		if((len=GetModuleFileNameA(cygwindll, cygname,MAX_PATH ))) {
			putchar('\n');
			fwrite(cygname,len,1,stdout);
			}
		}
#endif
	putchar('\n');
	exit(0);
	}

extern OFFTYPE MAXCDSIZE;
extern OFFTYPE sizespecified;
extern int userblocksize;
extern int STBLOCKSIZE;

extern char realsize;
#ifndef NEWTERM
extern char staticscreen;
extern char *linemax,*colmax;
char *linemax=LINEMAX,*colmax=COLMAX;
char staticscreen=0;
#endif
#include "config.h"
#ifndef PD24
int	dokeyconfig=0;
#endif
#include <string.h>
extern int blackwhite;
extern char *macrofile;
char *macrofile=NULL;
char **toexecute=NULL;
int executemax=0,executeiter=0;
char **toplugin=NULL;
int pluginmax=0,pluginiter=0;
inline OFFTYPE getget(int margc,char **margv,int &i ) {
	char *getstr;
	if(margv[i][2])
		getstr=margv[i]+2;
	else {
		if((++i<margc)&&(margv[i][0]>='0')&&margv[i][0]<='9')
			getstr=margv[i];
		else {
			return -1LL;
		  };
	};	
	
	return STRTOO(getstr, NULL, 0);
	}
extern int doargs(int beg,int margc,char **margv) ;
int doargs(int beg,int margc,char **margv) {
	int i;

	for(i=beg;i<margc&&margv[i][0]=='-';i++) {
			switch(margv[i][1]) {
#ifdef COMPILECOLORS
				case 'B': {
					blackwhite=1;
					break;
					};
				case 'C': {
					blackwhite=0;
					break;
					};
#endif

				case 's': {
					char *value;
					if(margv[i][2])
						value=margv[i]+2;
					else {
						if(++i<margc)
							value=margv[i];
						else {
							showhelp();return 0;
						  };
						 };
				
					macrofile=value;
					break;
					};break;

				case 'e': {
					char *value;
					if(margv[i][2])
						value=margv[i]+2;
					else {
						if(++i<margc)
							value=margv[i];
						else {
							showhelp();return 0;
						  };
						 };

					if(executemax<=executeiter) {
						executemax=(executemax+1)*2;
						toexecute=(char **)realloc(toexecute,executemax*sizeof(*toexecute));
						}
					toexecute[executeiter++]=value;
					break;
					};break;
				case 'p': 
					{
						char *value;
						if(margv[i][2])
							value=margv[i]+2;
						else 
						{
							if(++i<margc)
								value=margv[i];
							else {
								showhelp();return 0;
								};
						};
						if(pluginmax<=pluginiter) 
						{
							pluginmax=(pluginmax+1)*2;
							toplugin=(char **)realloc(toplugin,pluginmax*sizeof(*toplugin));
						}
						toplugin[pluginiter++]=value;
						break;
					};
					break;
				case 'f': {
					char *value;
					if(margv[i][2])
						value=margv[i]+2;
					else {
						if(++i<margc)
							value=margv[i];
						else {
							showhelp();return 0;
						  };
						 };
					setenv(CONFVAR, value, 1);
					break;
					};break;

#ifndef PD24
				case 'k': dokeyconfig=i+1;
			fprintf(stderr,"keyconfig");

return 0;
#endif
#ifndef NEWTERM
				case 'y': {
					if(margv[i][2])
						linemax=margv[i]+2;
					else {
						if((++i<margc)&&(margv[i][0]>='0')&&margv[i][0]<='9')
							linemax=margv[i];
						else {
						showhelp();
						  };
					};	}; break;
				case 'x': 
					if(margv[i][2])
						colmax=margv[i]+2;
					else {
						if((++i<margc)&&(margv[i][0]>='0')&&margv[i][0]<='9')
							colmax=margv[i];
						else {
							showhelp();
						  }
					};break;
				case 't': staticscreen=1;break;
#endif
				case 'd': {
					char *disksizestr;

					if(margv[i][2])
						disksizestr=margv[i]+2;
					else {
						if((++i<margc)&&(margv[i][0]>='0')&&margv[i][0]<='9')
							disksizestr=margv[i];
						else {
						showhelp();
						break;
						  };
					};	
					sizespecified=STRTOO(disksizestr, NULL, 0);

					}; break;
				case 'b': {
					char *blocksizestr;

					if(margv[i][2])
						blocksizestr=margv[i]+2;
					else {
						if((++i<margc)&&(margv[i][0]>='0')&&margv[i][0]<='9')
							blocksizestr=margv[i];
						else {
						showhelp();
						break;
						  };
					};	
					userblocksize=STRTOO(blocksizestr, NULL, 0);
					if(userblocksize<=0) {
						fprintf(stderr,"Blocksize should be greater then 0\n");exit(1);};

					STBLOCKSIZE=userblocksize;
					}; break;

				case 'V': 
					OFFTYPE MAXCDSIZETMP;
					if((MAXCDSIZETMP=getget(margc,margv,i )) >=0) 
						MAXCDSIZE= MAXCDSIZETMP;
					else
						{
						printf("No valid MAXCDSIZE after -V. Default value: " OFFPRINT "\n",MAXCDSIZE);
						exit(1);
						};	
					break;

				case 'w': realsize=1;break;

				case 'v': showversion();exit(0);
				case 'c': details();exit(0);
				case 'h': ;
				default: showhelp();
				};
		}
	return i;
	}

void doargs(void) {
#ifndef PD24
	char *ptr;
	if((ptr=strstr(margv[0],"keyconfig"))&&ptr[9]=='\0') {
		dokeyconfig=1;
		return;
		}
#endif
miter=doargs(1, margc,margv) ;
return;
}
