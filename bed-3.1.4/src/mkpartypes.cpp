#include <stdio.h>
#include "sys_types.h"

int main(void) {
int end=sizeof(msdos_systypes)/sizeof(msdos_systypes[0]);
//printf("end=%d\n",end);
const char *ptypes[256];
for(int i=0;i<0x100;i++)
	ptypes[i]=NULL;
for(int iter=0;iter<end;iter++) {
	unsigned char ty=msdos_systypes[iter].type;
	const char *name= msdos_systypes[iter].name;
	if(name)
		ptypes[ty]=name;
	}

for(int i=0;i<0x100;i++) {
	printf("Part_t(0x%X,",i);
	if(ptypes[i])
		printf("\"%s\"",ptypes[i]);
	else
		printf("NULL");
	printf("),\n");
	}
}


