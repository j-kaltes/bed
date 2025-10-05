#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include "../screenpart.h"
#include "../ascii.h"
#include <limits.h>
#include <ctype.h>

static
#include "ebcdic2ascii.h"
static
#include "ascii2ebcdic.h"

#define datadecs	chconv(ascii,ebcdic)	chconv(ebcdic,ascii)
#undef chconv
#define chconv(from,to) class  from##to : public Ascii {		private:	public:	 from##to ( Editor *ed,int x1=0, int y1=0): Ascii( ed,x1, y1)	{	};		declareparent 	int addkey(unsigned char *bin,int binlen, int key, int pos ) override {			bin[pos]=::to##2##from[key];			return 0;			}	int form2bin(const char *formal, unsigned char *bin, int &len) const override {			for(int i=0;i<len;i++)			bin[i]=::to##2##from[(unsigned char)formal[i]];		return len;		}	ScreenPart *newone(void) const override {return new from##to(*this);}	int bin2formal(unsigned char *bin, char *formal, int len) override {			for(int i=0;i<len;i++) {			formal[i]=::from##2##to[bin[i]];			if(!isel(formal[i]))				formal[i]=nonprint;			}		return 0;	}		};
datadecs
#undef chconv
#define chconv(from,to) parentdef(Ascii, from##to, #from"2"#to)
datadecs
#undef chconv
#define chconv(from,to) datatypename(from##to, from##to::thename)
#include "../typelist.h"
datadecs
