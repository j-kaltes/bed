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
#include "keyinput.h"
#include "myalloc.h"
#include "debug.h"

extern void	freekeys(struct keyinput::tree *par) ;
#define tablelimit 10 /*WHen more then this amount of keys is under one key a table is used */
keyinput::~keyinput() {freekeys(&basetree);}
intptr_t keyinput::keyseq(unsigned char input) {
	struct tree *iter;
DEBUGGING("keyseq ");
	if(keyhead->nr>tablelimit) {
		if(!(iter=keyhead->table[input])) {
			keyhead=treebase;
			return -2L; 
/*pointers take 4 bytes so -2 (0xFFFFFFFE) can't be a pointer */
			}
		if(iter->nr>0)	{
			keyhead=iter;
			return -1L;
			}
		keyhead=treebase;
DEBUGGING(">tablelimit ");
		return iter->sym;
		}
	iter=keyhead->children;
	for(int i=0;i<keyhead->nr;i++,iter=iter->next) 
		if(iter->ch==input) {
			if(iter->nr>0) {
				keyhead=iter;
				return -1L;
				}
			keyhead=treebase;
		DEBUGGING("<=tablelimit ");
			return iter->sym;
			};
	keyhead=treebase;
	return -2L;

	}
#include <stdio.h>
extern void showkeys(FILE*,unsigned char *,int ) ;


#define maxtable 256
void	freekeys(struct keyinput::tree *par) {
	struct keyinput::tree *next,*iter;
	if(!par->nr)
		return;
	if(par->nr>tablelimit) {
		for(int i=0;i<maxtable;i++) 
			if((iter=par->table[i]))
				freekeys(iter);
		myfree(par->table);
		return ;
		}

	iter=par->children;
	for(int i=0;i<par->nr;i++,iter=next) {
		next=iter->next;
		freekeys(iter);
		myfree( iter);
		}
	par->nr=0;
	par->children=NULL;
	}

int keyinput::putseq(const intptr_t sym,int nrkeys, const unsigned char *keybuf) {
struct tree *iter, *par= treebase;
int i;
again:

if(par->nr>tablelimit) {
	if((iter=par->table[*keybuf])) {
		if(nrkeys==1) {
			freekeys(iter);
			iter->nr=0;
			iter->sym=sym;
			return 0;
			}
		par=iter;
		nrkeys--;
		keybuf++;
		goto again;
		}
	}
else {
	iter=par->children;
	for(i=0;i<par->nr;iter=iter->next,i++) {
		if(iter->ch==*keybuf)	{
			if(nrkeys==1) {
				freekeys(iter);
				iter->nr=0;
				iter->sym=sym;
				return 0;
				}
			par=iter;
			nrkeys--;
			keybuf++;
			goto again;
			}
		}
	}
while(nrkeys) {
	if(par->nr==tablelimit) {
		iter=par->children;
		struct tree **table=par->table= myallocar(struct tree *,maxtable) ;
		memset(table,0,sizeof(struct tree *)*maxtable);
		for(i=0;i<par->nr;iter=iter->next,i++) {
			table[iter->ch]=iter;
			}
		}
	if(par->nr>=tablelimit) { /*put in table */
		par->table[*keybuf]= myalloc(struct tree);
		par->nr++;
		par= par->table[*keybuf];
		}
	else {
		if(par->nr)
			iter=par->children;
		else
			iter=NULL;

		par->children=myalloc(struct tree);
		par->nr++;
		par=par->children;
		par->next=iter;
		}

	par->ch=*keybuf++;
	par->nr=0;
	par->children=NULL;
	nrkeys--;
	}
par->sym=sym;
return 0;
}

intptr_t   keyinput::rmkeyseq(struct tree *start,unsigned char *input,int nrkeys) {
		struct tree **iter,*temp;
		int i;
		intptr_t   ret;
		if(nrkeys<=0)	
			return -3;
		for(i=0,iter=&(start->children);i<start->nr;i++,iter=&((*iter)->next)) 
			if((*iter)->ch==*input) {
				if((*iter)->nr==0) {
					if(nrkeys!=1)
						return -4;	
					ret=(*iter)->sym;
					}
				else
				     {
				       if(nrkeys<2)
				       		return -5;
					if((ret=rmkeyseq(*iter,input+1,nrkeys-1))<0)
						return ret;
					if((*iter)->nr>0) {
						return ret;
						}
					}

				temp=*iter;
				*iter=(*iter)->next;
				start->nr--;
				myfree(temp);
				return ret;
				};
		return -2;
	}




