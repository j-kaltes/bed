       #include <string.h>

#include <fstream>
#include <iostream>
#ifdef  __cplusplus
extern "C" {
#endif
#include "regex.h"
#ifdef  __cplusplus
}
#endif

//#define MYSYNTAX (RE_NO_POSIX_BACKTRACKING| RE_CHAR_CLASSES | RE_HAT_LISTS_NOT_NEWLINE | RE_INTERVALS | RE_NEWLINE_ALT| RE_NO_BK_BRACES|RE_BACKSLASH_ESCAPE_IN_LISTS|RE_NO_BK_VBAR|RE_NO_BK_PARENS )
#define MYSYNTAX (RE_CHAR_CLASSES|RE_DOT_NEWLINE | RE_HAT_LISTS_NOT_NEWLINE | RE_INTERVALS | RE_NEWLINE_ALT| RE_NO_BK_BRACES|RE_BACKSLASH_ESCAPE_IN_LISTS|RE_NO_BK_VBAR|RE_NO_BK_PARENS )
using namespace std;
int main(int argc,char **argv) { 
if(argc!=3) {
		cerr<<"Usage: "<<argv[0]<<" pattern name"<<endl;return 1;
	}
char tablebase[256];
const char *regexerrorstring=NULL;
struct re_pattern_buffer searchexpr;
struct re_registers searchregs;		
memset(&searchexpr,'\0',sizeof( searchexpr));
memset(&searchregs,'\0',sizeof( searchregs));
re_set_syntax(MYSYNTAX);
searchexpr.translate=NULL;
searchexpr.fastmap=(char *)tablebase;
auto pattern= argv[1];
auto patlen=strlen(pattern);
if((regexerrorstring=re_compile_pattern(pattern,patlen, &searchexpr))) {
	cout<<"compil failed: "<< regexerrorstring<<endl;
	return 2;
	}
int size1=1024,size2=2048;
char buf1[size1],buf2[size2];
ifstream fp(argv[2]);
fp.read(buf1,size1);
fp.read(buf2,size2);
fp.close();
int ret;
if((ret=re_search_2(&searchexpr,buf1, size1, buf2, size2, size1-1, -size1+1,&searchregs, size1+size2))<0) {
	cout<<"re_match_2 failed "<<ret<<endl;return 3;
		}
auto start= searchregs.start[0];
auto end= searchregs.end[0];
cout<<"start "<<start<<" end "<<end<<endl;
if(start<size1) {
	if(end<size1) {
		int len=end-start;
		cout.write(buf1+start,len);
		}
	else {
		auto len =size1-start;
		cout.write(buf1+start,len);
		cout.write(buf2,end-(start+len));
		}
	}

return 0;
}
