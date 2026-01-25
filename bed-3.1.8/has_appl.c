#include "src/defines.h"
#include <stdio.h>
int main(void) {
#ifdef HAS_APPLICATIONS
puts(HAS_APPLICATIONS);
if(HAS_APPLICATIONS[0]!='\0')
	return 0;
#endif
return 1;
}
