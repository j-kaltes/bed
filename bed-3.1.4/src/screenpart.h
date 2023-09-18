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
#ifndef SCREENPART_H
#define SCREENPART_H
#include <limits.h>
#include "type.h"
const int maxlabel=80;

class Editor;
class Filter;
#define General ScreenPart

extern const char screenpartname[];
class Parent {
	const char * const name;			
	const class Parent *  paren;
	public:
	Parent(const char *nam,const class Parent * const par): name(nam), paren(par)
{
//		paren=par;
		};
	const char *getname(void) const {return name;}
	int isparent(const char *nam) const;	
	};	
//char child::thename[]=childname;
#define parentdef(par,child,childname)  class Parent child::desc=Parent(childname,&par::desc);
#define thename desc.getname()
#define parentd(par,child) parentdef(par,child,#child)
#define declareparent \
	public:\
	static class Parent desc; \
	virtual class Parent *getdesc(void) const override { return &desc;};

//	static  char thename[]; 
#define  has_super(inst,Type) (inst&&inst->isparent(Type::desc.getname()))
#define asciichild(x) has_super(x,Ascii)
#define instance(inst,Type) (inst->getname()==Type::desc.getname())
class ScreenPart {
//	declareparent
	public:
	static class Parent desc; 
	virtual class Parent *getdesc(void) const  { return &desc;};
	Editor *editor;
	ScreenPart *contain;
	int left,top,spaceafter,maxres,apart,bytes,half,oldhalf;
	DataType type;
//	const char *name;
	char label[maxlabel];
	char *userlabel;
	int begin(void) {return half=0;};
	int end(void) {
		oldhalf=half;
		return  (half=(maxres-1));
		};
	const char *getname(void)const  {return getdesc()->getname();}
   	int isparent(const char *nam) const {return getdesc()->isparent(nam);}

	inline int x(int pos) { return (((pos%nrx())/bytes)*(maxres+apart));}; /*** position to x **/
	inline int y(int pos) { return (pos/nrx());};		/*** position to y **/
	inline void postoxy(int pos,int &x1,int &y1) { /*** convert position on screen to x and y **/
			y1=y(pos);
			x1=x(pos);
			};
	int &nrx(void); /*max number of elements in x direction**/
	int &nry(void); /*max number of elements in y direction**/
	inline int oldpos(void); /*Previous position on screen of cursor **/
	inline void placepart(int l, int t) { left=l;top=t;}; /** sets left and top position of screenpart **/
	int movepos(int pos); /* Move to put pos */
	inline int cols(void) {return nrx();};
	inline int rows(void) {return nry();};
	int partwidth(void);
	void troggleselect(int pos); /*switch between selected and not selected */
	virtual int chbytes(int bytes)  {return -1;};
	virtual short chbase(short base) {return -1;};
// s/^\([^(]*\)ScreenPart::/\1 GeenScreenPart /g
#define GeenScreenPart 

 int datbytes(void) const; 
inline int datbytes(const int b)  { return screenupdatefilters(b); }

	virtual int userconfig(int kind);
	ScreenPart(ScreenPart const &part);
	ScreenPart( Editor *ed,int max,int b,int len=4,int x1=0, int y1=0, int a=1);

	ScreenPart() = delete;
	ScreenPart& operator=(ScreenPart const&) = delete;
	virtual ~ScreenPart();
	virtual ScreenPart *newone(void) const =0;
	int nocursor(void); /* removes cursor of current position */
	void cursorpos(int pos);
	void leave(void) {oldhalf=half;half=maxres;};
	int newchar(int key);

	int addchar(int key) ;


#define fromascii(to,from, len)  bin2search((from),(char *)(to),len)

	 int filterformat2bin(const char *from, unsigned char *to, int len) ;
 int filformat2bin(const char * const from, unsigned char *to, int &len) const;
	 int bin2f(unsigned char *bin, char *formal, int len) ;
	virtual int isel(const int key) const=0; 
	int select(void); 
	 int backchar(void) ;
	 int nextchar(void) ;
	int rewritepart(int size) ;
	int putline(int y) ;
	void unittoscreen(int pos) ;
	void printchar(int pos) ;
	inline int oppos(int pos) ;
	int oppos(int pos,int size) ;
	virtual int addkey(unsigned char *bin,int binlen, int key, int pos ) ;
/*formal=in dstatype bin: binary. len: gives back lenght of formal used 
 returns number of bytes used
*/
inline	virtual int form2bin(const char *formal, unsigned char *bin, int &len) const {
		len=format2bin(formal, bin,  len); 
		return datbytes();
		}
inline	virtual int format2bin(const char *formal, unsigned char *bin, int len)const {
		formal2bin(formal, bin,  len); 
		return len;
		}
//convert dataspecific string (formal) to binary array (bin). len is length of formal
	virtual int formal2bin(const char *formal, unsigned char *bin, int len) const {
		return 0;
		}

/*convert binary array (bin) to dataspecific string (formal). len is length
of bin*/
	virtual int bin2formal(unsigned char *bin, char *formal, int len) =0;

//virtual int  GeenScreenPart search2bin(const char *formal, unsigned char *bin, int len)  ;

 int search2bin(const char *const formal, unsigned char *bin, const int len,int binmax=INT_MAX) const ;
int  GeenScreenPart bin2search(unsigned char *bin,  char *formal, int len)  ;
	virtual void poschanged(int pos) {};
	public:
	Filter **filters;
	int firstfilter,lastfilter,maxfilter;
	public:
	int fromfilefilters(const char * const input,int &len,char *output) const ;
	int tofilefilters(char *input,int &len,char *output) const ;
virtual	int addfilter(Filter *fil) ;
virtual int  GeenScreenPart setfilter(Filter *fil) ;
	int getbytes(void) const;
int  GeenScreenPart filebytes(int b) ;
int  GeenScreenPart screenbytes(int b) ;
int  GeenScreenPart addreverse(int  len, int un);
int  GeenScreenPart screenupdatefilters(int byt) ;
int  GeenScreenPart addskip(int  take, int skip,int byt) ;
	virtual void mklabel(void);
	ScreenPart *getsubpart(int &forstart,int &binstart,int pos);
	virtual ScreenPart * GeenScreenPart getchild(int &forstart,int &binstart,int pos);
	int  GeenScreenPart rmlastfilt(char *name=NULL);
	int  GeenScreenPart rmlastfilter(void);
	char * GeenScreenPart lastfiltername(void);
	Filter * GeenScreenPart getlastfilter(void) ;
	int  GeenScreenPart equal(const ScreenPart *part) const;
	int  GeenScreenPart reverse(void) ;
	int  GeenScreenPart allocfilters(int nr) ;
	int  GeenScreenPart copyfiltersbefore(ScreenPart *part) ;
	int  GeenScreenPart copyfiltersafter(ScreenPart const *part) ;
	int	 GeenScreenPart mkfilterstr(char *str,int max) ;
	int  GeenScreenPart filtersstring(char *buf) ;
	virtual int calcsizes(void) ;
	virtual char *getconvstr(void);
	virtual int setconvstr(char *str);
ScreenPart * GeenScreenPart getchild(void) ;
ScreenPart * GeenScreenPart getparent(ScreenPart *start=NULL) ;
ScreenPart * GeenScreenPart getsubpart(void) ;
virtual int  GeenScreenPart byteoff2screen(int pos) {
		return -1;
		}
virtual int  GeenScreenPart screen2byteoff(int pos) {
		return -1;
		}
int  GeenScreenPart byte2screen(int pos) ;
int  GeenScreenPart screen2byte(int pos) ;
int  GeenScreenPart fromfilefilterspos(int pos) const ;
int  GeenScreenPart tofilefilterspos(int pos) const ;
ScreenPart * GeenScreenPart getsuperpart(void) ;
int  GeenScreenPart getbase(void);
char * GeenScreenPart takeuserlabel(void);
int sizelabels(int kind,int nr,...); 
int sizelabelsnl(int kind,int nr,...); 
void setuserlabel(const char *label) ;
	};

extern int getindatatype(const ScreenPart * const thesub) ;
	
/*	ScreenPart( Editor *ed,int max,int b,int len=4,int x1=0, int y1=0, int a=1)
ed: 
max: each elements screenbreath
b: Signifier
len: number of bytes
x1, y1: transform every position of screenpart. This is never used so, untested
a: number of spaces between each element
*/
#include "typelist.h"
#endif
