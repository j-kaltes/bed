/*
* Contains two procedures:
* 	cutselect: implements the usual cut editor function
*		insertpast: implements the usual editor past function 
*
* Both are very inefficient. The way bed represents changed filecontent should be
* changed to make this kind of operations consume less time and memory.
*
* */
#include "global.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#define minnum(x,y) (((x)<(y))?(x):(y))
BEDPLUGIN
class Editor {
#include "faked.h"

public:
	
int	cutselect(void) {
	OFFTYPE len;
	OFFTYPE first,end;
 if(!(len=getselregion(&first))) {
		message("No selection");return -1;
 		}
 	end=first+len;	
	if(copyselect()<0) {
			return -1;
				}
	OFFTYPE fsize=filesize(),newpos=first;
	OFFTYPE blocklen=fsize-end; 
	char *buf;
	if(!	(buf=(char *)malloc(blocklen))) {
			message("Can't allocate enough memory (" OFFPRINT ")",blocklen);
			return -1;
			}
	for(OFFTYPE old=end;old<fsize;old+=blocklen) {
			OFFTYPE takelen=minnum(blocklen,fsize-old);
			OFFTYPE ret=getmem(old,buf,takelen);
			if(ret!=takelen) {
				message("Error: getmem got only : " OFFPRINT " bytes asked" OFFPRINT "\n",ret,takelen);
				free(buf);
				return -1;
				}
			putmem(newpos,buf,ret);
			newpos+=blocklen;
			}
		free(buf);
	setselect();
	OFFTYPE newend=fsize-len-1;
	filetruncate(newend);
	topos(first);
	return 0;
	}

int insertpast(void) {
	char *selbuf;
	int len;
 if((len=getselbuf(&selbuf))<=0) {
	 	message("No copy buffer");return -1;
	 	}
	OFFTYPE curpos=getfilepos()+geteditpos();
	OFFTYPE fsize=filesize();
	OFFTYPE endpart=fsize-curpos;
	if(extendfile(len)<0 ) {
			return -1;
			}
	char *buf;
	if(!	(buf=(char *)malloc(endpart))) {
			message("Can't allocate enough memory (" OFFPRINT ")",endpart);
			return -1;
			}
	OFFTYPE ret;
	if((ret=getmem(curpos,buf,endpart))!=endpart) {
			message("getmem returned " OFFPRINT " instead of " OFFPRINT,ret,endpart);
			free(buf);return -1;
			}
	OFFTYPE newpos=curpos+len;
	if((ret=putmem(newpos,buf,endpart))!=endpart) {
			message("putmem returned " OFFPRINT " instead of " OFFPRINT,ret,endpart);
			free(buf);return -1;
			}
	if((ret=putmem(curpos,selbuf,len))!=len) {
			message("putmem returned " OFFPRINT " instead of " OFFPRINT,ret,len);
			free(buf);return -1;
			}
	return 0;
 	}
};



sethelp(cutselect,"Standard cut: selection is put in copy buffer, file after selection\nis moved to begin selection and rest file is truncated")
sethelp(insertpast,"Standard past: file is extended and content after current file\nposition moved so that copy buffer content can be inserted")

call(cutselect)
call(insertpast)

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
/*                                                                           */
/* Thu May 30 09:32:55 2002                                                  */
