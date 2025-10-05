      #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
       #include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *getfilegeg(char *name,int *len) {
int han=open(name,O_RDONLY);
int size=lseek(han,0,SEEK_END);
lseek(han,0,SEEK_SET);
char *buf=new char[size+1];
read(han,buf,size);
buf[size]='\0';
close(han);
*len=size;
return buf;
	}
#define error(...) fprintf(stderr,__VA_ARGS__)
int main(int argc,char **argv) {
if(argc!=3) {
	error("Usage: 	%s manpage googlegeg\nadds google Analytics\n",argv[0]);exit(2);

	}
	int len;
char *buf=getfilegeg(argv[1],&len),*end=buf+len;
int outhan= fileno(stdout)	;
for(char *ptr=buf;ptr<end;ptr++) {
	if(!(ptr=strchr(ptr,'\n'))) {
		break;
		}
	
	if(!memcmp(++ptr,"</HEAD>",7)) {
		int size;
		char *google=getfilegeg(argv[2],&size);	
		write(outhan,buf,ptr-buf);
		write(outhan,google,size);
		write(outhan,ptr,end-ptr);
		exit(0);
		}
	
	}
error("dont find </HEAD>\n");exit(3);
}
