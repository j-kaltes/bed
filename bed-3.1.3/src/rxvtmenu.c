#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define elm(name,p) {name,NULL}
#define elk(name,key,p) {name,key}
#define LINE {NULL,NULL}
#define END (char *)1
#define ENDSTRUCT {END,NULL}
#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define menumake(file,Name,el...) {{Name,NULL},  	##el,ENDSTRUCT}, VARMACRO
#else
#define menumake(file,Name,...) {{Name,NULL},  	__VA_ARGS__,ENDSTRUCT},
#endif
struct namekey {
	char *name;	
	char *key;
	};
struct namekey arrays[][30]={
#include "menu.h"
};

char rmchar(char *head,char *uit) {
	char *next=memccpy(uit,head,'~',80);
	strcpy(next-1,head+(next-uit));
	return *(next-1);
	}

int main(void) {
int i,nr=sizeof(arrays)/(sizeof(struct namekey)*30);
for(i=0;i<nr;i++) {
	int j;
	char uit[80];
	char *head=arrays[i][0].name;
/*	char op=toupper(rmchar(head,uit)); */
	char op=tolower(rmchar(head,uit));
	printf("/%s/*\n",uit);
	for(j=1;arrays[i][j].name!=END;j++) {
		if(arrays[i][j].name) {
			char la=tolower(rmchar(arrays[i][j].name,uit));
			if(arrays[i][j].key)
				printf("{%s}{%s} M-%c%c\n",uit,arrays[i][j].key,op,la);
			else
				printf("{%s} M-%c%c\n",uit,op,la);
			}
		else
			puts("{-}");
		}
	}

return 0;
};
