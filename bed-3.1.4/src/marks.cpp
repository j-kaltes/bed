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
#include "marks.h"
#include "myalloc.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
keylookup::keylookup():total(0) {keyhead=&treebase;memset(keyhead,0,sizeof(treebase));iter=head=&begin;};
keylookup::~keylookup() {
	deleteall();
	}
static int markcmp(const void *one, const void *two) {
	struct keylookup::mark *m1=*((struct keylookup::mark **)one),*m2=*((struct keylookup::mark **)two);
	OFFTYPE diff=(m1->pos-m2->pos);
	return diff>0?1:(diff<0?-1:0);
 	}
void keylookup::sort(void) {
	struct mark *pma[total];
	int i;
	tobegin();
	for(i=0;(pma[i]=next());i++) {
		
		}
assert(i==total);	
qsort(pma,total,sizeof( struct mark *),markcmp);
iter=head=&begin;
for(i=0;i<total;i++) {
	head->next=pma[i];
	head->next->prev=head;
	head=head->next;
	}
	}
void keylookup::test(void) {}

/*
void keylookup::test(void) {
	struct mark *el=iter;
	fprintf(stderr,"head: %p\n",head);
	while(el!=head) {
		el=el->next;
		if(el&&el->next==reinterpret_cast<struct mark *>(0x90000001e)) {
			fprintf(stderr,"0x90000001e\n");
			system("/bin/sh");
			}
		}
	}
*/	

int keylookup::putkeys(unsigned char *keys,int nr,OFFTYPE pos,Markgeg *des,uint32_t len) {
	total++;
	head->next=myalloc (struct mark);
	head->next->prev=head;

	head=head->next;

	head->pos=pos;
	head->len=len;
	head->des=des;

	head->keys=myallocar(unsigned char,nr);
	memcpy(head->keys,keys,nr);
	head->nrkeys=nr;
	return keymap(keys, nr, head) ;
	};
int keylookup::isstart(void) {
	return input.isstart();
};
int keylookup::select(struct keylookup::mark *mark) {
	if(mark==NULL)
		return -1;
	iter=mark;
	return 0;
	}
void keylookup::tobegin(void) {iter=&begin;};
struct keylookup::mark *keylookup::prev(void) {
	struct mark *tmp;
	if(iter!=&begin) {
		tmp=iter;
		iter=iter->prev;
		return tmp;
		}
	return NULL;
	}

int keylookup::deleteall(void) {
	int i;
	for(i=0;head!=&begin;i++)
		rmmark(head);
	return i;
	}
int keylookup::rmmark(struct keylookup::mark *mark) {
	if(mark==&begin)
		return -1;
	intptr_t  res =input.rmkeyseq(input.treebase,mark->keys,mark->nrkeys);
	if(res>0) {
		struct mark *was=(struct mark*)res;
		if(was!=mark) {
			keymap(mark->keys, mark->nrkeys, was) ;
			}
		 }
	mark->prev->next=mark->next;
	if(mark!=head) {
		mark->next->prev=mark->prev;
		if(iter==mark)
			iter=mark->next;

		}
	else {
		head=mark->prev;
		if(iter==mark)
			iter=head;
		}
	myfree(mark->keys);
	myfree(mark->des);
	myfree(mark);
	total--;
	return 0;
	}
/*
void keyleookup::defaultmark(int pos,char des) {
	head->next=myalloc (struct mark);
	head->next->prev=head;

	head=head->next;
	head->pos=pos;
	head->des=des;
	head->keys=NULL;
	}
*/
/*
int keylookup::putkey(unsigned char key, OFFTYPE  pos,Markgeg *des) {
	return putkeys(&key,1,pos,des,1);
	};
*/
struct keylookup::mark *keylookup::getinput(unsigned char *str,int len) {
	struct mark *mark=NULL;
 
	for(int i=0;(i<len)&&(!(mark=lookup(str[i])));i++)
		if(isstart())
			return NULL;
	return mark;
	};

struct keylookup::mark *keylookup::next(void) {
	if(iter!=head) {
		iter=iter->next;
		return iter;
		}
	return NULL;
	}
