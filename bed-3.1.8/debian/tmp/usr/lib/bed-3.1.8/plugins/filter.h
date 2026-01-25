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
#ifndef FILTER_H
#define FILTER_H
#include <assert.h>
extern char nothing[];
class ScreenPart;
class Filter {
	public:
	ScreenPart *part;
	const char *name;
	char *label;
mutable	int bytesscreen; //bytes screen side. I said const
mutable	int bytesfile; //bytes file side
	int err; /*set on error */
//	Filter(int fb,int sb) ;
//	Filter(int fb,int sb,const char *n="filter", char *l=nothing);
	Filter(int fb,int sb,const char *n="filter", char *l=nothing): part(NULL), name(n),label(l),bytesscreen(sb),bytesfile(fb),err(0) {
		}
	Filter(Filter const &fil): 
		part(NULL),
		name(fil.name),
		label(fil.label),
		bytesscreen(fil.bytesscreen),
		bytesfile(fil.bytesfile),
		err(fil.err)
		{
		}

	//Filter(Filter &fil) ;
	virtual int setpart(ScreenPart *p);
	virtual char *convstr(void) const ;
	virtual int error(void) ;
	virtual int chbytes(int bytes)=0; //set sized with bytes on fileside given, returns bytes on screen side
	virtual int chbytesback(int bytes)=0; //set sized with bytes on screen side given, returns bytes on file side
	virtual Filter *newone(void) const =0;
	virtual ~Filter();
	virtual int fromfile(const unsigned char * const input,int &len,unsigned char *const output) const {
		return fromfile(input,output);
		};
	virtual int tofile(const unsigned char * const input,int &len,unsigned char * const output) {
		return tofile(input,output) ;
		};
	virtual int fromfile(const unsigned char * const input,unsigned char * const output) const {
	     assert(false	);
		return 0;
		};
	virtual int tofile(const unsigned char * const input,unsigned char * const output) {
	     assert(false	);
		return 0;
	};
	virtual int  equal(const Filter *fil) const;
	virtual int  wantaway(void) {return 1;}

	virtual int screen2filepos(int pos) ;
	virtual int file2screenpos(int pos) ;
	};

typedef Filter *(*Filterprocptr)(int,int,const char*,ScreenPart *);
extern int filtersmaxlen,filtersnr;
//extern char **filtersnames;
//extern Filter *(**filtersfuncs)(int,int,char*,ScreenPart *) ;
//extern char *initfilter(const char *str, Filter *(*func)(int,int,const char *,ScreenPart*)) ;
extern char*	initfilter(const char *str, Filterprocptr func) ;
extern Filterprocptr  getfilterfunc(const char *name,int len) ;

#define filtername(filtername) filtername##name
#define registerfilterclass(filtername) \
	Filter *get##filtername(int fb,int sb,const char *opts,ScreenPart *part=NULL) {\
		return new filtername(fb,sb,opts);\
		}\
	 char *filtername##name= initfilter(#filtername,&get##filtername);

#define registerfilterclasspart(filtername) \
	Filter *get##filtername(int fb,int sb,const char *opts,ScreenPart *part=NULL) {\
		return new filtername(fb,sb,opts,part);\
		}\
	char *filtername##name= initfilter(#filtername,&get##filtername);
#endif
