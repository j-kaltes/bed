#include "defines.h"
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



/* Implements some procedures for Red-Black Trees as described in            */
/* Introduction to Algorithms of Thomas H. Cormen, Charles E. Leiserson      */
/* and Ronald L. Rivest.                                                     */
/*                                                                           */
/* It adds nothing. It's just to see it work.                                */
/*                                                                           */
/* Red-black Trees                                                           */
/*                                                                           */
/* Trees in which every node has a color and there are the following         */
/* constraints on nodes:                                                     */
/* 1. Every node is either red or black;                                     */
/* 2. Every leaf is black;                                                   */
/* 3. both children of a red node are black;                                 */
/* 4. every path to a leave has the same number of black nodes.              */
/*                                                                           */
/* A red-black tree with n internal nodes has a height of at                 */
/* most 2log(n+1).                                                           */
/* Fri Jul 31 16:19:01 1998                                                  */

#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include "rbtree.h"
#include "system.h"
#include "myalloc.h"

#define right(x) ((x)->right)
#define left(x) ((x)->left)
#define parent(x) ((x)->parent)
#define p(x) ((x)->parent)
#define color(x) ((x)->color)
#define thenil (treecontrol->nilbase)

#define data(x) ((x)->data)
#define key(x) nr(data(x))


#define DEBUG(x,y) fprintf(stderr,x,y);
void Tree::init(void) {
	treecontrol=(struct memhead*) allocate(sizeof(struct memhead)+sizeof(Treel ));

	NIL=&treecontrol->nilbase; treecontrol->nilbase.color=RBBLACK; treecontrol->nilbase.data.blocknr=0;
	treecontrol->nilbase.data.buf=NULL; treecontrol->nilbase.data.size=0;
	treecontrol->nilbase.left=NIL; treecontrol->nilbase.right=NIL; treecontrol->nilbase.parent=NIL;

	treecontrol->roottree=NIL;
	treecontrol->block=NULL;
	treecontrol->empty=(Treel  *)( (char *)treecontrol+sizeof(struct memhead));
	newsegment();
#ifdef XViewer
	view=NULL;
#endif //XViewer
	}
Tree::Tree(void) {
	init();
	}


Tree::Tree(Data dat[],int n) {
	int i;
	init();
	for(i=0;i<n;i++) 
		insert(dat[i]);
	}
Tree::~Tree(void) {
#ifdef XViewer
	if(view)
		delete view;
#endif
	deletegegs();
	}
/*
Treel *Tree::allocate(unsigned long size) {
	Treel *ptr=(Treel *)new char[size];
	memset((char*)ptr,'\0',size);
	return ptr;
	}
Treel *Tree::newsegment(void) {
	struct blocks *temp=(struct blocks*) treecontrol->empty;
	temp->next=treecontrol->block;
	temp->block=allocate(NUMBEROFTREELS*sizeof(Treel));
	treecontrol->block=temp;
	treecontrol->empty=temp->block;
	treecontrol->enddata=&treecontrol->empty[NUMBEROFTREELS-1];
	return treecontrol->empty;
	}
	*/
void Tree::deletegegs(void) {
	for(struct blocks *weg=treecontrol->block;weg;weg=weg->next) {
		myfree ((char *)(weg->block));
		}
	myfree ((char *)treecontrol);
	}
void Tree::empty(void) {
	deletegegs();
	init();
	}
void Tree::leftrotate(Treel *x) {
	Treel *y;


	y=right(x);
	right(x)=left(y);
	if(left(y)!=NIL)
		p(left(y))=x;
	parent(y)=parent(x);
	if(parent(x)==NIL) 
		root()=y;	
	else {
		if(x==left(p(x)))
			left(p(x))=y;
		else
			right(p(x))=y;

		}

	left(y)=x;
	p(x)=y;
	}
			

void Tree::rightrotate(Treel *x) {
	Treel *y;


	y=left(x);
	left(x)=right(y);
	if(right(y)!=NIL)
		p(right(y))=x;
	parent(y)=parent(x);
	if(parent(x)==NIL) 
		root()=y;	
	else {
		Treel *& lpx=right(p(x));
		if(x==lpx)
			lpx=y;
		else
			left(p(x))=y;

		}

	right(y)=x;
	p(x)=y;
	}
			
#define MAXTREEDEPTH 500

void Tree::show(Treel *x,char *prefix,int len) {
	if(x==NIL)
		return;
	strcpyn(prefix+len,"   ",MAXTREEDEPTH-len);
	show(right(x),prefix,len+3);
	prefix[len]='\0';

	if(color(x)==RBBLACK) 
		printf("%s." BLOCKPRINT "\n",prefix,key(x));
	else
		printf("%s " BLOCKPRINT "\n",prefix,key(x));
	strcpyn(prefix+len,"   ",MAXTREEDEPTH-len);
	show(left(x),prefix,len+3);
	}
Treel *Tree::next(Treel *x) {
	if(x==NIL)
		return NIL;
	Treel *y,*z;
	if(left(x)!=NIL)
		return left(x);
	else {
		if(right(x)!=NIL)
			return right(x);
		else {
			y=x;
			do {
				x=y;
				y=p(x);
				z=right(y);
				if(y==NIL||(y==root()&&x==z))
					return NIL;
				} while(z==NIL||x==z);
			return z;
			}
		}
	}
void Tree::rbinsert(Treel *x) {
	Treel *y;
	treeinsert(x); 
	color(x)=RBRED;
	while(x!=root()&&(color(p(x))==RBRED)) {
		if(p(x)==left(p(p(x)))) {
			y=right(p(p(x)));
			if(color(y)==RBRED) {
				color(p(x))=RBBLACK;
				color(y)=RBBLACK;
				color(p(p(x)))=RBRED;
				x=p(p(x));
				}
			else {
				if(x==right(p(x))) {
					x=p(x);
					leftrotate(x);
					}
				color(p(x))=RBBLACK;
				color(p(p(x)))=RBRED;
				rightrotate(p(p(x)));
				}
			}
		else	
			{
			y=left(p(p(x)));
			if(color(y)==RBRED) {
				color(p(x))=RBBLACK;
				color(y)=RBBLACK;
				color(p(p(x)))=RBRED;
				x=p(p(x));
				}
			else {
				if(x==left(p(x))) {
					x=p(x);
					rightrotate(x);
					}
				color(p(x))=RBBLACK;
				color(p(p(x)))=RBRED;
				leftrotate(p(p(x)));
				}
		}
	}
color(root())=RBBLACK;
   }
void Tree::treeinsert(Treel *z) {
	Treel *y=NIL;
	Treel *x=root();
	while(x!=NIL) {
		y=x;
		if(compareel(z,x)<0)
			x=left(x);
		else
			x=right(x);
		}
	p(z)=y;
	if(y==NIL) 
		root()=z;
	else {
		if(compareel(z,y)<0)
			left(y)=z;
		else
			right(y)=z;
		}
	}

void Tree::insert(Data dat) {
	Treel *ins=newtreel();
	ins->data=dat;
	ins->left=ins->right=ins->parent=NIL;
	ins->color=RBBLACK;
	rbinsert(ins);
	}


Treel *Tree::allocate(unsigned long size) {
	Treel *ptr=(Treel *)myallocar( char,size);
	memset((char*)ptr,'\0',size);
	return ptr;
	}
Treel *Tree::newsegment(void) {
	struct blocks *temp=(struct blocks*) treecontrol->empty;
	temp->next=treecontrol->block;
	temp->block=allocate(NUMBEROFTREELS*sizeof(Treel));
	treecontrol->block=temp;
	treecontrol->empty=temp->block;
	treecontrol->enddata=&treecontrol->empty[NUMBEROFTREELS-1];
	return treecontrol->empty;
	}
void Tree::deltreel(Treel *el) {
	el->left=treecontrol->empty;
	treecontrol->empty=el;
	}
Treel *Tree::newtreel(void) {
	Treel *tijd;
	if(treecontrol->empty==treecontrol->enddata) 
		newsegment();
	tijd=treecontrol->empty;
	if(treecontrol->empty->left) {
		treecontrol->empty=treecontrol->empty->left;
		}
	else  
		treecontrol->empty++;	

	return tijd;
	}


Treel *Tree::treeminimum(Treel *x) {
	while(left(x)!=NIL) 
		x=left(x);	
	return x;
}

Treel *Tree::treemaximum(Treel *x) {
	while(right(x)!=NIL) 
		x=right(x);	
	return x;
}
Treel *Tree::treesuccessor(Treel *x) {
	Treel *y;
	if(right(x)!=NIL)
		return treeminimum(right(x));
	y=p(x);
	while(y!=NIL && x==right(y)) {
		x=y;
		y=p(y);
		}
	return y;
	}

Treel *Tree::search(Key k) {
	return treesearch(root(),k);
	}


Treel *Tree::rightside(Key k) { 
	return rightside(root(),k);
	}

Treel *Tree::rightside(Treel *x,Key k) {
	Treel *old=x;
	Key co;
	while(x!=NIL && (co=comparekey(k,key(x)),co))  {
		old=x;
		if(co>0) {
			 x=right(x);	
			if(x==NIL) {
				while(old!=NIL && x==right(old)) {
					x=old;
					old=p(old);
					}
				x=old;
				break;
				}
			}
		else  {
			x=left(x);
			}
		}
	if(x==NIL)
		return old;
	return x;
	}
Treel *Tree::leftside(Key k) { 
	return leftside(root(),k);
	}
Treel *Tree::leftside(Treel *x,Key k) {
	Treel *old=x;
	Key co;
	while(x!=NIL && (co=comparekey(k,key(x)),co))  {
		old=x;
		if(co<0) {
			 x=left(x);	
			if(x==NIL) {
				while(old!=NIL && x==left(old)) {
					x=old;
					old=p(old);
					}
				x=old;
				break;
				}
			}
		else  {
			x=right(x);
			}
		}
	if(x==NIL)
		return old;
	return x;
	}

Treel *Tree::treesearch(Treel *x,Key k) {
	Key co;
	while(x!=NIL && (co=comparekey(k,key(x)),co)) 
		if(co<0)
			x=left(x);	
		else
			x=right(x);
	return x;
	}

void	Tree::rbdeletefixup(Treel *x) {
	Treel *w;
	while(x!=root()&&color(x)==RBBLACK) {
		if(x==left(p(x))) {
			w=right(p(x));
			if(color(w)==RBRED) {
				color(w)=RBBLACK;
				color(p(x))=RBRED;
				leftrotate(p(x));
				w=right(p(x));
				}
			if(color(left(w))==RBBLACK&&color(right(w))==RBBLACK) {
				color(w)=RBRED;
				x=p(x);
				}
			else {
				if(color(right(w))==RBBLACK) {
					color(left(w))=RBBLACK;
					color(w)=RBRED;
					rightrotate(w);
					w=right(p(x));
					}
				color(w)=color(p(x));
				color(p(x))=RBBLACK;
				color(right(w))=RBBLACK;
				leftrotate(p(x));
				x=root();
				}
		}
	else {
			w=left(p(x));
			if(color(w)==RBRED) {
				color(w)=RBBLACK;
				color(p(x))=RBRED;
				rightrotate(p(x));
				w=left(p(x));
				}
			if(color(right(w))==RBBLACK&&color(left(w))==RBBLACK) {
				color(w)=RBRED;
				x=p(x);
				}
			else {
				if(color(left(w))==RBBLACK) {
					color(right(w))=RBBLACK;
					color(w)=RBRED;
					leftrotate(w);
					w=left(p(x));
					}
				color(w)=color(p(x));
				color(p(x))=RBBLACK;
				color(left(w))=RBBLACK;
				rightrotate(p(x));
				x=root();
				}
		}
	}
	color(x)=RBBLACK;
}

Treel *Tree::rbdelete(Treel *z) {
	Treel *y,*x;
	if(left(z)==NIL || right(z)==NIL)
		y=z;
	else
		y=treesuccessor(z);
	if(left(y)!=NIL)
		x=left(y);
	else
		x=right(y);
	p(x)=p(y);
	if(p(y)==NIL)
		root()=x;
	else {
		if(y==left(p(y)))
			left(p(y))=x;
		else
			right(p(y))=x;
		}
	if(y!=z)
		data(z)=data(y);
	if(color(y)==RBBLACK)
		rbdeletefixup(x);
	return y;
	}
int Tree::delel(Treel *el) {
	if(el!=NIL) {
		deltreel(rbdelete(el));
		return 1; 
		}
	else
		return 0;
	};
int Tree::del(Key dig) {
	Treel *el;
	el=search(dig);
	return delel(el);
	}




void Tree::show(Treel *x) {
	char buf[MAXTREEDEPTH];
	show(x,buf,0);
	}
#ifdef XViewer
int Tree::showtree(int xleft,int ytop, Treel *t, int parentx ) {
	char buf[11];
	int newright=xleft+1,newleft=xleft;
	if(t==NIL) {
		return xleft;
		}
	newleft=showtree(xleft,ytop+1,left(t),-1);
	newright=showtree(newleft+1,ytop+1,right(t),newleft);
	if(parentx<0)
		parentx=newright;
	if(ytop>=0)
		view->drawline(parentx,ytop,newleft);
		
	snprintf(buf,11,"%d",nr(data(t)));
//	strcpyn(buf,data(t).buf,8); buf[8]='\0';
	view->drawnode(newleft,ytop+1,buf,((color(t)==RBRED)?1:0));
	return  newright;
	}
void Tree::showtree(void) {
	if(!view)
		view=new DrawTree;
	else
		view->clear();
	showtree(0,-1,root(),0);
	view->flush();
        }

#endif // XViewer
void Tree::show(void) {
	show(root());
	}

#ifdef TEST
char *    	nospace(char *ptr) {
    	while(isspace(*ptr))
    		ptr++;
    	return ptr;
    	}

#include <string.h>
void help() {
	puts("i dig: insert dig");
	puts("d dig: delete dig");
	puts("l dig: leftside of dig");
	puts("r dig: rightside of dig");
	puts("/ dig: search for dig");
	puts("s    : show tree");
	puts("h    : help");
	puts("q    : quit");
	}
int main( int argc, char **argv )
{
	Treel *tt;
    int i,total,goon=1;
    char buf[180],*ptr,ch;
    argv++;argc--;
    Data *gegs=(Data *)new Data[argc];
    for(i=0,total=0;i<argc;i++) {
    	if(argv[i][0]>='0'&&argv[i][0]<='9'||(argv[i][0]=='-'&&argv[i][1]>='0'&&argv[i][1]<='9')) 
		gegs[total++]=(Data){atoi(argv[i]),argv[i],sizeof(argv[i])};
	}
    Tree *tree=new Tree(gegs,total);
    help();
    while(goon) {
    	gets(buf);
    	ptr=nospace(buf);
    	ch=*ptr++;
	ptr=nospace(ptr);sscanf(ptr,"%d",&i);
    	switch(ch) {
    		case 'i':tree->insert((Data){i,ptr,0});break;
    		case 'd':tree->del(i);break;
    		case 'r':
    			if((tt=tree->rightside(i))!=tree->NIL)
				printf("The first on the right is %d\n",tt->data);
			else
				printf("nothing on the right\n");
    			break;
    		case 'l':
    			if((tt=tree->leftside(i))!=tree->NIL)
				printf("The first on the left is %d\n",tt->data);
			else
				printf("nothing on the left\n");
    			break;
    		case '/':
    			if((tt=tree->search(i))!=tree->NIL)
				printf("%d exists\n",i);
			else
				printf("%d doesn't exist\n",i);
    			break;
    		case 's': tree->showtree();break;
    		case 'c': tree->empty();break;
    		case 'f': for(tt=tree->treecontrol->roottree;tt!=tree->NIL;tt=tree->next(tt))
				printf("%d\n",tt->data.blocknr);
			break;
    		case 'q': goon=0;break;
    		default: help();break;
    		}
    	}
}
#endif
