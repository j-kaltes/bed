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
#include "desturbe.h"
#include <string.h>
#include <stdlib.h>
#include "editor.h"
#include "main.h"
#include "stdlib.h"
#include "myalloc.h"
#include "screen.h"

void freeredos(Editor *ed) {
	while(ed->redos>ed->lastundo) {
		struct undo *unel=&ed->undobuf[ed->redos%ed->MAXUNDO];
		if(unel->len>0) {
			if(unel->str) {
				myrefree(unel->str);
				unel->str=NULL;
				}
			unel->len=0;
			}
		ed->redos--;
		}
	}


#define UNLIMITEDUNDO 1

inline struct undo *Editor::getnextundo(void) {
		lastundo++;
		if(lastundo>=(beginundo+MAXUNDO))  {
#ifdef UNLIMITEDUNDO
			int newlen = MAXUNDO*2;
			undobuf=(struct undo *)realloc(undobuf,newlen*sizeof(struct undo));
			memset(undobuf+MAXUNDO,0,(newlen-MAXUNDO)*sizeof(struct undo));
			MAXUNDO=newlen;
#else
			beginundo++;
#endif
			}
		struct undo *un=&undobuf[lastundo%MAXUNDO];
		if(redos>=lastundo) {
			if(un->len>0) {
				if(un->str) {
					myrefree(un->str);
					un->str=NULL;
					}
				un->len=0;
				}
			}
		return un;
		}


int Editor::popundo(unsigned char **buf,OFFTYPE *pos,OFFTYPE *diskused) {
		int len;
		*pos=undobuf[lastundo%MAXUNDO].pos;
		len=undobuf[lastundo%MAXUNDO].len;
		*buf= undobuf[lastundo%MAXUNDO].str;
		*diskused=undobuf[lastundo%MAXUNDO].usedondisk;

		undobuf[lastundo%MAXUNDO].str=NULL;
		lastundo--;
		return len;
		};
/*
Use editor procedures:
undoone:
	mem.truncatefile(pos);		
	resetnumbase() ;
	mem.extend(pos+ undosize);
	mem.putpart(pos,undosize,(char *)bufgegs)
	topos(pos);
	modified++;
	if(!shouldreinit)
					fromfile();

	strtoundoer(OFFTYPE pos,int size) ;
	mem.getpart(pos,size,(char *)(ptr->str));


Seperate editor from undo procedures
Editor procedures use general undo procedures
*/

int Editor::undoone(void) {
	if(lastundo>=beginundo) {
		if(searchdontedit())
			return -1;
		unsigned char *bufgegs;
		int undosize;
		OFFTYPE pos,useddisk;
		undosize=popundo(&bufgegs,&pos,&useddisk);
		if(undosize!=0) {
			INCDEST();
			memorize();
			int oldlastundo=lastundo;
			int oldredos=redos;
			if(undosize<0)  { /*Truncate undo extend*/
				OFFTYPE thesize=mem.filesize-pos;
				strtoundoer(pos,thesize);
				mem.truncatefile(pos);
				resetnumbase() ;
				pos--;
				undosize=1;
				mem.useddiskfilesize=useddisk;
				}
			else { /*size >=0 */
				if((pos+undosize)>mem.filesize) { /*Extend, undo truncate */
					if(pos!=mem.filesize) { 
						strtoundoer(pos,0);
						message("Skipped undo truncate: pos!=mem.filesize");
						lastundo=oldlastundo;
						redos=oldredos;
						if(bufgegs)
							myrefree( bufgegs);
						DECDEST();
						return 0;
						}
					mem.undotruncate(pos+ undosize,useddisk);
					resetnumbase() ;
					extendtoundoonly(pos);
					}
				 else {  /*Normal undo */
					strtoundoer(pos,undosize);
					}
				mem.useddiskfilesize=useddisk;
				mem.putpart(pos,undosize,(char *)bufgegs); /*Put undo buffer in filebuffer*/
				if(bufgegs)
					myrefree( bufgegs);
			    } /*end size>=0 */
			lastundo=oldlastundo;
			redos=oldredos;
			topos(pos);
			modified++;
			if(!shouldreinit)
				fromfile();
			updated=0;
			DECDEST();

			return undosize;
			} 
		else  { /*undosize==0*/
			return 0;
			}
		}
	else // lastundo<beginundo 
		return -1;
	};
int Editor::strtoundoer(OFFTYPE pos,int size) {
		if(pos<0||pos>=filesize()||size<=0)
			return -1;
		if((pos+size)>filesize()) {
			size=(filesize()-pos);
			}
		struct undo *ptr=getnextundo();
		ptr->len=size;
		ptr->pos=pos;
		ptr->str=(unsigned char *)expandchunk(ptr->str,size,0);
		ptr->usedondisk=mem.useddiskfilesize;
		mem.getpart(pos,size,(char *)(ptr->str));
		return 0;
		}

int Editor::appendtoundo(OFFTYPE pos, int size) {
		if(pos<0||pos>=filesize()||size<=0)
			return -1;
		if((pos+size)>filesize()) {
			size=(filesize()-pos);
			}
	struct undo *ptr;
	freeredos(this);
	if(lastundo>=beginundo&& (ptr=&undobuf[lastundo%MAXUNDO],ptr->len>=0&& (ptr->pos+ptr->len)==pos)) 
		{
		}
	else {
		ptr=getnextundo();
		ptr->pos=pos;
		ptr->len=0;
		ptr->str=NULL;
		ptr->usedondisk=mem.useddiskfilesize;//?
		};
	redos=lastundo;
	ptr->str=(unsigned char *)expandchunk(ptr->str,ptr->len+size,ptr->len);
	mem.getpart(pos,size,(char *)(ptr->str)+ptr->len);
	ptr->len+=size;
	return 0;
	}

/*
Start Editor interface */






int Editor::addundo(OFFTYPE pos,unsigned char ch) {
		if(pos<0||pos>=filesize())
			return -1;
		struct undo *ptr;
		freeredos(this);
		if(lastundo>=beginundo&& (ptr=&undobuf[lastundo%MAXUNDO],ptr->len>=0&& (ptr->pos+ptr->len)==pos)) 
			{
			}
		else {
			ptr=getnextundo();
			ptr->pos=pos;
			ptr->len=0;
			ptr->str=NULL;
			ptr->usedondisk=mem.useddiskfilesize;//?
			};
		redos=lastundo;
		ptr->str=(unsigned char *)expandchunk(ptr->str,ptr->len+1,ptr->len);
		ptr->str[ptr->len++]=ch;
		return 0;
		}
void Editor::initundo(void) {
	MAXUNDO=STARTMAXUNDO;
	undobuf=(struct undo *)calloc(MAXUNDO, sizeof(struct  undo));
//			memset(undobuf,0,sizeof(struct  undo)*MAXUNDO);
			beginundo=0;
			redos=lastundo=-1;
			undohalf=0xff;
			}
void Editor::delundo(void) {
	freeredos(this);
	while(lastundo>=beginundo) {
		struct  undo *unel=&undobuf[lastundo%MAXUNDO];
		if(unel->len>0) {
			if(unel->str)
				myrefree(unel->str);
			}
		lastundo--;
		}
	}
int Editor::strtoundo(OFFTYPE pos,int size) {
	int ret=0;
	if((ret=strtoundoer(pos,size))>=0) {
		freeredos(this);
		redos=lastundo;
		}
	return ret;
	};

int Editor::undopos(void)  {
	if(lastundo>=beginundo) {
		struct undo *ptr=&undobuf[lastundo%MAXUNDO];
		if(ptr->len>0)
			return ptr->pos+ptr->len;
		else
			return -10;
		}
	else	
		return -10;
	}
int Editor::inundo(OFFTYPE pos,unsigned char *buf,int size) {
		if(pos<0||pos>=filesize()||size<=0)
			return -1;
		if((pos+size)>filesize()) {
			size=(filesize()-pos);
			}
	freeredos(this);
		struct undo *ptr=getnextundo();
		ptr->len=size;
		ptr->pos=pos;
		ptr->str=buf;
		ptr->usedondisk=mem.useddiskfilesize;//?
		redos=lastundo;
		return 0;
		}


int Editor::appendstrtoundo(OFFTYPE pos,char *str, int size) {
		if(pos<0||pos>=filesize()||size<=0)
			return -1;
		if((pos+size)>filesize()) {
			size=(filesize()-pos);
			}
	struct undo *ptr;
	freeredos(this);
	if(lastundo>=beginundo&& (ptr=&undobuf[lastundo%MAXUNDO],ptr->len>=0&& (ptr->pos+ptr->len)==pos)) 
		{
		}
	else {
		ptr=getnextundo();
		ptr->pos=pos;
		ptr->len=0;
		ptr->usedondisk=mem.useddiskfilesize;//?
		ptr->str=NULL;
		};

	redos=lastundo;
	ptr->str=(unsigned char *)expandchunk(ptr->str,ptr->len+size,ptr->len);
	memmove(ptr->str+ptr->len,str,size);
	ptr->len+=size;
	return 0;
	}


int	Editor::extendtoundoonly(OFFTYPE oldsize) {
	struct undo *ptr=getnextundo();
	ptr->len=-1;
	ptr->pos=oldsize;
	ptr->usedondisk=mem.useddiskfilesize;//?
	ptr->str=NULL;
	return 0;
	}

int	Editor::extendtoundo(OFFTYPE oldsize) {
	freeredos(this);
	extendtoundoonly(oldsize);
	redos=lastundo;
	return 0;
	}
int Editor::undo(void) {
		int ret;
		while((ret=undoone())==0) {
			};
		if(ret<0)
			beep();
		return ret;
		}
int Editor::redo(void) {
		int ret;
		do {
			if(redos>lastundo) {
				lastundo++;


				if((ret=undoone())>-1) {
					lastundo++;
					}
				}
			else {
				beep();
				return -1;
				}
			} while(ret==0);
		return 0;
		}
