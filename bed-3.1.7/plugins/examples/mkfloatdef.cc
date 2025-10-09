
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxlen 1000000

#define mkstr(x) "%0" #x "." #x "Lg\n"
#define form(x) mkstr(x)
int printlb(long double *l,char uit[],int len) {
const char printstring[]= form(maxlen);
return snprintf(uit,len,printstring,*l);
}
int main() {
//unsigned int get[4]={0xFF0FABCDLL,0x123F42D1LL,0xFFFaa34f,0};
unsigned int get[4]={0x71C27366LL, 0xFFF46C06LL, 0x00003FFELL,0LL};
//double long *l=reinterpret_cast<double long *> (get);
double long *l=(double long *) get;
char small[maxlen];
char large[maxlen];
printf("#define SIZEOF_LONG_DOUBLE %zd\n",sizeof(long double));
get[3]=0;
printlb(l,small,maxlen);
get[3]=0xFFAA1234;
printlb(l,large,maxlen);

///printf("small=%s\nlarge=%s\n",small,large);
if(strcmp(small,large)) {
	puts("#define REALLONGDOUBLE16");
	exit(0);
	}
	
puts("#undef REALLONGDOUBLE16");
exit(0);
}
