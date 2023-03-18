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
/***
 *  v2  2000.09.20  Jim Blackson	rename mem.truncate to mem.truncatefile.
 */

#include "desturbe.h"
#include "editor.h"
#include "editlimits.cpp"
extern long long getfree(void) ;
#include "main.h"
#include <stdlib.h>
#include <string.h>
int Editor::extendfile(OFFTYPE len) {
	if(searchdontedit())
		return -1;
/*	if((len>MAXEXTEND)&&(useext(len)>getfree())) {
		message("Too big. Extend multiple times with save between");
		return -1;
		}
		*/
	char buf[mem.BLOCKSIZE];
	INCDEST();
	memorize();
	long begin=(mem.filesize%mem.BLOCKSIZE);
	OFFTYPE oldsize=mem.filesize;
	extendtoundo(oldsize);
	BLOCKTYPE bl=(oldsize/mem.BLOCKSIZE);
	mem.getblock(bl,buf);
	memset(buf+begin,'\0',mem.BLOCKSIZE-begin);	
	mem.filesize+=len;
	mem.putblock(bl++,buf);
/*	BLOCKTYPE blend=mem.filesize/mem.BLOCKSIZE+1;
	memset(buf,'\0',mem.BLOCKSIZE);
	for(;bl<blend;bl++)
		mem.putblock(bl,buf);
		*/
	modified++;
	resetnumbase() ;
	if(!shouldreinit)
//		dofromfile=1;
		fromfile();
	message("%s extended with " OFFPRINT " bytes",mem.filename,len);
	updated=0;
	DECDEST();
	return 0;
	}

/*
int Editor::extend(void) {
	char ant[MAXANT];
	getinput("How many bytes? ",ant);
	return extendfile(atoi(ant));
	}
*/
#include "screen.h"
#include "dialog.h"
int Editor::extend(void) {
	if(searchdontedit())
		return -1;
	int wlin=10,wcol=20+NUMDIGETS_USED ,res=3;
	static int fromend=0;
	OFFTYPE ext=0;
	char extention[80],*endptr;
	again:
	while(1) {
		if(fromend) {
			sprintf(extention,OFFPRINT,ext);
			}
		else
			sprintf(extention,OFFPRINT,ext+filesize());
res--;
begindialog(wlin,wcol,8);
      		optselect(fromend,fromend, 
			opt("~File",5,16,0),
			opt("~Extention",6,16,1));
		linernr("~Size: ",extention,2,3, NUMDIGETS_USED+1 );
			oke(5,3);
			act("~Cancel", 7,3);
		  enddialog(res,res);
		if(!res||res==5) { 
			editup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			res=3;
			menuresized();
			wrefresh(editscreen);
			goto again;
			}
		ext=STRTOO(extention,&endptr,10);
		if(endptr==extention) {
			beep();
			message("No digit");
			wrefresh(editscreen);
			goto again;
			}
		ext-=(fromend)?0:filesize();
		if(res<3) {
			fromend=res-1;
			goto again;
			}
		if(ext<0) {
			beep();
			OFFTYPE news=filesize()+ext;
			if(news>1) {
				editup();
				switch(getkeyinput( OFFPRINT  " is smaller then filesize (" OFFPRINT "). Do you want to truncate file? (y/N): ",news,filesize())) {
					case 'y':
					case 'Y': {
						int ret;
						topos(news-1);toscreen();
						if((ret=truncate())>=0)
							return ret;
						};break;
					default:break;
					};
				fileinfo();
				}
			else
				message("Filesize " OFFPRINT "?",news);
			wrefresh(editscreen);
			goto again;
			};
/*
		if(ext>INT_MAX) {
			message("Extend in multiple part with save between");
			wrefresh(editscreen);
			goto again;
			} */
				
		break;
		}
	int ret=0;
	if((ret= extendfile(ext))<0) {
		wrefresh(editscreen);
		goto again;
		}
	
	editup(); 
	return ret;
	}
int Editor::truncate(void) {
	OFFTYPE vanaf=editpos+filepos+1;
	OFFTYPE eraf=mem.filesize-vanaf;
	int tomuch=((eraf>MAXTOUNDO)&&(usetrunc(eraf)>getfree()));
	switch(getkeyinput("Delete everything after cursor? (y/N/c)%s",((tomuch)?" ***no undo***":""))) {
		case 'y':
		case 'Y': return forcetruncate();break;
		default: message("Cancel"); beep();return -1;
		}
	return -1;
	}

#include "screenpart.h"
inline int Editor::forcetruncate(void) {
	int bytes=parts[mode]->getbytes();
	OFFTYPE vanaf= filepos+(1+editpos/bytes)*bytes;
//editpos+filepos+1;
	return filetruncate(vanaf);
	}
int Editor::filetruncate(OFFTYPE vanaf) {
	OFFTYPE eraf=mem.filesize-vanaf;
	int tomuch=((eraf>MAXTOUNDO)&&(eraf>getfree()||usetrunc(eraf)>getfree()));
	INCDEST();
	memorize();
	if(!tomuch)
		strtoundo(vanaf,eraf) ;
	mem.truncatefile(vanaf);				/* v2 */
	modified++;
	resetnumbase(); 
	if(!shouldreinit) {
	//	dofromfile=1;
		fromfile();
		updated=0;
		}
	DECDEST();
	return 0;
	}

/*
#include "type.h"
#include "screenpart.h"

int Editor::putreturn(void) {
	if(!commandmode&&isaascii(parts[mode])) {
		changed=1;
		changechar(editpos,0x0A);
		nextfast();
		wrefresh(editscreen);
		}
	else {
		return input();
		}
	return 0;
	}


	*/

