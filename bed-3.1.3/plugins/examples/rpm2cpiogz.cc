
#include "global.h" 
#include "screenpart.h"
#include "dialog.h"
#include "screen.h"
BEDPLUGIN

class Editor {
public:
#include "faked.h"

int rpm2cpio(void) {
	char zoek[]={0x1F,0x8B,0x08,0x00,0x00,0x00,0x00};
	topos(0);

	OFFTYPE pos=dosearch(zoek,sizeof(zoek));
	if(pos==OFFTYPEINVALID) {
		message("Couldn't find string");
		return -1;
		}
	topos(pos);
	setselect();
	endfile();
	savesel("bed.cpio.gz");
	quit();
	return 0;
	}
};

call(rpm2cpio)
