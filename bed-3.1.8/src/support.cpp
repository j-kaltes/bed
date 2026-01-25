#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __BORLANDC__
#include <io.h>

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#ifdef _Windows
#define _WIN32
#endif
#ifdef __WIN32__
#define _WIN32
#endif
#ifdef _WIN32
#include <windows.h>
char windowstempdirbuf[MAX_PATH];
char *windowstempdir=(GetWindowsDirectory(windowstempdirbuf,MAX_PATH)?(strcat(windowstempdirbuf,"\\Temp"),windowstempdirbuf):NULL);

#endif
#include "system.h"
#include "win.h"
int writable(char *name) {
	int ret=0;
	char ch='x';
	int handle=creat(name,0644);
	if(handle<0)
		return 0;
	if(write(handle,&ch,1)!=1)
		ret=0;
	else 
		ret=1;
	close(handle);
	unlink(name);
	return ret;
}
extern const char *tmpdirs[];
extern const int tmpdirnr;
extern int tmpdirlens[];
#ifdef _WIN32
 const char *tmpdirs[]={getenv("TMP"),getenv("TEMP")
#ifdef TEMPDIR
,TEMPDIR
#endif
,"/cygdrive/c/tmp","/cygdrive/c/temp",windowstempdir,"/cygdrive/c/Windows/Temp",P_tmpdir,getenv("HOMEPATH"),"/tmp"};
#else
#ifdef  __ANDROID__ 
 const char *tmpdirs[]={getenv("TMPDIR")
#ifdef TEMPDIR
,TEMPDIR
#endif
,"/data/local/tmp","/sdcard","/sdcard/Android/data","~",CONFDIR,CONFDIRLINK};
#else
 const char *tmpdirs[]={getenv("TMPDIR")
#ifdef TEMPDIR
,TEMPDIR
#endif
,"~/tmp",P_tmpdir,"/tmp","/var/tmp","/usr/local/tmp"};
#endif
#endif
 const int tmpdirnr=(sizeof(tmpdirs)/sizeof(char*));
 int tmpdirlens[tmpdirnr]={};
#include "myalloc.h"
int  namedir(char *buf,int i,const char *name) {
	const char *tmp=tmpdirs[i];
	if(tmp) {
		int len;
		if(tmp[0]!='/'|| (!tmpdirlens[i]&& !(tmpdirlens[i]=strlen(tmp)))) {
	
//#ifdef _WIN32
#ifdef __CYGWIN__
			absunixpath(tmp,buf);
#else
#ifdef HAS_REALPATH
		if(!realpath(tmp,buf))
			return 0;
#else
			expandfilename(buf,tmp);
#endif
#endif
			len=strlen(buf);
			tmpdirlens[i]=len;
			char *ptr= myallocar(char,len+1);
			memcpy(ptr,buf,len+1);
			tmpdirs[i]=ptr;
			}
		else {
			len=tmpdirlens[i];
			memcpy(buf,tmp,len);
			}
//		 buf[len++]=FILENAMESEP[0];
		 buf[len++]='/';
		strcpy(buf+len,name);
		return len;
		}
	else 
		return 0;
	}
int mytmpname(char *buf,int len,const char *name) {
	for(int i=0;i<tmpdirnr;i++) {
			if(int ret=namedir(buf, i,name) ) {
				if(mktemp(buf)&&writable(buf))
					return ret;
				}
			}
//#ifdef _WIN32
#ifdef  __CYGWIN__
char tmp[len];
if(DWORD plen= GetTempPath(len,tmp)) {
	strcpy(tmp+plen,name);
	absunixpath(tmp,buf);
	if(mktemp(buf)&&writable(buf))
		return plen;
	}

#endif
	strcpy(buf,name);
	if(mktemp(buf)&&writable(buf))
		return 1;
	if(tmpnam(buf) &&writable(buf))
		return 1;
	 return -1;
	}
#ifdef TEST
void main(void) {
char buf[256];
mytmpname(buf,256,"bed-XXXXXX");
puts(buf);
}


#endif
