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

#ifndef __RBTREE_H__
#define __RBTREE_H__
#include <features.h>
class	DrawTree;

#ifdef XViewer
#include "showtree.h"
#endif
#include <sys/types.h>
/*
#include <sys/ipc.h>
#include <sys/shm.h>
*/

/* Procedures specific for Data                                              */
#include "offtypes.h"
struct data {
	BLOCKTYPE blocknr;
	char *buf;
	BEDS_SIZE_T size;
	};
typedef struct data Data;
typedef BLOCKTYPE Key;

#define nr(x) (x.blocknr)
#define comparekey(one,two) ((one)-(two))
#define comparedat(one,two) comparekey(nr(one),nr(two))
#define compareel(first, second) comparedat((first->data),(second->data))

/* Tree structures                                                           */

#define NUMBEROFTREELS 0x100
typedef enum rbcolors {RBRED,RBBLACK} Color;
struct treeel {
	struct treeel *left;
	struct treeel *right;
	struct treeel *parent;
	Color	color;

	Data	data;

	};

typedef struct treeel Treel;

struct blocks {
//	key_t key;
	Treel *block;
	struct blocks *next;
	};

struct memhead {
	Treel *roottree;
	Treel *empty;
	struct blocks *block;
	Treel *enddata;
	Treel nilbase;
	};

//#define root() (treecontrol->roottree)
class Tree {
	public:
	struct memhead *treecontrol;
	Treel *NIL;
	void start(void); 
	void init(void); 
	Tree(void);
	void deletegegs(void) ;
	~Tree(void);
	Tree(Data*dat,int n);
	void empty(void);
	void leftrotate(Treel *x);
	void rightrotate(Treel *x); 

	void treeinsert(Treel *x); 
	Treel *treeminimum(Treel *x);
	Treel *treemaximum(Treel *x);
	Treel *treesuccessor(Treel *x);
	Treel *treesearch(Treel *x,Key key);

	Treel *search(Key k); 
	Treel *leftside(Key k); 
	Treel *leftside(Treel *x,Key k) ;
	Treel *rightside(Key k); 
	Treel *rightside(Treel *x,Key k) ;

	Treel *next(Treel *x);
	Treel* &root(void) { return treecontrol->roottree;};
	void rbinsert(Treel *x);
	void insert(Data dat);
	int del(Key dig); 
	int delel(Treel *el);
void	rbdeletefixup(Treel *x); 
Treel *rbdelete(Treel *z);

	void show(Treel *x,char *prefix,int len); 
	void show(Treel *x); 

	void show(void); 
	void showtree(void); 
	int showtree(int xleft,int ytop, Treel *t, int parentx ) ;

	Treel *allocate(unsigned long number) ;
	Treel *newsegment(void) ;
	void deltreel(Treel *el);
	Treel *newtreel(void) ;
	private:
#ifdef  XViewer
	DrawTree *view;
#endif // XViewer

	};



#endif /* __RBTREE_H__*/
