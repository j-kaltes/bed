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
#ifndef __HASH_H__
#define __HASH_H__

#define PROTO(X)	X
#define POINTER void *
class Hash {
	public:
	typedef struct deel {
		  char *label;
		  POINTER plaats;
		  struct deel *next;
		  } Deel;
	typedef Deel * Hashtable;
	int  TABLESIZE;
	Hashtable *tab;
	Hash(int len=255): TABLESIZE(len) {
		tab=(Hashtable *) calloc(TABLESIZE,sizeof(Hashtable));
		}
    unsigned short hash(const char *str) const {return ((hashfunctie(str)) % TABLESIZE);};
	POINTER getitem PROTO((const char *label)) const;
	POINTER *itemptr PROTO((char *label));
void doortable(void *gegs,void (*funct)(void *,POINTER,POINTER)) const;
	void leeg();
    };




 POINTER *Hash::itemptr(char *label)
{
unsigned short index=hash(label);
Deel **item = &tab[index], *begin,*tijd;
for(;*item;item= &((*item)->next))
	if(!compfunctie(label,(*item)->label)) {
		return &(*item)->plaats;
		}
begin=tab[index];
tijd=tab[index]=(Deel *) malloc(sizeof(Deel));
tijd->label=label;
tijd->next=begin;
tijd->plaats=NULL;
return &(tijd->plaats);
}
 POINTER Hash::getitem(const char *label) const
{
unsigned short index=hash(label);
Deel *deel;
for(deel=tab[index];deel;deel=deel->next)
	if(!compfunctie(label,deel->label))
		return deel->plaats;
return NULL;
}
/* past procedure funct toe op elke plaats. Om toe te passen op elke*/
 void Hash::doortable(void *gegs,void (*funct)(void *,POINTER,POINTER)) const
{
Deel *item;
int i;
for(i=0;i<TABLESIZE;i++)
	for(item=tab[i];item;item= item->next)
                (*funct)(gegs,item->label,item->plaats);
}




 void Hash::leeg(void) {
int i;
Deel *item,*oud;
for(i=0;i<TABLESIZE;i++) {
	for(item=tab[i];item;) {
		  oud=item;
		  item=item->next;
		  free(oud);
		  }
	tab[i]=(Deel *)NULL;
	}
}

#endif
