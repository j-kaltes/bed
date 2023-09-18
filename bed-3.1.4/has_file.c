#include "src/defines.h"
#include <stdio.h>
int main(void) {
#ifdef HAS_FILE
	puts("y");
return 0;
#else
	puts("n");
return 1;
#endif
}
