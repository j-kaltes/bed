
#include "global.h"
BEDPLUGIN
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include "../filter.h"
#include "../screenpart.h"
#include "../dialog.h"
#include "../screen.h"
ScreenPart *getFloat(Editor *ed);
extern char *filtername(Multiply) ;
//template <class FT,class BT>
class Multiply: public Filter {
        typedef float FT;
        typedef uint64_t BT;
	#define FLOATSTR "lf"
	#define MULTISTR "multiply"
	inline static constexpr int mulsize=sizeof(MULTISTR);
	double multiplier;
	void mklabel(double div) {
		constexpr int maxstr=mulsize+50;
		label=new char[maxstr];
		snprintf(label,maxstr,MULTISTR " %.*" FLOATSTR,DECIMAL_DIG, multiplier);

		}
	public:
	Multiply(double multiplier,int fb,int sb) : Filter(fb,sb,filtername(Multiply)),multiplier(multiplier){
		mklabel(multiplier);
		}
	Multiply(int fb,int sb,const char *conv) : Filter(fb,sb,filtername(Multiply)) {
		sscanf(conv,"%" FLOATSTR,&multiplier);
		mklabel(multiplier);
		}
	Multiply(Multiply const &rev): Multiply(rev.bytesfile,rev.bytesscreen,rev.multiplier) {
		
		}
	char *convstr(void)  const override {
		return label+mulsize;
		}
	~Multiply() {
		delete[] label;
		}
	virtual Filter *newone(void)const  override {
		return new Multiply(*this);
		}

	virtual int chbytes(int b) override {
	//	bytesfile=b;
//		bytesscreen=4;
		return bytesscreen;
		}
	virtual int chbytesback(int b)  override {
//		bytesfile=2;
	//	bytesscreen=b;
		return bytesfile;
		}

	virtual int fromfile(const unsigned char * const input,unsigned char *const output) const override {
		BT tus=0;	
		memcpy((unsigned char *)&tus,input,bytesfile);
		FT out=tus*multiplier;
		memcpy(output,&out,bytesscreen);
		return bytesscreen;
		}
	virtual int tofile(const unsigned char * const input,unsigned char * const output)  override  {
		const FT *in=reinterpret_cast<const FT *>(input);
		BT out=BT(roundf(*in/multiplier));
		memcpy(output,&out,bytesfile);
		return bytesfile;
		}
	int  equal(const Filter *fil) const override {
		if(!Filter::equal(fil))
			return 0;
		if(multiplier!=((Multiply *)fil)->multiplier)
			return 0;
		return 1;
		}
	};

registerfilterclass(Multiply) 



class Editor {
public:
#include "faked.h"
int multiply(void) {
	constexpr const int maxliner=80;
	int wlin=12,wcol=29,res=0;
	char multi[maxliner]="1",bytes[maxliner]="4";
	while(1) {
		static int separ=1;
		begindialog(wlin,wcol,7)
		linernrmax("~Multiply with:  ",multi,2,3,5,maxliner);
		linernrmax("~Number of bytes:",bytes,4,3,5,maxliner);
		optselect(separ,!separ, opt("~Add field",6,3,1), opt("~Separate",7,3,0));
		oke(9,3);
		act("~Cancel", 9,17);
		enddialog(res,0)


		if(!res||res==6) { 
			filesup(); 
			return -1; 
			}	
		if(res==INT_MAX)  {
			erefresh();
			continue;
			}
		int byt=atoi(bytes);
		if(byt>8)
			message("Number of bytes should not be lager than 8");
		double mul=atof(multi);
		Multiply *ad=new Multiply(mul,byt,4);
		if(ad->error()) {
			message("Can't add filter");
			delete ad;
			erefresh();
			continue;
			}
		ScreenPart *fl=getFloat(this);
  		fl->addfilter(ad);
		modechanged();
//		res=installpart(fl);
		constexpr int maxbuf=50;
		char buf[maxbuf];
		snprintf(buf,maxbuf,"%d bytes unsigned times %lg",byt,mul);
		  fl->setuserlabel(buf) ;

		 res=installpart(fl, separ);
		if(res<0)		{
			filesup(); 
			message("Unknown error");
			return -1;
			}
			
		break;
		}
	return 0;
	}

};

call(multiply)
sethelp(multiply,"Applied to float datatype. Integer in file is multiplied with float and displayed as float.")
