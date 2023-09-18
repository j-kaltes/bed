#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std; 


void printtime(time_t t) {
	cout<<"GMT:		"<<asctime(gmtime(&t));
	cout<<"Localtime:	"<<ctime(&t);
}
int main(int argc,char ** argv) {
//setenv( "TZ","Europe/Amsterdam",1);
setenv("TZ","Europe/London",1);
if(argc!=1) 
	printtime(atoi(argv[1]));
else {
	time_t winter=1583843230,summer=1586863098;
	printtime(winter);
	cout<<endl;
	printtime(summer);
	}

}
