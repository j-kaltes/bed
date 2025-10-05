/*
zo 31 dec 2017 16:14:41 CET Jaap Korthals Altes <binaryeditor@gmx.com>

Converts CHANGELOG to debian format to put in debian/changelog.


compile with:
g++ relog.cc -o relog -lre2

run:
debian/relog/relog CHANGELOG > debian/changelog


Change unstable and urgency by hand.
*/


#include <stdio.h>
#include <iostream>
#include "time.h"
#include <re2/re2.h>
#include <re2/stringpiece.h>
#include "pstream.h"
int main(int argc ,char **argv) {
using namespace re2;
using namespace std;
if(argc!=2) {
	cerr<<"Use "<<argv[0]<<" CHANGELOG\nConverts bed/CHANGELOG to debian/changelog\n";
	return 3;
	}
const char *  const patstr=R"(.*Version *([^ ,]*),*[  ]*([^0-9]*)([0-9]*) [       \(]*([^\)]*)[   \)]*)";
char *file=argv[1];
FILE *fp=fopen(file,"r");
fseek(fp,0,SEEK_END);
int end=ftell(fp);
fseek(fp,0,SEEK_SET);
char contents[end+1];
fread(contents,1,end,fp);
contents[end]='\0';
	RE2::Options opt;
//    opt.set_posix_syntax(true) ;
	opt.set_log_errors(false);
    RE2 pattern(patstr,opt);
if(!pattern.ok()) {
	cerr<<pattern.error().c_str()<<endl;
	exit(4);
	}
std::size_t args_count = pattern.NumberOfCapturingGroups()+1;
     StringPiece input(contents,end);    // Wrap a StringPiece around it
const char *nonmatch=contents;

StringPiece word[args_count];
/*
  if(RE2::PartialMatch(input, pattern,&word)) {
		cout<<word<<endl;
	}
*/
string min,max;
int iter=0,startpos=0;


constexpr int outlen=256;
char out[outlen];
int got=0;
for( startpos=0;startpos<end;) {

const char *start=input.data()+startpos;
const char * const endline=strchrnul(start,'\n');
int len=endline-start;
 if(pattern.Match(input, startpos, startpos+len, RE2::ANCHOR_BOTH , word, args_count) ) {
	if(got) {
		cout<<"\n -- Jaap Korthals Altes <binaryeditor@gmx.com>  "<<out<<endl;
		}
	static char buf[256]="date  -R --date=\'";
	static const int blen=strlen(buf);
	int siz=word[4].length();
	memcpy(buf+blen ,word[4].data(),siz);
	buf[blen+siz]='\'';
	buf[blen+siz+1]='\0';
	redi::ipstream  dat(buf);
	dat.read(out,outlen);
	 got= dat.gcount();
	out[got]='\0';
	 dat.close();
	cout<<"bed ("<<word[1]<<") unstable; urgency=medium\n\n";
	}
else {
	cout<<"  * ";
	if(*start=='-')
		while(isspace(*++start))
			;
	cout.write(start,endline-start);
	cout<<'\n';
	};
startpos+=(len+1);
}
if(got) {
	cout<<"-- Jaap Korthals Altes <binaryeditor@gmx.com>  "<<out<<endl;
	}
return 0;
}
