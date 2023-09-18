#include <stdio.h>
#define helpteksten(file,proc,text) {file,#proc,text},
struct {
	char *file;
	char *name;
	char *text;
} data[]={
#include HELP_H
};
int datalen=sizeof(data)/sizeof(data[0]);
int main(void) {
	int i;
	for(i=0;i<datalen;i++) {
		printf("#%s\n",data[i].name);
		puts(data[i].text);
		printf("plugin %s\n",data[i].file);
		}
	return 0;
	}
