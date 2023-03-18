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



#ifndef KEYINPUT_H
#include <features.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define KEYINPUT_H
class keyinput;
/*
tree
	ch1
	  nr=3
	  children:
		nr=0
		cha
			sym	
	      next
		chb
			...
	      next
		chc
			..
	     next=NULL
   next
	ch2
	..
   next
	ch3
	..
   next=NULL
*/	
class keyinput {
	public:
	struct tree {
		int nr; /*Number of children 0 if sym*/
		unsigned char ch;
		union  {
			struct tree *children;
			struct tree **table;
			intptr_t  sym;
			};
		struct tree *next;
		};
	struct tree *keyhead;
	struct tree basetree;
	struct tree *treebase;
intptr_t   keyseq(unsigned char input) ;
intptr_t   rmkeyseq(struct tree *start,unsigned char *input,int nrkeys) ;
	int putseq(const intptr_t sym, int nr,const unsigned char *keys) ;
/*	int putseq(void *sym,int nr, unsigned char *keys) {
		return putseq((long)sym,nr,keys);
		}
*/
	inline int isstart(void) {return keyhead==treebase;};
	inline void reset(void) {keyhead=&basetree;};
	keyinput(void) { 
		keyhead=treebase=&basetree;
		memset((char*)treebase,'\0',sizeof(basetree));
		};
	keyinput( void (*fun)(class keyinput *inp))  { 
		keyhead=treebase=&basetree;
		memset((char*)treebase,'\0',sizeof(basetree));
		fun(this);
		};
	~keyinput(); 
	};
//#define listshow 252
#define menukey 253
#define secondescape 254
#define STATICKEYS 1
#endif
