#include "global.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
BEDPLUGIN
class Editor {
#include "faked.h"
public:
	
int	helpf1(void) {
	message("Press F1 in menu and dialogs to get help, Alt-X or Esc,X to open menu (highlighted X)");
	return 0;
	}
};




call_on_start(helpf1)
