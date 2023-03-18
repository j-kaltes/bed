
#define Signed OldSigned
#define signed oldsigned
class  Signed : public General {	
	char frombin[20];
	public:	 
		declareparent 
	Signed ( Editor *ed,int b,int x1=0, int y1=0);	
	Signed ( Editor *ed,int x1=0, int y1=0);
	Signed ( Signed const &sig);			
	virtual int format2bin(const char *const formal, unsigned char *bin, int len) const override; 

	virtual int chbytes(int b) override; 
	virtual ScreenPart *newone(void) const override ;
	virtual int bin2formal(unsigned char *bin, char *formal, int len) override;	
	virtual int isel(const int ch) const override ;
virtual int userconfig(int kind) override ;
	};
