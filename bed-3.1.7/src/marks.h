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
#ifndef MARKS_H
#define MARKS_H
#include "keyinput.h"

#include "offtypes.h"
class keylookup {
	int total;
public:
typedef  char Markgeg;
struct mark  {
	char nrkeys;
	unsigned char *keys;
	 OFFTYPE pos;
	 uint32_t len;
	Markgeg *des;
	struct mark *next,*prev;
	};

struct tree {
	int nr;
	unsigned char ch;
	union  {
		struct tree *children;
		struct mark *mark;
		};
	struct tree *next;
	};

struct tree treebase,*keyhead;
struct mark *head,begin,*iter;
keyinput input;
keylookup();
~keylookup();
int rmmark(struct mark *mark) ;
int deleteall(void) ;
int isstart(void);
struct mark *next(void) ;
int select(struct mark *mark) ;
void tobegin(void);
struct mark *prev(void);
inline int gettotal(void) {return total;};
void sort(void);
/*
void defaultmark(int pos,char *des); 
*/
//int putkey(unsigned char key, OFFTYPE pos,Markgeg *des); 

//int putkeys(unsigned char *keys,int nr,OFFTYPE pos,Markgeg *des) ;
int putkeys(unsigned char *keys,int nr,OFFTYPE pos,Markgeg *des,uint32_t len) ;
struct mark *getinput(unsigned char *str,int len);
/*inline struct mark *keylookup::lookup(unsigned char ch);
inline int keymap(unsigned char *keys,int nr, struct mark *mark );
*/
#define wrongkeyseq(res) (((res)==-1)||((res)==-2))
//inline static void tomark(const struct mark *mark) const;
inline struct keylookup::mark *lookup(unsigned char ch) {
	intptr_t  res=input.keyseq(ch);
	if(wrongkeyseq(res))
		return NULL;
	return (struct mark*)res;
	}
inline int keymap(unsigned char *keys,int nr, struct mark *mark ) {
	return input.putseq((intptr_t)mark, nr, keys) ;
	}
	void test();
};


#define MAXMARKALL 58
#define MAXMARKDAT (MAXMARKALL-maxnumstr-3)
#endif
