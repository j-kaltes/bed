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
#include <features.h>
#include <string.h>
#include <stdio.h>

 
int getlabel(const char **labels,  int nr, const char *str) {
	int beg=0, end=nr,half,ver;

	while(beg<end) {
		half=(beg+end)/2;
		ver=strcmp(str,labels[half]);
		if(ver<0)
			end=half;
		else {
			if(ver>0)
				beg=half+1;
			else
				return half;
			}
		}

	return -1;
	}
int getlabelcase(const char **labels,  int nr, const char *str) {
	int beg=0, end=nr,half,ver;

	while(beg<end) {
		half=(beg+end)/2;
		ver=strcasecmp(str,labels[half]);
		if(ver<0)
			end=half;
		else {
			if(ver>0)
				beg=half+1;
			else
				return half;
			}
		}

	return -1;
	}

int getunderlabeli(char **labels,  int nr, const char *str,int icase) {
	int beg=0, end=nr,half,ver;

	while(beg<end) {
		half=(beg+end)/2;
		if(icase)
			ver=strcmp(str,labels[half]);
		else
			ver=strcasecmp(str,labels[half]);
		if(ver<0)
			end=half;
		else {
			if(ver>0)
				beg=half+1;
			else
				return half;
			}
		}
	return beg;
	}
int getunderlabel(char **labels,  int nr, const char *str) {
	return getunderlabeli(labels,nr,str,1);
	}
#ifdef TEST
#define prodef(name) #name,
char *proc[]= {
#include "procedures.h"
};
int nr=sizeof(proc)/sizeof(char*);
void main(int argc,char **argv) {
	char buf[400];
	int i;
	gets(buf);
	i=getlabel(proc,nr,buf);
	if(i>=0)
		printf("Found %s at %d\n",proc[i],i);
	}
#endif
