#include <ctype.h>
#include <string.h>
#include "global.h"
BEDPLUGIN
#include "screenpart.h"
class  NewDataType : public ScreenPart {	
	public:	 
		declareparent 
	 NewDataType ( Editor *ed):ScreenPart(ed,1,256,1) {
	 	apart=0;
	 	};
	virtual int form2bin(const char *formal, unsigned char *bin, int &len) const override {	
		memmove((char *)bin,formal,len);
		return len;
		}
	virtual ScreenPart *newone(void) const override {
		return new NewDataType(*this);
		}
	virtual int bin2formal(unsigned char *bin, char *formal, int len) override {	
		for(int i=0;i<len;i++) {
			formal[i]=0x7F&bin[i];
			if(formal[i]<=32)
				formal[i]='.';
			}
		return 0;
		}
	virtual int isel(const int ch) const override {return isprint(ch);}
	};
parentd(ScreenPart,NewDataType) 
datatype(NewDataType)
