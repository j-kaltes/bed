#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define elm(name,p) {name,#p}
#define elk(name,key,p) {name,#p}
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
	const char *name;	
	const char *proc;
	};
struct namekey arrays[][30]={
#include "menu.h"
};
int arlen=sizeof(arrays)/sizeof(arrays[0]);
int main(void) {
	for(int i=0;i<arlen;i++) {
		const char *menuname=strchr(arrays[i][0].name,'~')+1;
		char fkey=tolower(*menuname);
		for(int j=1;arrays[i][j].name!=END;j++) {
			struct namekey *item=&arrays[i][j];
			if(item->name) {
				char name[256];
				const char *ptr;
				menuname=strchr(item->name,'~');
				int p,uit;
				ptr=item->name;
				char ikey=tolower(*(menuname+1));
				for(p=0,uit=0;ptr[p];p++) {
					if(ptr[p]=='~')
						continue;
					if(!isalnum(ptr[p])) {
						name[uit++]='\\';
						}
					name[uit++]=ptr[p];
					}
				name[uit]='\0';
				printf("s/^#%s$/#%s\\\n%s (Alt-%c,%c)\\\n/g\n",item->proc,item->proc,name,fkey,ikey);
				printf("s/prockey \\<%s\\>/Alt-%c,%c/g\n",item->proc,fkey,ikey);
				}
			}
		}

	return 0;
	}
