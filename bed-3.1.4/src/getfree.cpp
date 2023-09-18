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
#include <string.h>
#include "debug.h"

#ifdef HAS_MEMINFO
long long getfree(void) {
	char gegs[80];
	FILE *fp;
	int memfree=-1,buffers=0,cached=0;
	long long freem;
	if(!(fp=fopen("/proc/meminfo","r"))) {
		fprintf(stderr,"Can't open /proc/meminfo");
		return -1;
		}
	while(fgets(gegs,80,fp))  {
		if(!strncmp("MemFree:",gegs,8)) {
			sscanf(gegs+9,"%d",&memfree);
			}
		if(!strncmp("Buffers:",gegs,8)) {
			sscanf(gegs+9,"%d",&buffers);
			}
		if(!strncmp("Cached:",gegs,7)) {
			sscanf(gegs+8,"%d",&cached);
			if(memfree>0&&buffers)
				break;
			}
		}
	fclose(fp);
	if(memfree==-1) {
		fprintf(stderr,"Error reading /proc/meminfo\n");
		return 20*1024*1024;
		}
	freem=(memfree+cached+buffers);
	return freem*1024;
	}
#else
#define minnum(x,y) ((x)<(y)?(x):(y))
inline long long canusemem(long long ramav,long long totram,long long avvis) {
	long take=(totram-ramav);
	take=minnum(take,avvis/2);
	return (long long )ramav+take;
	}
#ifdef _WIN32
#include <windows.h>
#if (_WIN32_WINNT >= 0x0500)
long long getfree(void) {

MEMORYSTATUSEX memex;
memex.dwLength=sizeof(memex);

GlobalMemoryStatusEx(&memex );

//	long long ret= canusemem( memex.ullAvailPhys,memex.ullTotalPhys,memex.ullAvailPageFile);
//	return ret;
return memex.ullAvailPhys;
	}
#else
long long getfree(void) {
	MEMORYSTATUS memstat;
	memstat.dwLength=sizeof(memstat);
	GlobalMemoryStatus(&memstat);
//	long long  ret= canusemem( memstat.dwAvailPhys,memstat.dwTotalPhys,memstat.dwAvailPageFile);
	return memstat.dwAvailPhys;
	}
#endif
#else
#ifdef SYSCONF_MEM
#include <unistd.h>
long long getfree(void) {
       long psize,av,max;
       psize=sysconf(_SC_PAGESIZE);
	max=sysconf(_SC_PHYS_PAGES);
	av=sysconf(_SC_AVPHYS_PAGES);
	long long ret;
//	ret= ((long long)psize*av)+(((long long)max*psize)/7);
	ret= canusemem(av*psize,max*psize,psize*max*2);
	DEBUGGING("psize=%ld,max=%ld,av=%ld,ret=%ld\n",psize,max,av,ret);
	return ret;
}
#else
long long getfree(void) {
	return 1024*1024*500;
	}
#endif
#endif
#endif
#ifdef TEST
int main(void) {
	long long		ret=getfree();
	printf("Free: %lld\n",ret);
	return 0;
	}
#endif
