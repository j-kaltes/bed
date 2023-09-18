#include "global.h" 
#include "screenpart.h"
#include "dialog.h"
#include "screen.h"
BEDPLUGIN

class Editor {
public:
#include "faked.h"
int mkline(ScreenPart *sub,char *uit,int maxuit) {
	int have=snprintf(uit,maxuit,"%s size %d (datatype %d)",sub->getname(),sub->getbytes(),sub->datbytes());
	if(sub->type!=0&&sub->type!=256)
		have+=snprintf(uit+have,maxuit-have, " base: %d" ,sub->type);
	have+=sub->mkfilterstr(uit+have,maxuit-have);
	const char *userl=(sub->userlabel?sub->userlabel:"");
	have+=snprintf(uit+have,maxuit-have," %s",userl);
	return have;	
	}
int datatypeinfo(void) {
	ScreenPart *super=activemode(),*sub;
	int nr=1;
	while((sub=super->getchild())) {
		super=sub;	
		nr++;
		}
char buf[thecols];
int res;
do {
	res=0;
	sub=super;
	mkline(sub,buf,thecols);
	message("%s",buf);
	erefresh();
	if(nr<2) {
		return 0;
		}
	begindialog((nr+4),thecols,2);
	    mvwaddstr(takescr,nr,2,buf);
	   
		for(int i=1;i<nr;i++) {
			sub=sub->getparent();
			if(sub) {
				mkline(sub,buf,thecols);
				 mvwaddstr(takescr,nr-i,2,buf);
				 }
			else {
				 mvwaddstr(takescr,nr-i,2,"Error");
				}
			}
		   oke(nr+2,thecols/2-2);
		   enddialog(res,res);
			
	} while(res==INT_MAX&&(resized(),rewriteall(),1));
filesup();
return 0;     
}
};


sethelp(datatypeinfo, "Gives information on active datatype.")

call(datatypeinfo)

