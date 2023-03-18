

#include "src/confdefines.h"
#ifndef OWNTRANSTIME 
#define _XOPEN_SOURCE 
#include <time.h>
#include <stdio.h>
#include <string.h>
extern int retime(FILE *defs)  ;
//static char timeformat[]="%a %b %e %H:%M:%S %C%y";
static char timeformat[]="%a %b %e %H:%M:%S ";

int retime(FILE *defs) {
	time_t bin;
	struct tm *tim;
	struct tm uit;	
	int maxres=100;
	char formal[maxres];
	char formaluit[maxres];
	bin=time(NULL);
	tim=gmtime(&bin);
 	strftime(formal,maxres,timeformat,  tim);
	memset(&uit,'\0',sizeof(uit));
	strptime(formal, timeformat, &uit);
 	strftime(formaluit,maxres,timeformat,  &uit);

fputs("Checking if strptime reverses strftime... ",stdout);
	if(strcmp(formaluit,formal)) {
		fputs("#define OWNTRANSTIME 1\n",defs);
puts("no");

		return(3);		
		}
	else {
puts("yes");
		fputs("/*#undef OWNTRANSTIME*/\n",defs);
		return(0);
		}
	}
#endif

