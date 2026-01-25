#include "global.h" 
#include "config.h"
BEDPLUGIN
ScreenPart *getDigit(Editor *ed);
ScreenPart *getFloat(Editor *ed);
class Editor {
#include "faked.h"
public:
	
int	tryinstall1(void) {
	ScreenPart *fl=getFloat(this);
	installpart(fl);
	return 0;
	}
int tryinstall2(void) {
	ScreenPart *par=getDigit(this);
	installpart(par,0);
	return 0;
	}
};

sethelp(tryinstall1,"Installs Float with installpart/1 and getFLoat")
sethelp(tryinstall2,"Installs Digit with installpart(part,0) and getDigit")

call(tryinstall1)
call(tryinstall2)
