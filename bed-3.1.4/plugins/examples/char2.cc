#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include "global.h"
BEDPLUGIN
#include "screenpart.h"
class  char2 : public ScreenPart {	
	declareparent 
	public:	 
	char2 ( Editor *ed): ScreenPart( ed, 1 ,256, 2 )	{
	apart=0;
	};	
	virtual ScreenPart *newone(void) const override {return new char2(*this);}
	virtual int form2bin(const char *formal, unsigned char *bin, int &len) const override {	
		bin[0]=formal[0];
		return 2;	
		}
	virtual int bin2formal(unsigned char *bin, char *formal, int len) override {	
		if(isprint(bin[0]))
			formal[0]=bin[0];	
		else
			formal[0]='.';
		return 0;	}	
	virtual int isel(const int ch) const override {return isprint(ch);}
	};
datatype(char2)
parentd(ScreenPart,char2) 
class  char2b : public ScreenPart {	
	public:	 char2b ( Editor *ed,int x1=0, int y1=0): ScreenPart( ed, 1 ,256, 2 ,x1, y1,0)	{
	};	
	declareparent 
	virtual ScreenPart *newone(void) const override {return new char2b(*this);}
	virtual int formal2bin(const char *formal, unsigned char *bin, int len) const override {	
		bin[1]=formal[0];
		return 0;	
		}
	virtual int bin2formal(unsigned char *bin, char *formal, int len) override {	
		if(isprint(bin[1]))
			formal[0]=bin[1];	
		else
			formal[0]='.';
		return 0;	}	
	virtual int isel(const int ch)const  override {return isprint(ch);}
	};

datatype(char2b)
parentd(ScreenPart,char2b) 



