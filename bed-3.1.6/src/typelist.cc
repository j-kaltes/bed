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
#include <stdio.h>
#include <string.h>

#include "editor.h"
#include "typelist.h"

/*datatypes::datatypes(char  str[],ScreenPart *(*func)(Editor *ed)) {
	int len=strlen(str);
	if(maxlen>=nr) {
		maxlen=(maxlen+10)*2;

		struct namefunc **tmp=functions;
		functions=new (struct namefunc *)[maxlen];
		if(tmp) {
			memcpy(functions,tmp,nr*sizeof(struct namefunc));
			delete[] tmp;
			}
		}

	sorted=0
	functions[nr].name=new char[len+1];
	memcpy(functions[nr].name,str,len+1);
	functions[nr++].func=func;
	};
*/

typedef char *charptr ;
typedef		ScreenPart *(*Screenfuncptr)(Editor*);
datatypes::datatypes(const char  str[],ScreenPart *(*func)(Editor *ed)) {
	int len=strlen(str);
	if(maxlen>=nr) {
		maxlen=(maxlen+10)*2;

		char **tmp=names;
		names=new charptr[maxlen];
		if(tmp) {
			memcpy(names,tmp,nr*sizeof(char*));
			delete[] tmp;
			}
		ScreenPart *(**tmpfunc)(Editor*) =funcs;
		funcs=new Screenfuncptr[maxlen];
		if(tmpfunc) {
			memcpy(funcs,tmpfunc,nr*sizeof(ScreenPart *(*)(Editor *)));
			delete[] tmpfunc;
			}
		}

	sorted=0;
	names[nr]=new char[len+1];
	memcpy(names[nr],str,len+1);
	funcs[nr++]=func;
	};

ScreenPart *(*datatypes::getfunc(char *name,int len)) (Editor *) {
/*	if(!sorted)
		sort();
*/
	int i;
	for(i=0;i<nr;i++)
		if(!strncmp(name,names[i],len))
			return funcs[i];
	return NULL;
	}


//static 	struct namefunc *functions=NULL;
char **datatypes::names=NULL;
ScreenPart *(**datatypes::funcs)(Editor *ed) =NULL;
int datatypes::maxlen=0,datatypes::nr=0,datatypes::sorted=1;
/*
datatypes one("one");
datatypes two("two");
datatypes three("three");
*/
#include "dialog.h"


int rewriteall(void) ;
int resized(void) ;
ScreenPart *asktype(Editor *ed) {
	static int take=0;
	int ant;
	while((ant=selectitemindex("Data Types",datatypes::names,datatypes::nr,take))!=INT_MAX) {
			if(ant==(INT_MAX-1)) {
				resized();
				rewriteall();
				}
			else {
				take=ant;
				return datatypes::funcs[ant](ed);
				 }
		}
	return NULL;
	}

int Editor::listtypes(void) {
	ScreenPart *par=asktype(this);
	editup();
	if(par)
		return par->userconfig(0);
	return -1;
	}

/*
int Editor::listtypes(void) {
	static int take=0;
	int ant;
	while((ant=selectitemindex("Data Types",datatypes::names,datatypes::nr,take))!=INT_MAX) {
			if(ant==(INT_MAX-1)) {
				menuresized();
				wrefresh(editscreen);
				}
			else {
				editup();
				take=ant;
				ScreenPart *par= datatypes::funcs[ant](this);
				if(par)
					return par->userconfig(0);

				else
					message("System Error: can't init datatype");

				break;

				 }
		}
	editup();
	return -1;
	}
*/
