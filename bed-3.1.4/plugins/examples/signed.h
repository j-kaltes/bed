#include "../multidigit.h"
#include "digitstring.h"
//#define Signed NewSigned
//#define signed newsigned
class  Signed : public Digit {	
	int twobit1;
	mint twoton;
	public:	 
		declareparent 
	Signed ( Editor *ed,int b=4,int x1=0, int y1=0);	
//	Signed ( Editor *ed,int x1=0, int y1=0);
	Signed ( Signed const &sig);			
	~Signed();			
	virtual int form2bin(const char *formal, unsigned char *bin, int &len) const  override;	
/*
	int chbytes(int b) override; 
	short chbase(short base) override ;
*/
	virtual int calcsizes(void) override;
virtual	ScreenPart *newone(void) const override ;
virtual	int bin2formal(unsigned char *bin, char *formal, int len) override;	
virtual	int isel(const int ch) const  override;
	};
